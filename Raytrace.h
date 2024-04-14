#include <vector>
// #include "Camera.h" 
// #include "Scene.h"  
// #include "Image.h"
#include "Geometry.h"
using namespace std;

struct Ray {
	vec3 origin;
	vec3 direction;
	Ray(vec3 o, vec3 d) : origin(o), direction(d) {}
};

struct Intersection {
	Ray ray;
	vec3 intersection;
	Object *obj;//the geometry that ray intersect with(triangle or sphere)
	float t;
	vec3 normal;
	Intersection(Ray r, vec3 i, Object *obj, float point, vec3 n): ray(r), intersection(i), obj(obj), t(point),normal(n){}
};

class Raytrace
{
public:
	static float tri_intersect(Ray ray, Object *tri);
	static float sphere_intersect(Ray ray, Object *sphere);
	static vector<vector<vec3> > raytrace(Camera *cam, Scene *scene, int width, int height);
	static Ray rayThruPixel(Camera *cam, int i, int j);
	static Intersection intersect(Ray ray);
	static bool isVisible(Ray ray,vec3 lightpos);
	static vec3 ComputePointLight(const vec3 direction, const vec3 lightcolor, const vec3 normal, const vec3 halfvec, const float distance, vec3 diffuse, vec3 specular,float shininess);
	static vec3 ComputeDLight (const vec3 direction, const vec3 lightcolor, const vec3 normal, const vec3 halfvec,vec3 diffuse, vec3 specular,float shininess);
	static Ray reflectedRay(Ray ray, vec3 intersection, vec3 normal);
	static vec3 findColor(Intersection hit, Scene *scene,int depth);
	//static bool isBBoxIntersect(Ray ray, Object* obj);
	//static Intersection accintersect(Ray ray);
	//static vector<vector<vec3> > accraytrace(Camera* cam, Scene* scene, int width, int height);
};

