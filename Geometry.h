#include "Ray.h"
#include "mat.h"
#include <string>
#include <fstream>

using namespace std;

class Geometry{
	public:
		virtual bool colision(Ray r, double &t, vec3 &normal);
		virtual bool colision(Ray r) const=0;
		virtual void setTexture(string filename);
		virtual vec3 getColor() const=0;
		virtual double getAmbient() const=0;
		virtual double getDiffuse() const=0;
		virtual double getSpecular() const=0;
		virtual double getCosPower() const=0;
		virtual double getReflectance()const=0;
		virtual double getTransmitance()const=0;
		virtual void   printSize() const=0;
		virtual void   printType() const=0;
		virtual void setTransform(double sx, double sy, double sz, double tx, double ty, double tz, double rx, double ry, double rz);
		virtual void setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index);
		virtual Ray  transform(Ray ray)const;
};

bool Geometry::colision(Ray r, double &t, vec3 &normal){
	return false;
}
void Geometry::setTexture(string filename){

}
vec3 Geometry::getColor() const{
	return vec3(0,0,0);
}
double Geometry::getAmbient() const{
	return -1.0;
}
double Geometry::getDiffuse() const{
	return -1.0;
}
double Geometry::getSpecular() const{
	return -1.0;
}
double Geometry::getCosPower() const{
	return -1;
}
double Geometry::getReflectance()const{
	return -1;
}
double Geometry::getTransmitance()const{
	return -1;
}
void Geometry::setTransform(double sx, double sy, double sz, double tx, double ty, double tz, double rx, double ry, double rz){

}
void Geometry::setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index){

}
Ray Geometry::transform(Ray ray)const{
	return ray;
}
