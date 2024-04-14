#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>

#include <FreeImage.h>

#include "Raytrace.h"
#include "readfile.h"
using namespace std;

#define MAINPROGRAM 
#include "variables.h"


void saveImage(string fnam, vector<vector<vec3> > img) {
  int pix = w * h;
  BYTE *pixels = new BYTE[3*pix];	
  for (int i = 0; i < h; i++) {
    for (int j = 0; j <w; j++) {
        vec3 color = img[i][j];
        int index = 3 * ((h-i-1) * w + j);
        pixels[index+2] = (BYTE)(std::min(color.r, 1.0f) * 255.0f);
        pixels[index+1] = (BYTE)(std::min(color.g, 1.0f) * 255.0f);
        pixels[index] = (BYTE)(std::min(color.b, 1.0f) * 255.0f);
    }
  }
  FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

  FreeImage_Save(FIF_PNG, image, fnam.c_str(), 0);
  delete[] pixels;
}

int main(int argc, char* argv[]) {
    FreeImage_Initialise();
    readfile(argv[1]); 
    vector<vector<vec3> > img = Raytrace::raytrace(camera, scene, w, h);
    saveImage(outputFilename, img);
    FreeImage_DeInitialise();
    return 0;
}