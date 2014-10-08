/******************************************************************/
/*         Main raytracer file                                    */
/*                                                                */
/* Group Members: <FILL IN>                                       */
/******************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include "common.h"
#include "lowlevel.h"
#include "raytrace.h"

/* local functions */
void initScene(void);
void initCamera (int, int);
void display(void);
void init(int, int);
void traceRay(ray*,color*);
void drawScene(void);
int firstHit(ray*,point*,vector*, material* *m);


/* local data */

light* lights[NUM_LIGHTS];
sphere* spheres[NUM_SPHERES];
cone* cones[NUM_CONES];
ellipsoid* ellipsoids[NUM_ELLIPSOIDS];
plane* planes[NUM_PLANES];

/* the viewing parameters: */
point* viewpoint;
GLfloat pnear;       /* distance from viewpoint to image plane */
GLfloat fovx;        /* x-angle of view frustum */
int width = 500;     /* width of window in pixels */
int height = 350;    /* height of window in pixels */
double box = 1.0;
int CAP = 10;
int ALIASING = 5;

int main (int argc, char** argv) {
  int win;

  glutInit(&argc,argv);
  glutInitWindowSize(width,height);
  glutInitWindowPosition(100,100);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  win = glutCreateWindow("raytrace");
  glutSetWindow(win);
  init(width,height);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}

void init(int w, int h) {
  /* OpenGL setup */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, box, 0.0, box, -box, 2*box);
  gluLookAt(0.0, box, -box, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0);
  glClearColor(0.0, 0.0, 0.0, 0.0);  

  /* low-level graphics setup */
  initCanvas(w,h);

  /* raytracer setup */
  initCamera(w,h);
  initScene();  
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  drawScene();  /* draws the picture in the canvas */
  flushCanvas();  /* draw the canvas to the OpenGL window */
  glFlush();
}

void initScene () {

  /* Defines 4 lights pointed toward the orgin offset from eachother by 2.5 along x axis */
  for(int x = 0; x < NUM_LIGHTS; x++) {
    lights[x] = makeLight(makePoint((double) x * 2.5, 1.0, 200.0), 1.0, 0.1, 0.05);
  }

  /* Defines a line of spheres along the x axis */
  for(int x = 0; x < NUM_SPHERES; x++) {
    spheres[x] = makeSphere(x * 0.125 - 0.4, 0.0, -2.0, 0.06);
    spheres[x]->m = makeMaterial(x * 0.1, 0.0, 1.0, 0.05, 1.0, 1.0, 1.0, 0.0, 1);
  }

  /* Defines 4 reflecting mirrors to create a box */
    planes[0] = makePlane(makePoint(1.5, 0.0, 0.2), makePoint(-0.3, 0.0, -4.0));
    planes[0]->m = makeMaterial(0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 1.0, 0.0, 1);

    planes[1] = makePlane(makePoint(0.0, 1.0, 0.), makePoint(0.0, -0.1, 0.0));
    planes[1]->m = makeMaterial(0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 1.0, 0.0, 10);

    planes[2] = makePlane(makePoint(0.0, -0.3, 1.0), makePoint(0.0, 0.0, -2.5));
    planes[2]->m = makeMaterial(0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 1.0, 0.0, 10);

    planes[3] = makePlane(makePoint(-1.5, 0.0, 0.2), makePoint(0.3, 0.0, -4.0));
    planes[3]->m = makeMaterial(0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 1.0, 0.0, 10);
 
  /* Creates vertically stacked ellipsoids of increasing opacity */
  for(int x = 0; x < NUM_ELLIPSOIDS; x++) {
    ellipsoids[x] = makeEllipsoid(0.0, 0.2 * x, -1.85, 0.006, 0.008, 0.005);
    ellipsoids[x]->m = makeMaterial(0.0, 0.3 * x, 0.3, 0.05, 1.0, 1.0, 1.0, 0.45 - 0.15 * x, 100);
  }
  
  /* Creates two identical cones on either size of the origin*/
  for(int x = 0; x < NUM_CONES; x++) {
    cones[x] = makeCone(0.2 - 0.4 * x, 0.0, -1.5, 0.005, 0.01, 0.008, 0.1);
    cones[x]->m = makeMaterial(1.0, 0.0, 0.2, 0.2, 1.0, 1.0, 0.0, 0.0, 100);
  } 
}

void initCamera (int w, int h) {
  viewpoint = makePoint(0.0,0.0,0.0);
  pnear = 1.0;
  fovx = PI/3;
}

void drawScene () {
  int i,j;
  GLfloat imageWidth;
  /* declare data structures on stack to avoid dynamic allocation */
  point worldPix;  /* current pixel in world coordinates */
  point direction; 
  ray r;
  color c;

  /* initialize */
  worldPix.w = 1.0;
  worldPix.z = -pnear;

  r = *(makeRay(&direction, &worldPix, 2));

  imageWidth = 2*pnear*tan(fovx/2);

  /* trace a ray for every pixel */
  for (i=0; i<width; i++) {
    for (j=0; j<height; j++) {
      
      /* find direction */
      /* note: direction vector is NOT NORMALIZED */
      c.r = 0;
      c.g = 0; 
      c.b = 0;
      for(int a = 0; a < ALIASING; a++) {
        calculateDirection(viewpoint,&worldPix,&direction);
        color ct;
        ct.r = 0;
        ct.b = 0;
        ct.g = 0;
        
        /* find position of pixel in world coordinates */
        /* y position = (pixel height/middle) scaled to world coords */ 
        worldPix.y = (j-((height)/2))*imageWidth/width;
        /* x position = (pixel width/middle) scaled to world coords */ 
        worldPix.x = (i-((width)/2))*imageWidth/width;

        double wide = ((j+1)-(height/2))*imageWidth/width - (j-(height/2))*imageWidth/width;
        double high = ((i+1)-(width/2))*imageWidth/width - (i-(width/2))*imageWidth/width;

        double num_w = rand() % 100 * wide/100;
        double num_h = rand() % 100 * high/100;

        worldPix.y += num_w;
        worldPix.x += num_h;
        
        r.factor = 1.0;
        traceRay(&r,&ct,1);
        c.r += ct.r;
        c.b += ct.b;
        c.g += ct.g;
      }
      /* write the pixel! */
      c.r /= ALIASING;
      c.g /= ALIASING;
      c.b /= ALIASING;
      drawPixel(i,j,c.r,c.g,c.b);
    } 
  }
}

/* returns the color seen by ray r in parameter c */
/* d is the recursive depth */
void traceRay(ray* r, color* c, int d) {
  point p;
  vector n;
  material* m;

  if (d >= CAP) {
    return;
  }

  p.w = 0.0;  /* inialize to "no intersection" */
  firstHit(r,&p,&n, &m);

  if (p.w != 0.0) {
    vector* v = makePoint(0.0, 0.0, 0.0);
    point* s = makePoint(0.0, 0.0, 0.0);
    ray* refl = makeRay(v, s, 0);
    reflect(r, &p, &n, refl);
    refl->factor = m->reflectivity * (1.0 - m->translucence);
    if(refl->factor != 0) {
      traceRay(refl, c, d+1); 
    }

    vector* w = makePoint(0.0, 0.0, 0.0);
    point* q = makePoint(0.0, 0.0, 0.0);
    ray* thro = makeRay(w, q, 0);

    through(r, &p, &n, thro);
    double val = m->translucence;
    thro->factor = val;
    if(thro->factor > 0.001) {
      traceRay(thro, c, d);
    }
    shade(&p, &n, m, r, c, r->factor, d); /* do the lighting calculations */
  } else {             /* nothing was hit */
    c->r += 0.0;
    c->g += 0.0;
    c->b += 0.0;
  }
}


/* firstHit */
/* If something is hit, returns the finite intersection point p, 
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
int firstHit(ray* r, point* p, vector* n, material* *m) {

  double T_sphere = INF;
  double T_ellipsoid = INF;
  double T_cone = INF;
  double T_plane = INF;

  int hit_sphere = FALSE;
  int hit_plane = FALSE;
  int hit_ellipsoid = FALSE;
  int hit_cone = FALSE;

  sphere* s;
  ellipsoid* e;
  cone* c;
  plane* pl;

  double t = INF;
  for(int i = 0; i < NUM_SPHERES; i++) {  
    int tempHit = FALSE;
    tempHit = raySphereIntersect(r, spheres[i], &t);
    hit_sphere = hit_sphere || tempHit;
  
    if(T_sphere > t) {
      T_sphere = t;
      s = spheres[i];
    }
  }

  t = INF;
  for(int i = 0; i < NUM_PLANES; i++) {  
    int tempHit = FALSE;
    tempHit = rayPlaneIntersect(r, planes[i], &t);
    hit_plane = hit_plane || tempHit;
  
    if(T_plane > t) {
      T_plane = t;
      pl = planes[i];
    }
  }

  t = INF;
  for(int i = 0; i < NUM_ELLIPSOIDS; i++) {  
    int tempHit = FALSE;
    tempHit = rayEllipsoidIntersect(r, ellipsoids[i], &t);
    hit_ellipsoid = hit_ellipsoid || tempHit;
  
    if(T_ellipsoid > t) {
      T_ellipsoid = t;
      e = ellipsoids[i];
    }
  }

  t = INF;
  for(int i = 0; i < NUM_CONES; i++) {  
    int tempHit = FALSE;
    tempHit = rayConeIntersect(r, cones[i], &t);
    hit_cone = hit_cone || tempHit;
  
    if(T_cone > t) {
      T_cone = t;
      c = cones[i];
    }
  }
  
  if(hit_sphere && T_sphere < T_cone && T_sphere < T_ellipsoid && T_sphere < T_plane) {
    *m = s->m;
    findPointOnRay(r, T_sphere, p);
    findSphereNormal(s, p, n);
  } else if(hit_ellipsoid && T_ellipsoid < T_sphere && T_ellipsoid < T_cone && T_ellipsoid < T_plane) {
    *m = e->m;
    findPointOnRay(r, T_ellipsoid, p);
    findEllipsoidNormal(e, p, n);
  } else if (hit_cone && T_cone < T_ellipsoid && T_cone < T_sphere && T_cone < T_plane) {
    *m = c->m;
    findPointOnRay(r, T_cone, p);
    findConeNormal(c, p, n);
  }else if (hit_plane && T_plane < T_ellipsoid && T_plane < T_cone && T_plane < T_sphere) {
    *m = pl->m;
    findPointOnRay(r,T_plane,p);
    n->x = pl->normal->x;
    n->y = pl->normal->y;
    n->z = pl->normal->z;
  } else {
    p->w = 0.0;
  }
  return 0;
}

