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
vector<float> sp;
vector<float> pl;
vector<float> dl;


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
vector<float> handleDiffuse(vector<float> coeff, vector<float> n, vector<float> lights) {
  vector<float> resp;
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;
  for (int k = 0; k < lights.size(); k += 6) {
    vector<float> l;
    l.push_back(lights[k]); l.push_back(lights[k+1]); l.push_back(lights[k+2]);

    //nobody cares about falloff
    //float rsq = sq(dist(l, n));

    float prod = dot(normalize(n), normalize(l));
    red += prod * coeff[0] * lights[k + 3];
    green += prod * coeff[1] * lights[k + 4];
    blue += prod * coeff[2] * lights[k + 5];
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
	vector<float> diffuse = handleDiffuse(kd, nl, pl); 
        //setPixel(i,j, 1.0, 0.0, 0.0);

        // This is amusing, but it assumes negative color values are treated reasonably.
         setPixel(i,j, diffuse[0], diffuse[1], diffuse[2]);
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
	sp.push_back(atof(argv[++k]));
      }
    }
   if (s.compare("-pl") == 0) {
      for (int a = 0; a < 6; a++) {
	pl.push_back(atof(argv[++k]));
      }
    }
   if (s.compare("-dl") == 0) {
      for (int a = 0; a < 6; a++) {
	dl.push_back(atof(argv[++k]));
      }
    }
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








