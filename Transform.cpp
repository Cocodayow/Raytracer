// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
  float x = axis[0];
  float y = axis[1];
  float z = axis[2];
  float c = cos(degrees * pi/180.0);
  float co = 1-cos(degrees * pi/180.0);
  float s = sin(degrees * pi/180.0);
  mat3 first = mat3(1.0*c,0.0,0.0,0.0,1.0*c,0.0,0.0,0.0,1.0*c);
  mat3 second = mat3(x*x*co, x*y*co, x*z*co, x*y*co, y*y*co, y*z*co, x*z*co, y*z*co, z*z*co);
  mat3 third = mat3(0.0, z*s, -y*s, -z*s, 0.0, x*s, y*s, -x*s, 0.0);
  mat3 result = first + second + third;
  return result;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
  mat3 m = Transform::rotate(degrees, glm::normalize(up));
  eye = m * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
  vec3 w = glm::cross(eye,up);
  w = glm::normalize(w);
  mat3 m = Transform::rotate(degrees, w);
  eye = m * eye;
  up = glm::cross(w,eye);
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
  vec3 w = glm::normalize(eye);
  vec3 u = glm::normalize(glm::cross(up,w));
  vec3 v = glm::cross(w,u);
  mat4 m = mat4(u[0],v[0],w[0],0.0,u[1],v[1],w[1],0.0,u[2],v[2],w[2],0.0,0.0,0.0,0.0,1.0);
  mat4 m2 = mat4(1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,-eye[0],-eye[1],-eye[2],1.0);
  mat4 ret = m*m2;
  return ret;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  mat4 ret = mat4(1/tan(fovy/2 * pi/180.0)/aspect,0.0,0.0,0.0,0.0,1/tan(fovy/2 * pi/180.0),0.0,0.0,0.0,0.0,
  -(zNear+zFar)/(zFar-zNear),-1.0,0.0,0.0,-(2*zFar*zNear)/(zFar-zNear),0.0);
  return ret;
  //return glm::perspective(fovy,aspect,zNear,zFar);
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
  mat4 ret = mat4(sx,0.0,0.0,0.0,
                  0.0,sy,0.0,0.0,
                  0.0,0.0,sz,0.0,
                  0.0,0.0,0.0,1.0);
  return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
  mat4 ret = mat4(1.0,0.0,0.0,0.0,
                  0.0,1.0,0.0,0.0,
                  0.0,0.0,1.0,0.0,
                  tx, ty, tz, 1.0);
  return ret;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
