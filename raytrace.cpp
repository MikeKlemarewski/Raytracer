#include "SceneBuilder.h"
#include <time.h>

#define COLOR_DEPTH 255
#define RAD_TO_DEGREE 180/M_PI

using namespace std;

clock_t start, end;
bool    anti_aliased = false;
bool    dof          = false;

double fovx, fovy;
list<vec3> pixels;
string output_file;

int width, height;

//Camera settings
vec3   cam_pos;
vec3   cam_up;
vec3   cam_lookat;
double cam_angle;
int    cam_depth;

vec3   background_color;
double z_depth;

//Scene objects
list <Geometry*> shapes;
list <Light>     lights;
vec3             ambient_light;

//Create the final ppm image file
void makePPM(string filename){

	int color_depth = COLOR_DEPTH;

	ofstream file (filename.c_str());
	file << "P3\n" << "#" << filename << endl << width << " " << height << endl << color_depth << endl;
	
	for(list<vec3>::iterator iter = pixels.begin(); iter!=pixels.end(); ++iter){
		file << (int)iter->x << " " << (int)iter->y << " " << (int)iter->z << endl;
	} 
	file.close();
}

//Adds ambient component to color vector c
void calcAmbient(vec3 &c, Geometry *shape){
	c = shape->getColor() * ambient_light * shape->getAmbient();
}

//Add diffuse component to color vector c
void calcDiffuse(vec3 &c, Geometry *shape, Ray lightRay, vec3 N, double intensity){
	vec3 L = lightRay.getDirection();
	double diff = max((double)0, shape->getDiffuse() * dot(N,L));
	c += intensity * diff * shape->getColor();
}

//Add specular component to color vector c
void calcSpecular(vec3 &c, Geometry *shape, Ray eyeRay, Ray lightRay, vec3 N, Light light, double intensity){
	vec3 E = eyeRay.getDirection();
	vec3 L = lightRay.getDirection();
	
	//Get reflected ray from Normal and 
	vec3 R = normalize((2 * dot(N,L)*N) - L);
	double d = dot(E,R);
	if(d > 0){
		double spec = pow(d,shape->getCosPower()) * shape->getSpecular();
		c += intensity * spec * light.getColor();
	}
}

//Main raytracing algorithm
vec3 castRays(Ray ray, int depth){

	//Until we reach the max depth, keep casting rays	
	if(depth <= cam_depth){
		vec3 color, tmp_normal, normal, ray_dir;
		Ray shadowRay, cameraRay;
		double tmp_t, t;
		Geometry *shape;
		bool hit = false;
		
		ray_dir = ray.getDirection();
		
		//Check all objects to see if ray hits any	
		for(list<Geometry*>::iterator iter = shapes.begin(); iter!=shapes.end(); ++iter){
			if((*iter)->colision(ray, tmp_t, tmp_normal)){
				
				hit = true;
				//Simple depth test to see if object is closer than last object that was hit	
				if(tmp_t<z_depth){
					z_depth = tmp_t;
					shape = *iter;
					t = tmp_t;
					normal = tmp_normal;
				}
			}
		}
		z_depth = 9999999999;

		//Ray hit something, calculate illumination
		if(hit){

			//Add ambient light contribution			
			calcAmbient(color,shape);

			//Minus acounts for rounding errors that may have previously ocurred.
			//Makes sure the hit position isn't on the wrong side of the surface
			vec3 hitPos = cam_pos + ((t-0.0000001) * ray_dir);

			bool obstructed = false;
			//Check shadow ray to each light and see if we need to add lighting
			for(list<Light>::iterator light = lights.begin(); light!=lights.end(); ++light){

				int light_type = light->getType();
				//Check to see if shadow ray hits any other geometry
				for(list<Geometry*>::iterator iter = shapes.begin(); iter!=shapes.end(); ++iter){
				
					//Point or spot light	
					if(light_type == 1 | light_type == 3){
						shadowRay = Ray(hitPos,light->getPosition());
						if((*iter)->colision(shadowRay)){
							obstructed = true;
							break;
						}
					}
					//Parallel light
					else if(light_type == 2){
						shadowRay = Ray(hitPos,light->getPosition());
						shadowRay.setDirection(normalize(-light->getPosition()));
						if((*iter)->colision(shadowRay)){
							obstructed = true;
							break;
						}
					}
				}

				//If shadow ray isn't obstructed by any objects, get illumination
				if(!obstructed){
					cameraRay = Ray(hitPos,cam_pos);
					normal = normal;

					//Spot light
					if(light_type == 3){
						double theta = light->getAngle1();
						double phi = light->getAngle2();
						double angle = RAD_TO_DEGREE * acos(dot(-shadowRay.getDirection(), light->getDirection()));
						if(angle < theta){
							calcDiffuse(color, shape, shadowRay, normal,1.0);
							calcSpecular(color, shape, cameraRay, shadowRay, normal, *light,1.0);
						}
						else if(angle < phi){
							double att = 1;
							double mag = ((1.0/angle - cos(phi/2.0))/(cos(theta/2.0) - cos(phi/2.0)));
							mag = theta/angle;
							calcDiffuse(color, shape, shadowRay, normal, mag);
							calcSpecular(color, shape, cameraRay, shadowRay, normal, *light, mag);
						}
					}

					//Point or Parallel light
					else{
						calcDiffuse(color, shape, shadowRay, normal,1.0);
						calcSpecular(color, shape, cameraRay, shadowRay, normal, *light,1.0);
					}

				}
				obstructed = false;
			}

			//If object is reflective, cast reflected ray
			double kr = shape->getReflectance();
			if(kr > 0.0){
				vec3 R = normalize((2 * dot(normal,-ray_dir)*normal) + ray_dir);
				Ray reflected_ray = Ray(hitPos,vec3(0,0,0));
				reflected_ray.setDirection(R);
				vec3 reflect_color = castRays(reflected_ray,depth+1);
				color += (kr * reflect_color);
			}
	
			//If object transmits, calculate transmited light
			double kt = shape->getTransmitance();
			if(kt > 0.0){
				vec3 R = normalize(normal * 2 * dot(normal,ray_dir));
				Ray reflected_ray = Ray(hitPos,vec3(0,0,0));
				reflected_ray.setDirection(R);
				vec3 reflect_color = castRays(reflected_ray,depth+1);
				color += (kt * reflect_color);
			}
			return color;
		}
		//If ray hits nothing, return background color
		else{
			return (background_color);
		}	
	}
	else{
		return vec3(0,0,0);
	}	
} 

void drawImage(){

	Ray cast;
	double x_dir, y_dir, pixel_size;
	vec3 pixel_color;


	double tan_x = tan(fovx);
	double tan_y = tan(fovy);
	
	double a = ((2.0*(double)1-(double)width)/(double)width);
	double b = ((2.0*(double)2-(double)width)/(double)width);
	pixel_size = (b - a / tan_x)/4.0;
	
	//Loop over each pixel and cast rays into scene
	for(int j = height-1; j>=0; j--){
		for(int i = 0; i<width; i++){

			x_dir = ((2.0*(double)i-(double)width)/(double)width) * tan_x;
			y_dir = ((2.0*(double)j-(double)height)/(double)height) * tan_y;

			//Takes the average of 4 pixels around the current pixel
			if(anti_aliased){
				cast = Ray(cam_pos, vec3(x_dir,y_dir,0.0) + cam_lookat);
				pixel_color = castRays(cast, 1);
				cast = Ray(cam_pos, vec3(x_dir + pixel_size,y_dir,0.0) + cam_lookat);
				pixel_color += castRays(cast, 1);
				cast = Ray(cam_pos, vec3(x_dir,y_dir + pixel_size,0.0) + cam_lookat);
				pixel_color += castRays(cast, 1);
				cast = Ray(cam_pos, vec3(x_dir + pixel_size,y_dir + pixel_size,0.0) + cam_lookat);
				pixel_color += castRays(cast, 1);
			
				pixel_color = pixel_color/4.0;
			}
			else if(dof){

				double cx = cam_lookat.x;
				double cy = cam_lookat.y;
				double cz = cam_lookat.z;
				double offset = 0.015;
				cast = Ray(cam_pos, vec3(x_dir,y_dir,0.0) + cam_lookat);
				pixel_color = castRays(cast, 1);
				
				/*
				cast = Ray(vec3(cam_pos.x + offset,cam_pos.y,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				cast = Ray(vec3(cam_pos.x - offset,cam_pos.y,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				cast = Ray(vec3(cam_pos.x,cam_pos.y - offset,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				cast = Ray(vec3(cam_pos.x,cam_pos.y + offset,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				*/
				cast = Ray(vec3(cam_pos.x + offset,cam_pos.y,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				cast = Ray(vec3(cam_pos.x - offset,cam_pos.y,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				cast = Ray(vec3(cam_pos.x,cam_pos.y - offset,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
				cast = Ray(vec3(cam_pos.x,cam_pos.y + offset,cam_pos.z), vec3(x_dir,y_dir,0.0) + vec3(cx,cy,cz));
				pixel_color += castRays(cast, 1);
/*
				cast = Ray(vec3(cam_pos.x,cam_pos.y,cam_pos.z), vec3(0.0,0.0,0.0));
				cast.setDirection(vec3(x_dir,y_dir,0.0) + vec3(cx,cy + offset,cz)); 
				pixel_color += castRays(cast, 1);
*/	
				pixel_color = pixel_color/5.0;
			}	
			else{
				//Create ray and cast it into the scene.  Push returned color to pixel array
				cast = Ray(cam_pos, vec3(x_dir,y_dir,0.0) + cam_lookat);
		
				pixel_color = castRays(cast, 1);
			}
			
			pixels.push_back(vec3(pixel_color.x * COLOR_DEPTH, pixel_color.y * COLOR_DEPTH, pixel_color.z * COLOR_DEPTH));
		}
	} 
}

//Calculate width of viewing frame in world coordinates
void setUpView(){

	fovx = cam_angle/180.0 * M_PI;
	fovy = height/width * fovx;
	
}

int main(int argc, char* argv[]){
	

	//Parse scene file and store values;
	if(argc > 1){
		SceneBuilder scene = SceneBuilder(argv[1]);
		printf("Parsing scene file:%s  and setting up geometry!\n\n\n",argv[1]);
		width              = scene.getWidth();	
		height             = scene.getHeight();
		output_file        = scene.getOutputFile();
		background_color   = scene.getBackgroundColor();
		cam_pos            = scene.getCamPos();	
		cam_up             = scene.getCamUp();	
		cam_lookat         = scene.getCamLookat();	
		cam_angle          = scene.getCamAngle();
		cam_depth          = scene.getCamDepth();
		shapes             = scene.getShapes();
		lights             = scene.getLights();
		ambient_light      = scene.getAmbientLight();
	}
	//Check for extra arguments
	if(argc > 2){
		string arg = argv[2];
		if(arg.compare("anti")==0){
			printf("Adding anti-aliased effect!\n");
			anti_aliased = true;
		}
		if(arg.compare("dof")==0){
			printf("Adding depth of field effect!\n");
			dof = true;
		}
	}

	z_depth = 9999999999;

	setUpView();

	start = clock();

	printf("Tracing Rays!\n\n\n");
	drawImage();

	end = clock()-start;	
	double sec = (double)end/(double)CLOCKS_PER_SEC;
	printf("Finished!  Took %f seconds to render.\n",sec);
	printf("Output file is: %s\n\n",output_file.c_str());

	makePPM(output_file);

	return 0;
}
