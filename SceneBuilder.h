#include <math.h>
#include <stdlib.h>
#include <map>
#include <list>
#include <iostream>
#include "Sphere.h"
#include "MeshObject.h"
#include "Light.h"

using namespace std;

class SceneBuilder{

	
	string output_file;
	int width, height;
	vec3 background_color;
	vec3 cam_pos;
	vec3 cam_up;
	vec3 cam_lookat;
	double cam_angle;
	int cam_depth;
	
	double trans_x, trans_y ,trans_z ,scale_x ,scale_y ,scale_z, rotate_x, rotate_y, rotate_z;
	
	Sphere *s;
	MeshObject *mObj;
	list<Geometry*> shapes;
	
	vec3 ambient_light;
	list <Light> lights;

public:
	SceneBuilder(string path);
	void sceneBuildScene(string file);
	void sceneSetup(string setting, list<string> args);
	int getWidth();	
	int getHeight();
	string getOutputFile();
	vec3 getBackgroundColor();
	vec3 getCamPos();	
	vec3 getCamUp();	
	vec3 getCamLookat();	
	double getCamAngle();
	int getCamDepth();
	list<Geometry*> getShapes();
	list<Light> getLights();
	vec3 getAmbientLight();
};

SceneBuilder::SceneBuilder(string path){
	
	string line;
	string word;
	ifstream file;
	
	scale_x = 1.0;
	scale_y = 1.0;
	scale_z = 1.0;
	trans_x = 0.0;
	trans_y = 0.0;
	trans_z = 0.0;
	rotate_x = 0.0;
	rotate_y = 0.0;
	rotate_z = 0.0;

	map<string,list<string> > inputs;
	
	file.open(path.c_str());	

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
				
					default:
						if(first_word){
							setting.push_back(ch);
							file.get(ch);
						}
						else{
							word.push_back(ch);
							file.get(ch);
						}
						break;
				}
			}
			args.push_back(word);
			sceneSetup(setting,args);
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

void SceneBuilder::sceneSetup(string setting, list<string> args){

	list<string>::iterator iter = args.begin();
	
	if(setting.compare("output_file")==0){
		output_file = *iter;
	}
	
	else if(setting.compare("cam_point")==0){
		cam_pos.x = atof(iter->c_str());
		++iter;
		cam_pos.y = atof(iter->c_str());
		++iter;
		cam_pos.z = atof(iter->c_str());
	}
	else if(setting.compare("cam_lookat")==0){
		cam_lookat.x = atof(iter->c_str());
		++iter;
		cam_lookat.y = atof(iter->c_str());
		++iter;
		cam_lookat.z = atof(iter->c_str());
	}	
	else if(setting.compare("cam_up")==0){
		cam_up.x = atof(iter->c_str());
		++iter;
		cam_up.y = atof(iter->c_str());
		++iter;
		cam_up.z = atof(iter->c_str());
	}	
	else if(setting.compare("cam_angle")==0){
		cam_angle = atof(iter->c_str());
	}	
	else if(setting.compare("cam_res")==0){
		width = atoi(iter->c_str());
		++iter;
		height = atoi(iter->c_str());
	}
	else if(setting.compare("cam_depth")==0){
		cam_depth = atoi(iter->c_str());
	}	
	else if(setting.compare("background_color")==0){
		background_color.x = atof(iter->c_str());
		++iter;
		background_color.y = atof(iter->c_str());
		++iter;
		background_color.z = atof(iter->c_str());
	}
	else if(setting.compare("surface_sphere")==0){
		double x,y,z,r;
		x = atof(iter->c_str());
		++iter;
		y = atof(iter->c_str());
		++iter;
		z = atof(iter->c_str());
		++iter;
		r = atof(iter->c_str());
		s = new Sphere(x,y,z,r);
		shapes.push_back(s);	
	}
	else if(setting.compare("material")==0){
		double type,r,g,b,ka,kd,ks,n_s,refl,trans,iof;
		type = atoi(iter->c_str());
		++iter;
		r = atof(iter->c_str());
		++iter;
		g = atof(iter->c_str());
		++iter;
		b = atof(iter->c_str());
		++iter;
		ka = atof(iter->c_str());
		++iter;
		kd = atof(iter->c_str());
		++iter;
		ks = atof(iter->c_str());
		++iter;
		n_s = atof(iter->c_str());
		++iter;
		refl = atof(iter->c_str());
		++iter;
		trans = atof(iter->c_str());
		++iter;
		iof = atof(iter->c_str());
		++iter;
		shapes.back()->setProperties(type,r,g,b,ka,kd,ks,n_s,refl,trans,iof);
	}
	else if(setting.compare("material_texture")==0){
		double type,ka,kd,ks,n_s,refl,trans,iof;
		type = atoi(iter->c_str());
		++iter;
		string texture = *iter;
		++iter;
		ka = atof(iter->c_str());
		++iter;
		kd = atof(iter->c_str());
		++iter;
		ks = atof(iter->c_str());
		++iter;
		n_s = atof(iter->c_str());
		++iter;
		refl = atof(iter->c_str());
		++iter;
		trans = atof(iter->c_str());
		++iter;
		iof = atof(iter->c_str());
		++iter;
		shapes.back()->setTexture(texture);
		shapes.back()->setProperties(type,0.0,0.0,0.0,ka,kd,ks,n_s,refl,trans,iof);
	}
	else if(setting.compare("light_ambient")==0){
		double r,g,b;
		r = atof(iter->c_str());
		++iter;
		g = atof(iter->c_str());
		++iter;
		b = atof(iter->c_str());
		++iter;
		ambient_light = vec3(r,g,b);
	}
	else if(setting.compare("light_point")==0){
		double r,g,b,x,y,z;
		r = atof(iter->c_str());
		++iter;
		g = atof(iter->c_str());
		++iter;
		b = atof(iter->c_str());
		++iter;
		x = atof(iter->c_str());
		++iter;
		y = atof(iter->c_str());
		++iter;
		z = atof(iter->c_str());
		++iter;
		lights.push_back(Light(1,r,g,b,x,y,z));
	}
	else if(setting.compare("light_parallel")==0){
		double r,g,b,x,y,z;
		r = atof(iter->c_str());
		++iter;
		g = atof(iter->c_str());
		++iter;
		b = atof(iter->c_str());
		++iter;
		x = atof(iter->c_str());
		++iter;
		y = atof(iter->c_str());
		++iter;
		z = atof(iter->c_str());
		++iter;
		lights.push_back(Light(2,r,g,b,x,y,z));
	}
	else if(setting.compare("light_spot")==0){
		double r,g,b,x,y,z,px,py,pz,a1,a2;
		r = atof(iter->c_str());
		++iter;
		g = atof(iter->c_str());
		++iter;
		b = atof(iter->c_str());
		++iter;
		x = atof(iter->c_str());
		++iter;
		y = atof(iter->c_str());
		++iter;
		z = atof(iter->c_str());
		++iter;
		px = atof(iter->c_str());
		++iter;
		py = atof(iter->c_str());
		++iter;
		pz = atof(iter->c_str());
		++iter;
		a1 = atof(iter->c_str());
		++iter;
		a2 = atof(iter->c_str());
		lights.push_back(Light(3,r,g,b,x,y,z,px,py,pz,a1,a2));
	}
	else if(setting.compare("surface_mesh")==0){
		mObj = new MeshObject(*iter);
		shapes.push_back(mObj);
	}
	else if(setting.compare("scale")==0){
		scale_x = atof(iter->c_str());
		++iter;
		scale_y = atof(iter->c_str());
		++iter;
		scale_z = atof(iter->c_str());
	}
	else if(setting.compare("translate")==0){
		trans_x = atof(iter->c_str());
		++iter;
		trans_y = atof(iter->c_str());
		++iter;
		trans_z = atof(iter->c_str());
	}
	else if(setting.compare("rotateX")==0){
		rotate_x = atof(iter->c_str());
	}
	else if(setting.compare("rotateY")==0){
		rotate_y = atof(iter->c_str());
	}
	else if(setting.compare("rotateZ")==0){
		rotate_z = atof(iter->c_str());
	}
	else if(setting.compare("end_surface")==0){
		
		shapes.back()->setTransform(scale_x,scale_y,scale_z,trans_x,trans_y,trans_z,rotate_x,rotate_y,rotate_z);
		scale_x = 1.0;
		scale_y = 1.0;
		scale_z = 1.0;
		trans_x = 0.0;
		trans_y = 0.0;
		trans_z = 0.0;
		rotate_x = 0.0;
		rotate_y = 0.0;
		rotate_z = 0.0;
	}	
}


int SceneBuilder::getWidth(){
	return width;
}	
int SceneBuilder::getHeight(){
	return height;
}
string SceneBuilder::getOutputFile(){
	return output_file;
}
vec3 SceneBuilder::getBackgroundColor(){
	return background_color;
}
vec3 SceneBuilder::getCamPos(){
	return cam_pos;
}	
vec3 SceneBuilder::getCamUp(){
	return cam_up;
}
vec3 SceneBuilder::getCamLookat(){
	return cam_lookat;
}	
double SceneBuilder::getCamAngle(){
	return cam_angle;
}
int SceneBuilder::getCamDepth(){
	return cam_depth;
}
list<Geometry*> SceneBuilder::getShapes(){
	return shapes;
}
list<Light> SceneBuilder::getLights(){
	return lights;
}
vec3 SceneBuilder::getAmbientLight(){
	return ambient_light;
}
