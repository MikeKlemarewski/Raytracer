class Light{

	int type;
	vec3 color;
	vec3 position;	
	vec3 direction;
	double angle1, angle2;

	public:
		Light(int t, double r, double g, double b, double x, double y, double z);
		Light(int t, double r, double g, double b, double x, double y, double z, double px, double py, double pz, double a1, double a2);
		int  getType();
		vec3 getColor();
		vec3 getPosition();
		vec3 getDirection();
		double getAngle1();
		double getAngle2();
};


Light::Light(int t, double r, double g, double b, double x, double y, double z){

	type = t;
	color = vec3(r,g,b);
	position = vec3(x,y,z);
	direction = vec3(0.0,0.0,0.0);
	angle1 = 0.0;
	angle2 = 0.0;
}

Light::Light(int t, double r, double g, double b, double x, double y, double z, double px, double py, double pz, double a1, double a2){
	type = t;
	color = vec3(r,g,b);
	position = vec3(x,y,z);
	direction = normalize(vec3(px,py,pz));
	angle1 = a1;
	angle2 = a2;

}

int Light::getType(){
	return type;
}

vec3 Light::getColor(){
	return color;
}

vec3 Light::getPosition(){
	return position;
}

vec3 Light::getDirection(){
	return direction;
}
double Light::getAngle1(){
	return angle1;
}
double Light::getAngle2(){
	return angle2;
}
