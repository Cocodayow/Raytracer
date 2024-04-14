#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;
#include "Raytrace.h"
#include "variables.h" 

Ray Raytrace::rayThruPixel(Camera *cam, int i, int j){
    //Construct coordinate frame
    vec3 eye = cam->eye;
    vec3 center = cam->center;
    vec3 w_ = glm::normalize(eye - center);
    vec3 u_ = glm::normalize(glm::cross(cam->up, w_));
    vec3 v_ = glm::cross(w_,u_);

    float fovy= cam->fovy;
    float aspect = float(w)/float(h);
    float halfw = float(w)/2.0f;
    float halfh = float(h)/2.0f;
    float a = aspect* tan(fovy*0.5f*pi/180) * ((float(j)+0.5f)-halfw)/halfw;
    float b = tan(fovy*0.5f*pi/180) * (halfh - (float(i)+0.5f))/halfh;
    vec3 direction = glm::normalize(a*u_ + b*v_ - w_);
    return Ray(eye,direction);
}

//return t
float Raytrace::sphere_intersect(Ray ray, Object *sphere){
    vec3 p0 = ray.origin;
    vec3 p1 = ray.direction;
    vec3 center = sphere->getCenter();
    mat4 transf = sphere->getTransformation();
    //trasnform the ray
    vec4 p0_transformed = glm::inverse(transf) * vec4(p0, 1.0f);
    vec4 p1_transformed = glm::inverse(transf) * vec4(p1, 0.0f);
    vec3 p0after = vec3(p0_transformed.x, p0_transformed.y, p0_transformed.z);
    vec3 p1after = vec3(p1_transformed.x, p1_transformed.y, p1_transformed.z);
    //assign a,b and c values
    float a = glm::dot(p1after,p1after);
    float b = 2*glm::dot(p1after,p0after - center);
    float c = glm::dot(p0after - center,p0after - center) - (float(sphere->getRadius())* float(sphere->getRadius()));
    //check determinent
    float det = b*b - 4.0f*a*c;
    if (det < 0) {
        return -1.0f; //means there is no positive root
    }
    else if (det == 0.0f){

        return float(-b/(2.0f * a)); //just one root
    }
    else {//take the positive smaller one
        float t1 = (-b - sqrt(det))/(2.0f * a);
        float t2 = (-b + sqrt(det))/(2.0f * a);
        if (t1 <= 0){
            if (t2 > 0) {return t2;}
            else {return -1.0f;}
        }
        else {return t1;}
    }
}
//return t
float Raytrace::tri_intersect(Ray ray, Object *tri){
    //Implement Möller-Trumbore algorithm, which takes the intersection point in barycentric coordinates
    //Online Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html.

    vec3 A = tri->getv1();
    vec3 B = tri->getv2();
    vec3 C = tri->getv3();
    
    vec3 p0 = ray.origin;
    vec3 p1 = ray.direction;
    
    // (B-A) and (C-A) are the edges AB and AC of the triangle ABC
    vec3 AB = B - A;
    vec3 AC = C - A;
    //scalar triple product determinant, a combination of a cross and a dot product
    vec3 pvec = glm::cross(p1,AC);
    float det = glm::dot(AB,pvec);

    // if the determinant is negative, the triangle is 'back facing'
    if (det < 0.00001f) return -1.0f;

    // ray and triangle are parallel if det is close to 0
    if (det < 0.00001f && det > -0.00001f ){
        return -1.0f;
    } 

    //p0-A = -t*p1 + u*(B-A)+ v*(C-A)
    //t,u, and v are unknown scalars
    vec3 tvec = p0 - A;
    //calculate u and v
    float u = glm::dot(tvec,pvec) / det;
    vec3 qvec = glm::cross(tvec,AB);
    float v = glm::dot(p1,qvec) / det;
    float t = glm::dot(AC,qvec) /det;
    //check the bound of u and v
    if ( u > -0.0000001f && v > -0.0000001f && (u + v) < 1.00001f) {
        return t;
    }
    
    return -1.0f;
}

Intersection Raytrace::intersect(Ray ray){
    float min_t = INFINITY;
    Object* min_obj = NULL;
    vec3 interPoint = vec3(0.0f);
    vec3 normal = vec3(0.0f);
    for(Object* obj : scene->spheres) {
        float t1 = sphere_intersect(ray, obj);
        if (t1 > 0 && t1 < min_t) {
            min_obj = obj;
            min_t = t1;
            mat4 transf = obj->getTransformation();
        
            vec4 transformedP = glm::inverse(transf)*vec4(ray.origin+ray.direction*min_t,1.0f);
            interPoint = vec3(transformedP.x,transformedP.y,transformedP.z);
            vec3 originalnormal = glm::normalize(interPoint - obj->getCenter());
            
            vec4 n = glm::transpose(glm::inverse(transf))*vec4(originalnormal,1.0f);
            normal = glm::normalize(vec3(n.x,n.y,n.z));
            interPoint = vec3(transf * transformedP);
        }
    }
    for(Object* obj2 : scene->triangles) {
        float t2 = tri_intersect(ray, obj2);
        if (t2 > 0 && t2 < min_t) {
            min_obj = obj2;
            min_t = t2;
            normal = -1.0f * glm::normalize(glm::cross(obj2->getv3()-obj2->getv1(),obj2->getv2()-obj2->getv1()));
            interPoint = ray.origin+ray.direction*min_t;
        }
    }
    return Intersection(ray, interPoint, min_obj, min_t, normal);
}

bool Raytrace::isVisible(Ray ray,vec3 lightpos){
    
    float dis = glm::distance(ray.origin,lightpos);
    for(Object* obj1 : scene->spheres) {
        float t1 = sphere_intersect(ray, obj1);
        if (t1 > 0 && t1 <= dis) {
            return false;
        }
    }
    for(Object* obj2 : scene->triangles) {
        float t1 = tri_intersect(ray, obj2);
        if (t1 > 0 && t1 <= dis ) {
            return false;

        }
    }
    return true;
}

//Compute Point light
vec3 Raytrace::ComputePointLight (const vec3 direction, const vec3 lightcolor, const vec3 normal, const vec3 halfvec, 
const float distance,vec3 diffuse, vec3 specular,float shininess) {
    float attenuation = 1.0f / (float(c0) + float(c1) * distance + float(c2) * distance * distance);
    float nDotL = glm::dot(normal, direction);
    vec3 lambert = diffuse * lightcolor * attenuation * max(nDotL, 0.0f);

    float nDotH = glm::dot(normal, halfvec) ;
    vec3 phong = specular * lightcolor * attenuation * pow(max(nDotH, 0.0f), shininess) ;
    vec3 retval = lambert + phong;
    return retval ;
}

//Compute Directional light
vec3 Raytrace::ComputeDLight (const vec3 direction, const vec3 lightcolor, const vec3 normal, const vec3 halfvec
,vec3 diffuse, vec3 specular,float shininess) {
    float nDotL = glm::dot(normal, direction);
    vec3 lambert = diffuse * lightcolor * max(nDotL, 0.0f);

    float nDotH = glm::dot(normal, halfvec) ;
    vec3 phong = specular * lightcolor * pow(max(nDotH, 0.0f), shininess) ;

    vec3 retval = lambert + phong ;
    return retval ;
}
//Generate reflected ray
Ray Raytrace::reflectedRay(Ray ray, vec3 intersection, vec3 normal){
    vec3 direction = glm::reflect(ray.direction, normal);
    vec3 origin = intersection + 0.001f * normal;
    return Ray(origin,direction);
}
//Find color of a specific pixel
vec3 Raytrace:: findColor(Intersection hit, Scene *scene, int depth){
    vec3 finalColor = vec3(0.0f,0.0f,0.0f);
    if (depth <= 0) {
        return finalColor;
    }
    if (hit.obj == NULL) {
        return finalColor;
    }
    Ray ray = hit.ray;
    vec3 intersect = hit.intersection;
    vec3 normal = hit.normal;
    vec3 diff = hit.obj->getDiffuse();
    vec3 specu = hit.obj->getSpecular();
    float shi = hit.obj->getShininess();
    //directional light
    for (int i = 0; i < dirnumused; i++){
        vec3 direction = glm::normalize(vec3(directionallp[3*i],directionallp[3*i+1],directionallp[3*i+2]));
        vec3 shadow_ray_origin = intersect + 0.001f * normal;
        vec3 shadow_ray_direction = glm::normalize(direction);
        Ray *shadow_ray = new Ray(shadow_ray_origin, shadow_ray_direction);
        vec3 lightpos = vec3(INFINITY);
        bool visible = isVisible(*shadow_ray,lightpos);
        //check visibility
        if (visible){
            std::cout << 5 << std::endl;
            vec3 lightColor = vec3(directionallc[3*i],directionallc[3*i+1],directionallc[3*i+2]);
            vec3 halfVec = glm::normalize(direction -ray.direction);
            finalColor += ComputeDLight(direction,lightColor,normal,halfVec,diff,specu,shi);
        }
        
    }
    //Point light
    
    for (int i = 0; i < poirnumused; i++){
        vec3 lightpos = vec3(pointlp[3*i],pointlp[3*i+1],pointlp[3*i+2]);
        vec3 shadow_ray_origin = intersect + 0.001f * normal;
        vec3 shadow_ray_direction = glm::normalize(lightpos - shadow_ray_origin);
        Ray *shadow_ray = new Ray(shadow_ray_origin, shadow_ray_direction);
        bool visible = isVisible(*shadow_ray,lightpos);
        
        //check visibility
        if (visible){
            std::cout << 6 << std::endl;
            vec3 lightColor = vec3(pointlc[3*i],pointlc[3*i+1],pointlc[3*i+2]);
            float distance = glm::distance(intersect,lightpos);
            vec3 direction = glm::normalize(lightpos - intersect);
            vec3 halfVec = glm::normalize(direction -ray.direction) ;
            finalColor += ComputePointLight(direction,lightColor,normal,halfVec,distance,diff,specu,shi);
        }
    }
    //Reflection
    if(specu != vec3(0,0,0)){
        Ray reflectedr = reflectedRay(ray,intersect,normal);
        Intersection reflectedI = Raytrace::intersect(reflectedr);
        vec3 reflectedColor = findColor(reflectedI,scene, depth-1);
        finalColor += reflectedColor * specu;
    }
    finalColor += (hit.obj->getAmbient()+ hit.obj->getEmission());
    return finalColor;
}

vector<vector<vec3> > Raytrace::raytrace(Camera *cam, Scene *scene, int width, int height) {
    vector<vector<vec3> > image(height, vector<vec3>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Ray ray = rayThruPixel(cam, i, j);
            Intersection hit = Raytrace::intersect(ray);
            vec3 color = findColor(hit, scene,maxdepth);
            image[i][j] = color; 
        }
    }

    return image;
}
//Acceleration
/*
//the ray enters the box iff it enters all pairs of slabs
//the ray exits the box as long as it exits anu pair of slabs
//for each pair(opposite sides), calculate tmin and tmax;
//for all 3 pairs of tmins, tmax, return values accordingly
bool Raytrace::isBBoxIntersect(Ray ray, Object* obj) {
    //https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
    //https://cg.cs.tsinghua.edu.cn/course/docs/chap4.pdf
    obj->calculateBoundingBox();
    Boundingbox bbox = obj->getBoundingbox();
    float xmin = bbox->xmin;
    float xmax = bbox->xmax;
    float ymin = bbox->ymin;
    float ymax = bbox->ymax;
    float zmin = bbox->zmin;
    float zmax = bbox->zmax;
    vec3 p0 = ray.origin;
    vec3 p1 = ray.direction;
    
    // Opposite planes along the y-axis
    float opp1 = ymin;
    float opp2 = ymax;
    vec3 edge1 = vec3(xmin, opp1, zmin) - vec3(xmax, opp1, zmin);
    vec3 edge2 = vec3(xmin, opp1, zmax) - vec3(xmax, opp1, zmin);
    vec3 normal = glm::cross(edge1, edge2);
    vec3 diffp0p = vec3(xmin, opp1, zmin) - p0;
    if (glm::dot(p1, normal) != 0) {
        tp1 = -(glm::dot(diffp0p, normal) / glm::dot(p1, normal);
    };
    vec3 edge1 = vec3(xmin, opp2, zmin) - vec3(xmax, opp2, zmin);
    vec3 edge2 = vec3(xmin, opp2, zmax) - vec3(xmax, opp2, zmin);
    vec3 normal = glm::cross(edge1, edge2);
    vec3 diffp0p = vec3(xmin, opp2, zmin) - p0;
    if (glm::dot(p1, normal) != 0) {
        tp2 = -(glm::dot(diffp0p, normal) / glm::dot(p1, normal);
    };
    float tmaxy = tp1 > tp2 ? tp1 : tp2;
    float tminy = tp1 < tp2 ? tp1 : tp2;
    //x opposite planes
    float opp1 = xmin;
    float opp2 = xmax;
    vec3 edge1 = vec3(opp1, ymin, zmin) - vec3(opp1, ymax, zmin);
    vec3 edge2 = vec3(opp1, ymin, zmax) - vec3(opp1, ymax, zmin);
    vec3 normal = glm::cross(edge1, edge2);
    vec3 diffp0p = vec3(opp1, ymin, zmin) - p0;
    if (glm::dot(p1, normal) != 0) {
        tp1 = -(glm::dot(diffp0p, normal) / glm::dot(p1, normal);
    };
    vec3 edge1 = vec3(opp2, ymin, zmin) - vec3(opp2, ymax, zmin);
    vec3 edge2 = vec3(opp2, ymin, zmax) - vec3(opp2, ymax, zmin);
    vec3 normal = glm::cross(edge1, edge2);
    vec3 diffp0p = vec3(opp2, ymin, zmin) - p0;
    if (glm::dot(p1, normal) != 0) {
        tp2 = -(glm::dot(diffp0p, normal) / glm::dot(p1, normal);
    };
    float tmaxx = tp1 > tp2 ? tp1 : tp2;
    float tminx = tp1 < tp2 ? tp1 : tp2;
    //z opposite planes
    float opp1 = zmin;
    float opp2 = zmax;
    vec3 edge1 = vec3(xmin, ymin, opp1) - vec3(xmin, ymax, opp1);
    vec3 edge2 = vec3(xmax, ymin, opp1) - vec3(xmin, ymax, opp1);
    vec3 normal = glm::cross(edge1, edge2);
    vec3 diffp0p = vec3(xmin, ymin, opp1) - p0;
    if (glm::dot(p1, normal) != 0) {
        tp1 = -(glm::dot(diffp0p, normal) / glm::dot(p1, normal);
    };
    vec3 edge1 = vec3(xmin, ymin, opp2) - vec3(xmin, ymax, opp2);
    vec3 edge2 = vec3(xmax, ymin, opp2) - vec3(xmin, ymax, opp2);
    vec3 normal = glm::cross(edge1, edge2);
    vec3 diffp0p = vec3(xmin, ymin, opp2) - p0;
    if (glm::dot(p1, normal) != 0) {
        tp2 = -(glm::dot(diffp0p, normal) / glm::dot(p1, normal);
    };
    float tmaxz = tp1 > tp2 ? tp1 : tp2;
    float tminz = tp1 < tp2 ? tp1 : tp2;
    float texit = glm::max(tmaxx, tmaxy, tmaxz);
    float tenter = glm::min(tminx, tminy, tminz);
    if ((tenter < texit) && (texit >= 0)) {
        return true;
    };
    else {
        return false;
    };
    
};*/

/*//accelerated raytracing
Intersection Raytrace::accintersect(Ray ray) {
    float min_t = INFINITY;
    Object* min_obj = NULL;
    vec3 interPoint;
    vec3 normal;
    for (Object* obj : scene->spheres) {
        if (isBBoxIntersect(ray, obj)) {
            float t1 = sphere_intersect(ray, obj);
            if (t1 > 0 && t1 < min_t) {
                min_obj = obj;
                min_t = t1;
                mat4 transf = obj->getTransformation();

                vec4 transformedP = glm::inverse(transf) * vec4(ray.origin + ray.direction * min_t, 1.0f);
                interPoint = vec3(transformedP.x, transformedP.y, transformedP.z);
                vec3 originalnormal = glm::normalize(interPoint - obj->getCenter());

                vec4 n = glm::transpose(glm::inverse(transf)) * vec4(originalnormal, 1.0f);
                normal = glm::normalize(vec3(n.x, n.y, n.z));
                interPoint = vec3(transf * transformedP);
            }
        }
    }
    for (Object* obj2 : scene->triangles) {
        if (isBBoxIntersect(ray, obj)) {
            float t2 = tri_intersect(ray, obj2);
            if (t2 > 0 && t2 < min_t) {
                min_obj = obj2;
                min_t = t2;
                normal = -1.0f * glm::normalize(glm::cross(obj2->getv3() - obj2->getv1(), obj2->getv2() - obj2->getv1()));
                interPoint = ray.origin + ray.direction * min_t;
            }
        }
    }
    return Intersection(ray, interPoint, min_obj, min_t, normal);
}
vector<vector<vec3> > Raytrace::accraytrace(Camera* cam, Scene* scene, int width, int height) {
    vector<vector<vec3> > image(height, vector<vec3>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Ray ray = rayThruPixel(cam, i, j);
            Intersection hit = accintersect(ray);
            vec3 color = findColor(hit, scene, maxdepth);
            image[i][j] = color;
            if (i == 100 && j == 320) {
                std::cout << color[0] << color[1] << color[2] << std::endl;
            }
        }
    }

    return image;
}
*/



