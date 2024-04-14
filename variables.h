// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 

#ifdef MAINPROGRAM 
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 center(0.0,0.0,0.0) ; // Center look at point 
int amountinit = 5;
int w = 500, h = 500 ; // width and height 
float fovy = 90.0 ; // For field of view
int maxdepth = 5; //Initial maximum depth (number of bounces) for a ray (default should be 5)
std::string outputFilename = "raytrace.png"; // Default output filename
vec3 ambient(0.2,0.2,.2); //The global ambient color to be added for each object
vec3 attenuation(1,0,0); //Sets the constant, linear and quadratic attenuations
int maxvertsnum = 100000000;
int maxvertnormsnum = 100000000;
float c0 = 1;
float c1 = 0;
float c2 = 0;
int dirnumused = 0;
int poirnumused = 0;
#else 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int amountinit;
EXTERN int w, h ; 
EXTERN float fovy ; 
EXTERN int maxdepth;
EXTERN string outputFilename;
EXTERN int maxvertsnum;
EXTERN int maxvertnormsnum;
EXTERN vec3 ambient ;
//attenuation
EXTERN float c0,c1,c2;
EXTERN int dirnumused;
EXTERN int poirnumused;
#endif 

EXTERN std::vector<vec3> vertices;
EXTERN std::vector<vec3> vertexnormals;
EXTERN std::vector<Object*> spheres;
EXTERN std::vector<Object*> triangles;
EXTERN Scene *scene;
EXTERN Camera *camera;

EXTERN mat4 projection, modelview; // The mvp matrices

static enum {view, translate, scale} transop ; // which operation to transform 

EXTERN float sx, sy ; // the scale in x and y 
EXTERN float tx, ty ; // the translation in x and y

// Lighting parameter array, similar to that in the fragment shader
EXTERN vector<float> directionallp ; // Directional Light Positions
EXTERN vector<float> directionallc ; // Directional Light Colors
//EXTERN vector<float> directionallt ; // Directional Lights transformed by modelview
//EXTERN int dirnumused ;                 // How many directional lights are used 

EXTERN vector<float> pointlp ; // Point Light Positions
EXTERN vector<float> pointlc ; // Point Light Colors
//EXTERN vector<float> pointlt ; // Point Lights transformed by modelview
//EXTERN int poirnumused ;          // How many point lights are used 

// Materials (read from file) 
EXTERN vec3 diffuse ; //the diffuse color of the surface
EXTERN vec3 specular ; //the specular color of the surface
EXTERN vec3 emission ; //the emissive color of the surface
EXTERN float shininess ; // the shininess of the surface
