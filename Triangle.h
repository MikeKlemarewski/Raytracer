class Triangle{

	vec3   v0,v1,v2;
	vec3   normal, color;
	vec2   t0,t1,t2;
	int    type;
	double ambient, diffuse, specular, cos_power, reflect, transmit, iof;
	bool   textured;
	
	public:
		Triangle();
		Triangle(vec3 a, vec3 b, vec3 c);
		Triangle(vec3 a, vec3 b, vec3 c, vec2 u, vec2 v, vec2 w);
		
		void   setNormal(vec3 n);
		vec3   getColor()const;
		vec3   getNormal()const;
		vec3   getPosition()const;
		double getAmbient()const;
		double getDiffuse()const;
		double getSpecular()const;
		int    getCosPower()const;
		void   printType()const;
		void   printPosition()const;
		double colision(Ray ray,vec2 &p)const;
		void   setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index); 

};

Triangle::Triangle(){

}

Triangle::Triangle(vec3 a, vec3 b, vec3 c){
	v0 = a;
	v1 = b;
	v2 = c;
	normal = normalize(cross((v1-v0),(v2-v0)));
	textured = false;
}

Triangle::Triangle(vec3 a, vec3 b, vec3 c, vec2 u, vec2 v, vec2 w){
	v0 = a;
	v1 = b;
	v2 = c;
	normal = normalize(cross((v1-v0),(v2-v0)));
	t0 = u;
	t1 = v;
	t2 = w;
	textured = true;
}

void Triangle::setNormal(vec3 n){
	normal = n;
}

void Triangle::setProperties(int t, double r, double g, double b, double ka, double kd, double ks, double n_s, double refl, double trans, double index){

	type      = t;
	color     = vec3(r,g,b);
	ambient   = ka;
	diffuse   = kd;
	specular  = ks;
	cos_power = n_s;
	reflect   = refl;
	transmit  = trans;
	iof       = index;
}

vec3 Triangle::getColor()const{
	return color;
}
vec3 Triangle::getNormal()const{
	return normal;
}
vec3 Triangle::getPosition()const{
	return v0;
}
double Triangle::getAmbient()const{
	return ambient;
}
double Triangle::getDiffuse()const{
	return diffuse;
}
double Triangle::getSpecular()const{
	return specular;
}
int Triangle::getCosPower()const{
	return cos_power;
}
void Triangle::printType()const{
	printf("Triangle\n");
}

double Triangle::colision(Ray ray, vec2 &p)const{

	vec3 dir = ray.getDirection();	
	vec3 r0  = ray.getOrigin();

	double a,b;
	b = dot(normal,dir);
		
	//Ray is parallel to plane
	if(fabs(b) < 0.0000001){
		return -1.0;
	}
	//Ray hits, find intersection point
	else{
		vec3 w0, w;
	
		w0 = r0 - v0;
		a = -dot(normal,w0);
		
		// get intersect point of ray with triangle plane
		double distance = a / b;
		if (distance < 0.0){
		    return -1;
		}
		
		vec3 v = v1 - v0;
		vec3 u = v2 - v0;	
		
		//Point where ray hits plane
		vec3 hitPoint = r0 + distance * dir;
		w = hitPoint - v0;
		
		double s, s_numerator, t, t_numerator, denominator;
	
		denominator = pow(dot(u,v),2) - dot(u,u) * dot(v,v);
			
		s_numerator = dot(u,v)*dot(w,v) - dot(v,v)*dot(w,u);
		s = s_numerator/denominator;
		if (s > 1.0 || s < 0.0){
		    return -1;
		}
		t_numerator = dot(u,v)*dot(w,u) - dot(u,u)*dot(w,v);
		t = t_numerator/denominator;
		if (t < 0.0 || (t+s) > 1.0){
		    return -1;
		}
		//point used for texture mapping
		p = vec2(s,t);	
		return distance;
	}
}
