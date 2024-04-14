#include <vector>
#include "Transform.h"

typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ; 
typedef glm::vec3 vec3 ; 
typedef glm::vec4 vec4 ; 


struct Camera{
    vec3 eye;
    vec3 up;
    vec3 center;
    float fovy;
    Camera(const vec3 &ey, vec3 u, vec3 c, float f):  eye(ey), up(u), center(c), fovy(f) {};
};
/*
accelerated ray tracing
struct Boundingbox {
    vec3 xmin;
    vec3 xmax;
    vec3 ymin;
    vec3 ymax;
    vec3 zmin;
    vec3 zmax;

    //Boundingbox(vec3& mi = vec3(FLT_MAX), vec3& ma = vec3(-FLT_MAX)) : min(mi), max(ma) {};
    Boundingbox(vec3 xmi, vec3 xma, vec3 ymi, vec3 yma, vec3 zmi, vec3 zma) : xmin(xmi), xmax(xma), ymin(ymi), ymax(yma), zmin(zmi), zmax(zma) {};
    //void extend(const vec3& point) {
    // min = glm::min(min, point);
    // max = glm::max(max, point);
    //}
};
*/
// Define a base class for objects in the scene
class Object {
public:
    virtual vec3 getCenter() {};
    virtual float getRadius() {};
    virtual mat4 getTransformation() {}
    virtual vec3 getAmbient(){};// to get ambient color
    virtual vec3 getEmission(){};
    virtual vec3 getDiffuse(){};
    virtual vec3 getSpecular(){};
    virtual float getShininess(){};
    virtual vec3 getv1() {};
    virtual vec3 getv2() {};
    virtual vec3 getv3() {};
    //virtual void calculateBoundingBox() {};
    //virtual Boundingbox getBoundingbox() {};
    
};

// Define the Sphere class derived from Object
class Sphere : public Object {
private:
    glm::vec3 c; // Center
    float radius;
    mat4 transformation; // Scaling factors for x, y, and z axes
    glm::vec3 ambient;
    vec3 emission;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    //Boundingbox bbox;

public:

    Sphere(vec3 _c, float _radius, mat4 _transformation, vec3 _ambient, vec3 _emission, vec3 diff, vec3 spec, float shin)
        : c(_c), radius(_radius), transformation(_transformation), ambient(_ambient), emission(_emission), diffuse(diff), specular(spec), shininess(shin) {};

    vec3 getCenter() override {
        return c;
    }
    float getRadius() override {
        return radius;
    }
    mat4 getTransformation() override {
        return transformation;
    }
    vec3 getAmbient() override {
        return ambient;
    };
    vec3 getEmission() override {
        return emission;
    };
    vec3 getDiffuse() override {
        return diffuse;
    };
    vec3 getSpecular() override {
        return specular;
    };
    float getShininess() override {
        return shininess;
    };
    /*
    accelerated ray tracinng
    void calculateBoundingBox() override {
       // vec3 rvec(radius);
        //bbox.extend(c - rvac);
        //bbox.extend(c + rvac);
        bbox.xmax = (radius + c[0]) > c[0] - radius ? radius + c[0]: c[0] - radius;
        bbox.xmin = (radius + c[0]) < c[0] - radius ? c[0] - radius : radius + c[0];
        bbox.ymax = (radius + c[1]) > c[1] - radius ? radius + c[1] : c[1] - radius;
        bbox.ymin = (radius + c[1]) < c[1] - radius ? c[1] - radius : radius + c[1];
        bbox.zmax = (radius + c[2]) > c[2] - radius ? radius + c[2] : c[2] - radius;
        bbox.zmin = (radius + c[2]) < c[2] - radius ? c[2] - radius : radius + c[2];

    };
    Boundingbox getBoundingbox() override {
        return bbox;
    };
    */
};


// Define the Triangle class derived from Object
class Triangle :public Object {
private:
    vec3 v1, v2, v3;
    vec3 ambient;
    vec3 emission;
    vec3 diffuse;
    vec3 specular;
    float shininess;
public:
    Triangle(vec3 _v1, vec3 _v2, vec3 _v3, vec3 _ambient, vec3 _emission, vec3 diff, vec3 spec, float shin) :
        v1(_v1), v2(_v2), v3(_v3), ambient(_ambient), emission(_emission), diffuse(diff), specular(spec), shininess(shin) {};

    vec3 getv1() override {
        return v1;
    }
    vec3 getv2() override {
        return v2;
    }
    vec3 getv3() override {
        return v3;
    }
    vec3 getAmbient() override {
        return ambient;
    };
    vec3 getEmission() override {
        return emission;
    };
    vec3 getDiffuse() override {
        return diffuse;
    };
    vec3 getSpecular() override {
        return specular;
    };
    float getShininess() override {
        return shininess;
    };
    /*
    * accelerated ray tracing, calculates bounding box with triangles
    void calculateBoundingBox() override {
        //https://stackoverflow.com/questions/39974191/triangle-bounding-box
        bbox.xmax = v1[0] > v2[0] ? (v1[0] > v3[0] ? v1[0] : v3[0]) : (v2[0] > v3[0] ? v2[0] : v3[0]);
        bbox.ymax = v1[1] > v2[1] ? (v1[1] > v3[1] ? v1[1] : v3[1]) : (v2[1] > v3[1] ? v2[1] : v3[1]);
        bbox.zmax = v1[2] > v2[2] ? (v1[2] > v3[2] ? v1[2] : v3[2]) : (v2[2] > v3[2] ? v2[2] : v3[2]);
        bbox.xmin = v1[0] < v2[0] ? (v1[0] < v3[0] ? v1[0] : v3[0]) : (v2[0] < v3[0] ? v2[0] : v3[0]);
        bbox.ymin = v1[1] < v2[1] ? (v1[1] < v3[1] ? v1[1] : v3[1]) : (v2[1] < v3[1] ? v2[1] : v3[1]);
        bbox.zmin = v1[2] < v2[2] ? (v1[2] < v3[2] ? v1[2] : v3[2]) : (v2[2] < v3[2] ? v2[2] : v3[2]);

    }
    Boundingbox getBoundingbox() override {
        return bbox;
    };
    */
};

struct Scene{
    std::vector<Object*> spheres;
    std::vector<Object*> triangles;
    
    Scene(std::vector<Object*> s, std::vector<Object*> t) : spheres(s), triangles(t) {};
};
