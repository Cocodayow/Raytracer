// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <vector>
#include <string>

#include "Geometry.h"
using namespace std;
#include "variables.h" 
#include "readfile.h"

void matransform(std::stack<mat4> &transfstack, float* values) 
{
  mat4 transform = transfstack.top(); 
  vec4 valvec = vec4(values[0],values[1],values[2],values[3]); 
  vec4 newval = transform * valvec; 
  for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void rightmultiply(const mat4 & M, std::stack<mat4> &transfstack) 
{
  mat4 &T = transfstack.top(); 
  T = T * M; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, float* values) 
{
  for (int i = 0; i < numvals; i++) {
    s >> values[i]; 
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n"; 
      return false;
    }
  }
  return true; 
}

void readfile(const char* filename) 
{
  string str, cmd; 
  ifstream in;
  in.open(filename); 
  
  if (in.is_open()) {

    // I need to implement a matrix stack to store transforms.  
    // This is done using standard STL Templates 
    stack <mat4> transfstack; 
    transfstack.push(mat4(1.0));  // identity

    getline (in, str); 
    while (in) {
      if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
        // Ruled out comment and blank lines 

        stringstream s(str);
        s >> cmd; 
        int i; 
        float values[10]; // Position and color for light, colors for others
        // Up to 10 params for cameras.  
        bool validinput; // Validity of input 

        // Lighting Command
        //Directional Lights
        if (cmd == "directional") {
            validinput = readvals(s, 6, values); // Position/color for lts.
            if (validinput) {
              for (i = 0; i < 3; i++) {
                directionallp.push_back(values[i]);
                directionallc.push_back(values[i+3]);
              }
              dirnumused++; 
            }
        }
        //Point Lights
        else if (cmd == "point"){
            validinput = readvals(s, 6, values); // Position/color for lts.
            if (validinput) {
              for (i = 0; i < 3; i++) {
                pointlp.push_back(values[i]);
                pointlc.push_back(values[i+3]);
              }
              poirnumused++; 
            }
        }
        //Attenuation
        else if (cmd == "attenuation"){
            validinput = readvals(s, 3, values);
            if (validinput) {
                c0 = values[0];
                c1 = values[1];
                c2 = values[2];
            }
        }

        // Material Commands 
        // Ambient for each object.

        else if (cmd == "ambient") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            for (i = 0; i < 3; i++) {
              ambient[i] = values[i]; 
            }
          }
        } 
        
        //Diffuse, specular, shininess properties of the surface

        else if (cmd == "diffuse") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              diffuse[i] = values[i]; 
            }
          }
        } else if (cmd == "specular") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              specular[i] = values[i]; 
            }
          }
        } else if (cmd == "emission") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              emission[i] = values[i]; 
            }
          }
        } else if (cmd == "shininess") {
          validinput = readvals(s, 1, values); 
          if (validinput) {
            shininess = values[0]; 
          }
        } 
        
        //Size, maxdepth and outputfile name
        else if (cmd == "size") {
          validinput = readvals(s,2,values); 
          if (validinput) { 
            w = (int) values[0];
            h = (int) values[1]; 
          } 
        } else if (cmd == "maxdepth") {
            validinput = readvals(s, 1, values);
            if (validinput) {
            maxdepth = values[0]; 
          }
        } else if (cmd == "output") {
            s >> outputFilename;
          
        }

        //Camera
        else if (cmd == "camera") {
          validinput = readvals(s,10,values); // 10 values eye cen up fov
          if (validinput) {
            eyeinit = vec3(values[0], values[1], values[2]);
            center = vec3(values[3], values[4], values[5]);
            upinit = vec3(values[6], values[7], values[8]);
            //upinit = Transform::upvector(upvec, eyeinit-center);
            fovy = values[9];
          }
        }

        //Geometry
        else if (cmd == "maxverts") {
            validinput = readvals(s,1,values); 
            if (validinput) {
            maxvertsnum = values[0]; 
            }
        }else if (cmd == "maxvertnorms") {
            validinput = readvals(s,1,values); 
            if (validinput) {
            maxvertnormsnum = values[0]; 
            }
        }else if (cmd == "vertex") {
            validinput = readvals(s, 3, values); 
            if (validinput) {
                  vec3 v = vec3(values[0],values[1],values[2]);
                  vertices.push_back(v);
            }
        }else if (cmd == "vertexnormal") {
            // validinput = readvals(s, 6, values); 
            // if (validinput) {
            //       vec3 v = vec3(values[3],values[4],values[5]);
            //       vertexnormals.push_back(v);
            // }
        }else if (cmd == "sphere") {
            validinput = readvals(s, 4, values); 
            if (validinput) {
              vec3 center = vec3(values[0], values[1],values[2]);
              mat4 transform = transfstack.top();
              Object *s = new Sphere(center,values[3], transform,ambient,emission,diffuse,specular,shininess);
              spheres.push_back(s);
            }
        }else if (cmd == "tri" ) {
            validinput = readvals(s, 3, values); 
            if (validinput) {
              vec4 transVertex[3];
              vec3 newVertex[3];
              for (int i = 0; i< 3; i++){
                vec4 vertex = vec4(vertices[values[i]],1.0f);
                mat4 transform = transfstack.top();
                transVertex[i] = transform * vertex;
                newVertex[i] = vec3(transVertex[i]);
              }
              Object *t = new Triangle(newVertex[0],newVertex[1],newVertex[2],ambient,emission,diffuse,specular,shininess);
              triangles.push_back(t);
            }
        }

        //Transformations
        //these commands are exactly the same as in assignment 2
        else if (cmd == "translate") {
          validinput = readvals(s,3,values); 
          if (validinput) {
            rightmultiply(Transform::translate(values[0], values[1], values[2]), transfstack);
          }
        }
        else if (cmd == "scale") {
          validinput = readvals(s,3,values); 
          if (validinput) {
            rightmultiply(Transform::scale(values[0], values[1], values[2]), transfstack);
          }
        }
        else if (cmd == "rotate") {
          validinput = readvals(s,4,values); 
          if (validinput) {
            vec3 axis = vec3(values[0], values[1], values[2]);
            mat3 rotation = Transform::rotate(values[3], axis);
            mat4 rotationMat4 = mat4(rotation[0][0], rotation[0][1], rotation[0][2], 0.0,
                rotation[1][0], rotation[1][1], rotation[1][2], 0.0,
                rotation[2][0], rotation[2][1], rotation[2][2], 0.0, 0.0, 0.0, 0.0, 1.0);
            rightmultiply(rotationMat4, transfstack);
          }
        }

        //the basic push/pop code for matrix stacks
        else if (cmd == "pushTransform") {
          transfstack.push(transfstack.top()); 
        } else if (cmd == "popTransform") {
          if (transfstack.size() <= 1) {
            cerr << "Stack has no elements.  Cannot Pop\n"; 
          } else {
            transfstack.pop(); 
          }
        }

        else {
          cerr << "Unknown Command: " << cmd << " Skipping \n"; 
        }
      }
      getline (in, str); 
    }
    //set up scene and camera
    scene = new Scene(spheres, triangles);
    camera = new Camera(eyeinit,upinit,center,fovy);

    // Set up initial position for eye, up and amount
    // As well as booleans 

  } else {
    cerr << "Unable to Open Input Data File " << filename << "\n"; 
    throw 2; 
  }
}
