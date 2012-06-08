#include <vector>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "Triangle.h"
#define PPMREADBUFLEN 256

using namespace std;

class MeshObject : public Geometry{

	vector<vec3>   vertices;
	vector<vec2>   textures;
	list<vec2>     texture_points;
	vector<vec3>   normals;
	list<Triangle> faces;
	vector<vec3>   texture_colors;
	
	vec3 color;
	double ambient, diffuse, specular, cos_power, reflectance, transmit, iof;
	mat4 transform_mat, dir_mat, translate_mat, scale_mat, rotate_mat;
	int type, texture_width, texture_height, color_depth;
	bool textured;

	public:
	
		MeshObject(string filename);
		void  objectSetup(string setting, list<string>args);
		void  setTexture(string filename);
		void  setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index);
		vec3   getColor()const;
		double getAmbient()const;
		double getDiffuse()const;
		double getSpecular()const;
		double getCosPower()const;
		double getReflectance()const;
		double getTransmitance()const;
		void   printSize()const;
		void   setTransform(double sx, double sy, double sz, double tx, double ty, double tz, double rx, double ry, double rz);
		void   printType()const;
		bool   colision(Ray ray, double &t, vec3 &normal);
		bool   colision(Ray ray)const;
		Ray    transform(Ray ray) const;
		
};

MeshObject::MeshObject(string filename){

	textured = false;
	string line;
	string word;
	ifstream file;

	map<string,list<string> > inputs;
	
	file.open(filename.c_str());
	char ch;
	string setting;
	bool first_word = true;
	list<string> args;

	while(!file.eof()){
		file.get(ch);
		//Skip comments and newlines	
		if(ch != '#' & ch != '\n'){
			setting = ch;	
			file.get(ch);

			//Get the arguements that go with the setting
			while(ch != '\n'){

				switch(ch){
					case ' ':
						//cout << word << " ";
						if(!first_word){
							args.push_back(word);
							word.clear();
							file.get(ch);
						}
						else{
							first_word=false;
							file.get(ch);
						}
						break;
					case '/':
						args.push_back(word);
						word.clear();
						file.get(ch);
						break;	
					default:
						if(first_word){
							setting.push_back(ch); file.get(ch);
						}
						else{
							word.push_back(ch);
							file.get(ch);
						}
						break;
				}
			}
			args.push_back(word);
			objectSetup(setting,args);
			args.clear();
			word.clear();
			setting.clear();
			first_word=true;
		}
		else if(ch == '#'){
			getline(file,line);
		}
	}	

	file.close();
}

void MeshObject::setTexture(string filename){

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

void MeshObject::printSize()const{
	printf("Vector size: %d\n",(int)texture_colors.size());
}

//Handle different lines of object file
void MeshObject::objectSetup(string setting, list<string> args){

	list<string>::iterator iter = args.begin();
	
	if(setting.compare("v")==0){
		double a = atof(iter->c_str());		
		++iter;
		double b = atof(iter->c_str());		
		++iter;
		double c = atof(iter->c_str());		
		vertices.push_back(vec3(a,b,c));
	}
	else if(setting.compare("vt")==0){
		double a = atof(iter->c_str());		
		++iter;
		double b = atof(iter->c_str());		
		textures.push_back(vec2(a,b));
	}
	else if(setting.compare("vn")==0){
		double a = atof(iter->c_str());		
		++iter;
		double b = atof(iter->c_str());		
		++iter;
		double c = atof(iter->c_str());		
		normals.push_back(vec3(a,b,c));
	}
	else if(setting.compare("f")==0){
		int a,b,c;
		a = atoi(iter->c_str());
		++iter;
		//No texture coords or normals
		if(args.size()==3){
			b = atoi(iter->c_str());
			iter++;
			c = atoi(iter->c_str());
			//printf("Tri%d, %d, %d\n",a,b,c);
			faces.push_back(Triangle(vertices[a-1],vertices[b-1],vertices[c-1]));		
		}
		//Only vertices and texture coords
		else if(args.size()==6){
			int ta,tb,tc;
			ta = atoi(iter->c_str());
			++iter;
			b = atoi(iter->c_str());
			iter++;
			tb = atoi(iter->c_str());
			++iter;
			c = atoi(iter->c_str());
			++iter;
			tc = atoi(iter->c_str());
			faces.push_back(Triangle(vertices[a-1],vertices[b-1],vertices[c-1]));
			texture_points.push_back(textures[ta-1]);
			texture_points.push_back(textures[tb-1]);
			texture_points.push_back(textures[tc-1]);
		}
		else if(args.size()==9){
			int ta,tb,tc;
			int na,nb,nc;
			bool textured;
			if((*iter).compare("")==0){
				textured = false;
			}
			ta = atoi(iter->c_str());
			++iter;
			na = atoi(iter->c_str());
			++iter;
			b = atoi(iter->c_str());
			iter++;
			tb = atoi(iter->c_str());
			++iter;
			nb = atoi(iter->c_str());
			++iter;
			c = atoi(iter->c_str());
			++iter;
			tc = atoi(iter->c_str());
			++iter;
			nc = atoi(iter->c_str());

			Triangle t;
			if(!textured){
				t = Triangle(vertices[a-1],vertices[b-1],vertices[c-1]);
			}
			else{
					texture_points.push_back(textures[ta-1]);
					texture_points.push_back(textures[tb-1]);
					texture_points.push_back(textures[tc-1]);
			}
			t.setNormal(normals[na-1]);
			faces.push_back(t);
		}
	}
}

void MeshObject::setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index){
	type      = t;
	color     = vec3(r,g,b);
	ambient   = ka;
	diffuse   = kd;
	specular  = ks;
	cos_power = n_s;
	reflectance   = refl;
	transmit  = trans;
	iof       = index;
}

vec3 MeshObject::getColor()const{
	return color;
}
double MeshObject::getAmbient()const{
	return ambient;
}
double MeshObject::getDiffuse()const{
	return diffuse;
}
double MeshObject::getSpecular()const{
	return specular;
}
double MeshObject::getCosPower()const{
	return cos_power;
}
double MeshObject::getReflectance()const{
	return reflectance;
}
double MeshObject::getTransmitance()const{
	return transmit;
}

//Set up transform matrices for mesh object
void MeshObject::setTransform(double sx, double sy, double sz, double tx, double ty, double tz, double rx, double ry, double rz){
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

	//Final transform matrix
	transform_mat = scale_mat * rotate_mat * translate_mat;
	dir_mat = scale_mat * rotate_mat;
}

void MeshObject::printType()const{
	printf("Mesh Object\n");	
}

//Colision test that stores distance and normal in argument variables
bool MeshObject::colision(Ray ray, double &t, vec3 &normal){

	int n=0;	
	ray = transform(ray);	
	double tmp_t;
	int x,y;
	vec2 point;

	for(list<Triangle>::const_iterator iter = faces.begin(); iter!=faces.end(); ++iter){
		tmp_t = iter->colision(ray,point);
		if(tmp_t>=0){
			t = tmp_t;
			
			//Set color using texture coord		
			if(textured){
				x = (int)(point.x * texture_width);
				y = (int)(point.y * texture_height);
				int index = (y * texture_width) + x;
				color = texture_colors[index]/(double)(color_depth+1);
			}

			normal = iter->getNormal();
			return true;
		}
		++n;
	}
	return false;
}

//Colision test where distance isn't important
bool MeshObject::colision(Ray ray)const{

	ray = transform(ray);	
	double t;
	vec2 point;
	for(list<Triangle>::const_iterator iter = faces.begin(); iter!=faces.end(); ++iter){
		t = iter->colision(ray, point);
		if(t>=0){
			return true;
		}
	}
	return false;
}

//Transform ray to object coordinates
Ray	MeshObject::transform(Ray ray) const{
	
	vec4 tmp = vec4(ray.getOrigin(), 1.0);
	tmp = mvmult( transform_mat,tmp );
	vec3 new_start = vec3(tmp.x,tmp.y,tmp.z);
	ray.setOrigin(new_start);
	
	tmp = vec4(ray.getDirection(),1.0);
	tmp = mvmult(dir_mat,tmp);
	vec3 new_dir = vec3(tmp.x,tmp.y,tmp.z);
	ray.setDirection(new_dir);
	return ray;
}
