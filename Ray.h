#include "vec.h"
#include <stdio.h>

class Ray{

vec3 origin;
vec3 direction;

public:
	Ray();
	Ray(vec3 a, vec3 b);
	vec3 getDirection();
	vec3 getOrigin();
	void setOrigin(vec3 o);
	void setDirection(vec3 dir);
};

Ray::Ray(){

}

Ray::Ray(vec3 a, vec3 b){
	origin    = a;
	direction = normalize(b - a);
}

vec3 Ray::getOrigin(){
	return origin;
}
vec3 Ray::getDirection(){
	return direction;
}
void Ray::setOrigin(vec3 o){
	origin = o;
}
void Ray::setDirection(vec3 dir){
	direction = dir;
}
