#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>


#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
vector<float> ka;
vector<float> kd;
vector<float> ks;
vector<float> power;
vector<float> points;
vector<float> dirs;


void pv(vector<float> x) {
  for (int k = 0; k < x.size(); k++) {
    cout << x[k];
    cout << "\n";
  }
  cout << "\n";
}

void leaveOnSpace(unsigned char key, int x, int y) {
  if (key == ' ')
    exit(0);
}  
//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}
//square
float sq(float x) {
  return x * x;
}
//normalize v
vector<float> normalize(vector<float> v) {
  float norm = 0.0;
  for (int k = 0; k < v.size(); k++) {
    norm += sq(v[k]);
  }
  norm = sqrt(norm);
  vector<float> resp;
  resp.push_back(v[0] / norm);
  resp.push_back(v[1] / norm);
  resp.push_back(v[2] / norm);
  return resp;
}

//dot product
float dot(vector<float> n, vector<float> l) {
  float resp = 0.0;
  for (int k = 0; k < n.size(); k++) {
    resp += n[k] * l[k];
  }
  return resp;
}
//distance between vectors
float dist(vector<float> a, vector<float> b) {
  float d = 0.0;
  for (int k = 0; k < a.size(); k++) {
    d += sq(a[k] - b[k]);
  }
  return sqrt(d);
}
//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************
float max(float a, float b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}
//give the vector consisting of the diffuse shading component given
//position, lights, and diffusion coefficients
vector<float> handleDiffuse(vector<float> n) {
  vector<float> resp;
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;

  //point sources
  for (int k = 0; k < points.size(); k += 6) {
    vector<float> l;
    l.push_back(points[k] - n[0]); l.push_back(points[k+1] - n[1]); l.push_back(points[k+2] - n[2]);
    float prod = dot(normalize(n), normalize(l));
    red += max(prod * kd[0] * points[k + 3], 0);
    green += max(prod * kd[1] * points[k + 4], 0);
    blue += max(prod * kd[2] * points[k + 5], 0);
  }

  //direction lights
  for (int kk = 0; kk < dirs.size(); kk += 6) {
    vector<float> l;
    l.push_back(dirs[kk]); l.push_back(dirs[kk+1]); l.push_back(dirs[kk+2]);
    float prod = dot(normalize(n), normalize(l));
    red += max(prod * kd[0] * dirs[kk + 3], 0);
    green += max(prod * kd[1] * dirs[kk + 4], 0);
    blue += max(prod * kd[2] * dirs[kk + 5], 0);
  }


  resp.push_back(max(red, 0.0));
  resp.push_back(max(green, 0.0));
  resp.push_back(max(blue, 0.0));
  return resp;
}

//give the fancy specular r-vector
vector<float> calcr(vector<float> l, vector<float> n) {
  vector<float> resp;
  float dotty = dot(l, n);
  for (int k = 0; k < l.size(); k++) {
    resp.push_back(-1.0 * l[k] + dotty * 2 * n[k]);
  }
  return resp;
}
//give the vector consisting of specular shading component given
//position, lights, power, and specular coefficients

vector<float> handleSpecular(vector<float> n) {
  vector<float> resp;
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;
  for (int k = 0; k < points.size(); k += 6) {
    vector<float> l;
    l.push_back(points[k]-n[0]); l.push_back(points[k+1]-n[1]); l.push_back(points[k+2]-n[2]);
    vector<float> r = calcr(l, n);
    vector<float> v;
    v.push_back(0.0);
    v.push_back(0.0);
    v.push_back(1.0);
    float prod = pow(max(dot(normalize(r), normalize(v)), 0), power[0]);
    red += max(prod * ks[0] * points[k + 3], 0);
    green += max(prod * ks[1] * points[k + 4], 0);
    blue += max(prod * ks[2] * points[k + 5], 0);
  }

  for (int kk = 0; kk < dirs.size(); kk += 6) {
    vector<float> l;
    l.push_back(dirs[kk]); l.push_back(dirs[kk+1]); l.push_back(dirs[kk+2]);
    vector<float> r = calcr(l, n);
    vector<float> v;
    v.push_back(0.0);
    v.push_back(0.0);
    v.push_back(1.0);
    float prod = pow(max(dot(normalize(r), normalize(v)), 0), power[0]);
    red += max(prod * ks[0] * dirs[kk + 3], 0);
    green += max(prod * ks[1] * dirs[kk + 4], 0);
    blue += max(prod * ks[2] * dirs[kk + 5], 0);
  }
  resp.push_back(red);
  resp.push_back(green);
  resp.push_back(blue);
  return resp;
}

//returns the ambient light vector
vector<float> handleAmbient() {
  vector<float> resp;
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;
  for (int k = 0; k < points.size(); k += 6) {
    red += max(ka[0] * points[k + 3], 0);
    green += max(ka[1] * points[k + 4], 0);
    blue += max(ka[2] * points[k + 5], 0);
  }

  for (int kk = 0; kk < dirs.size(); kk += 6) {
    red += max(ka[0] * dirs[kk + 3], 0);
    green += max(ka[1] * dirs[kk + 4], 0);
    blue += max(ka[2] * dirs[kk + 5], 0);
  }

  resp.push_back(max(red, 0.0));
  resp.push_back(max(green, 0.0));
  resp.push_back(max(blue, 0.0));
  return resp;
}

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a shaded circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {

  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));


  vector<float> amb = handleAmbient();

  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);
	vector<float> nl;
	nl.push_back(x/radius); nl.push_back(y/radius); nl.push_back(z/radius);
	vector<float> diffuse = handleDiffuse(nl);
	vector<float> spec = handleSpecular(nl);

	setPixel(i,j, diffuse[0] + spec[0] + amb[0], diffuse[1] + spec[1] + amb[1], diffuse[2] + spec[2] + amb[2]);
      }


    }
  }


  glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  for (int k = 0; k < argc; k++) {
    string s = argv[k];
    if (s.compare("-ka") == 0) {
      for (int a = 0; a < 3; a++) {
	ka.push_back(atof(argv[++k]));
      }
    }
    if (s.compare("-kd") == 0) {
      for (int a = 0; a < 3; a++) {
	kd.push_back(atof(argv[++k]));
      }
    }
    if (s.compare("-ks") == 0) {
      for (int a = 0; a < 3; a++) {
	ks.push_back(atof(argv[++k]));
      }
    }
    if (s.compare("-sp") == 0) {
      for (int a = 0; a < 1; a++) {
	power.push_back(atof(argv[++k]));
      }
    }
   if (s.compare("-pl") == 0) {
      for (int a = 0; a < 6; a++) {
	points.push_back(atof(argv[++k]));
     }
    }
   if (s.compare("-dl") == 0) {
      for (int a = 0; a < 3; a++) {
	dirs.push_back(-1.0 * atof(argv[++k]));
      }
      for (int b = 0; b < 3; b++) {
	dirs.push_back(atof(argv[++k]));
      }
    }
  }
  if (ka.size() == 0) {
    ka.push_back(0.0);
    ka.push_back(0.0);
    ka.push_back(0.0);
  }
  if (ks.size() == 0) {
    ks.push_back(0.0);
    ks.push_back(0.0);
    ks.push_back(0.0);
  }
  if (kd.size() == 0) {
    kd.push_back(0.0);
    kd.push_back(0.0);
    kd.push_back(0.0);
  }
  if (power.size() == 0) {
    power.push_back(1.0);
  }
  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to usine a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutKeyboardFunc(leaveOnSpace);
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








