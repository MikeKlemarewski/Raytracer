#include "Geometry.h"
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

class Sphere : public Geometry{

	vector<vec3> texture_colors;
	vec3 center;
	vec3 color;
	double radius;
	double ambient, diffuse, specular, cos_power, reflectance, transmit, iof;
	int type, texture_width, texture_height, color_depth;
	mat4 transform_mat, dir_mat, translate_mat, scale_mat, rotate_mat;
	bool textured;

	public:
		Sphere();
		Sphere(double x, double y, double z, double r);
		void setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index);
		void   setTexture(string filename);
		vec3   getCenter();
		double getRadius();
		vec3   getColor()const;
		double getAmbient()const;
		double getDiffuse()const;
		double getSpecular()const;
		double getCosPower()const;
		double getReflectance()const;
		double getTransmitance()const;
		void   printSize()const;
		void   setTransform(double sx, double sy, double sz, double tx, double ty, double tz, double rx, double ry, double rz);
		void   setRotate(double x, double y, double z) const;
		void   printType()const;
		int    doubleCompare(double a, double b)const;
		bool colision(Ray ray, double &t, vec3 &normal);
		bool colision(Ray ray)const;
		Ray transform(Ray ray)const;
};
 
Sphere::Sphere(){

}
	
Sphere::Sphere(double x, double y, double z, double r){
	center = vec3(x,y,z);
	radius = r;
}

void Sphere::setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index){

	type        = t;
	color       = vec3(r,g,b);
	ambient     = ka;
	diffuse     = kd;
	specular    = ks;
	cos_power   = n_s;
	reflectance = refl;
	transmit    = trans;
	iof         = index;

}

void Sphere::setTexture(string filename){

	string line;
	string w,h,d;
	char ch;
	ifstream file;
	
	file.open(filename.c_str(),ifstream::binary);
	
	getline(file,line);
	file.get(ch);
	while(ch == '#'){
		getline(file,line);
		file.get(ch);	
	}	

	while(ch != ' '){
		w.append(1,ch);
		file.get(ch);	
	}	
	file.get(ch);
	while(ch != '\n'){
		h.append(1,ch);
		file.get(ch);	
	}
	file.get(ch);
	while(ch != '\n'){
		d.append(1,ch);	
		file.get(ch);	
	}
	texture_width = atoi(w.c_str());	
	texture_height = atoi(h.c_str());	
	color_depth = atoi(d.c_str());

	//The following lines(until file.close()) were taken from a file provided on stack overflow
	//I was having trouble dealing with reading the binary data into integer values
	//Link: http://stackoverflow.com/questions/882621/read-ppm-images-in-c		
	unsigned char *charImage = (unsigned char *) new unsigned char [3*texture_width*texture_height];
	file.read( reinterpret_cast<char *>(charImage), (3*texture_width*texture_height)*sizeof(unsigned char));
	
	int r,g,b;

	for(int i=0; i < texture_height; i++){
 		for(int j=0; j < 3*texture_width; j+=3) {
    		r = (int)charImage[i*3*texture_width+j];
    		g = (int)charImage[i*3*texture_width+j+1];
    		b = (int)charImage[i*3*texture_width+j+2];
			texture_colors.push_back(vec3(r,g,b));
		}
	}

	file.close();
	textured = true;
}

void Sphere::printSize()const{
	printf("Vector size: %d\n",(int)texture_colors.size());
}

vec3 Sphere::getCenter(){
	return center;
}

double Sphere::getRadius(){
	return radius;
}

vec3 Sphere::getColor()const{
	return color;
}

double Sphere::getAmbient()const{
	return ambient;
}
double Sphere::getDiffuse()const{
	return diffuse;
}
double Sphere::getSpecular()const{
	return specular;
}

double Sphere::getCosPower()const{
	return cos_power;
}

double Sphere::getReflectance()const{
	return reflectance;
}

double Sphere::getTransmitance()const{
	return transmit;
}

void Sphere::setTransform(double sx, double sy, double sz, double tx, double ty, double tz, double rx, double ry, double rz){
	
	//Create inverse translation matrix
	vec4 a = vec4(1.0,0.0,0.0,-tx);
	vec4 b = vec4(0.0,1.0,0.0,-ty);
	vec4 c = vec4(0.0,0.0,1.0,-tz);
	vec4 d = vec4(0.0,0.0,0.0,1.0);
	translate_mat = mat4(a,b,c,d);
	
	//Create inverse scale matrix
	scale_mat = Scale(1.0/sx,1.0/sy, 1.0/sz);
	
	//Create inverse rotation matrix
	mat4 rotate_x = RotateX(-rx);	
	mat4 rotate_y = RotateY(-ry);	
	mat4 rotate_z = RotateZ(-rz);	
	
	rotate_mat = rotate_x * rotate_y * rotate_z;

	transform_mat = scale_mat * rotate_mat * translate_mat;
	dir_mat = scale_mat * rotate_mat;
}
void Sphere::printType()const{
	printf("Sphere\n");
}

int Sphere::doubleCompare(double a, double b)const{
	if(a - b < 0.00001){
		if(b - a < 0.00001){
			return 0;
		}
		return -1;
	}
	return 1;
}

bool Sphere::colision(Ray ray, double &t, vec3 &normal){

	ray = transform(ray);
	vec3 dist = ray.getOrigin() - center;
	
	double a = dot(ray.getDirection(), ray.getDirection());
	double b = 2*dot( ray.getDirection(), dist );
	double c = dot( dist, dist ) - radius*radius;

	//Return false if ray starts on edge of sphere.
	//Implement better way of checking this
	if(doubleCompare(c,0.0)==0){
		return false;
	}
	double discriminant = b*b - 4*c;
	// first check to see if ray intersects sphere
	if ( discriminant > 0 ){
		discriminant = sqrt( discriminant );
		double t0 = (- b - discriminant) / 2;
		double t1 = (- b + discriminant) / 2;

		if(t0>0){
			t=t0;
		}
		else{
			t=t1;
		}
		vec3 hitPos = ray.getOrigin() + (t * ray.getDirection());	
		normal = normalize(hitPos - center);

		//Texture map spheres 
		if(textured){
			double tu = asin(normal.x)/M_PI * 0.5;
			double tv = asin(normal.y)/M_PI * 0.5;
			int x = (int)(normal.x * texture_width);
			int y = (int)(normal.y * texture_height);
			int index = (y * texture_width) + x;
			color = texture_colors[fabs(index)]/(double)(color_depth+1);
		}

		return true;
	}
	return false;
}

bool Sphere::colision(Ray ray)const{

	ray = transform(ray);
	vec3 dist = ray.getOrigin() - center;
	
	double a = dot(ray.getDirection(), ray.getDirection());
	double b = 2*dot( ray.getDirection(), dist );
	double c = dot( dist, dist ) - radius*radius;
	if(doubleCompare(c,0.0)==0){
		return false;
	}
	double discriminant = b*b- 4*a*c;
	// first check to see if ray intersects sphere
	if ( discriminant > 0 ){
		return true;
	}
	return false;
}

Ray Sphere::transform(Ray ray)const{
	
	vec4 tmp = vec4(ray.getOrigin(), 1.0);
	tmp = mvmult( transform_mat,tmp );
	vec3 new_start = vec3(tmp.x,tmp.y,tmp.z);
	ray.setOrigin(new_start);
	
	tmp = vec4(ray.getDirection(),1.0);
	tmp = mvmult(dir_mat,tmp);
	vec3 new_dir = normalize(vec3(tmp.x,tmp.y,tmp.z));
	ray.setDirection(new_dir);
	return ray;
}
