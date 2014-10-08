#ifndef _RAYTRACE_H_
#define _RAYTRACE_H_

/******************************************************************/
/*         Raytracer declarations                                 */
/******************************************************************/


/* constants */
#define TRUE 1
#define FALSE 0

#define PI 3.14159265358979323846264338327
#define INF 99999999999999999999999.99999

#define NUM_LIGHTS 4
#define NUM_SPHERES 10
#define NUM_ELLIPSOIDS 4
#define NUM_PLANES 4
#define NUM_CONES 2

/* data structures */

typedef struct material {
  /* color */
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  /* ambient reflectivity */
  GLfloat ambient;
  GLfloat diffuse;
  GLfloat specular;
  GLfloat reflectivity;
  GLfloat translucence;
  int shininess;
} material;

typedef struct point {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
} point;

/* a vector is just a point */
typedef point vector;

/* a ray is a start point and a direction */
typedef struct ray {
  point* start;
  vector* dir;
  double factor;
} ray;

typedef struct color {
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  /* these should be between 0 and 1 */
} color;

typedef struct sphere {
  point* c;  /* center */
  GLfloat r;  /* radius */
  material* m;
} sphere;

typedef struct plane {
  point* position;
  vector* normal;
  material* m;
} plane;

//x^2/a^2 + y^2/b^2 + z^2/c^2 - 1 = 0
typedef struct ellipsoid {
  point* center;
  point* curvature;
  material* m;
} ellipsoid;

typedef struct cone {
  point* center;
  point* curvature;
  GLfloat height;
  material* m;
} cone;

//all lights point toward center atm
typedef struct light {
  point* position;
  GLfloat ambient;
  GLfloat diffuse;
  GLfloat specular;
  plane* pane;
} light;



/* functions in raytrace.cpp */
void traceRay(ray*, color*, int);

/* functions in geometry.cpp */
sphere* makeSphere(GLfloat, GLfloat, GLfloat, GLfloat);
plane* makePlane(vector*, point*);
ellipsoid* makeEllipsoid(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
cone* makeCone(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
point* makePoint(GLfloat, GLfloat, GLfloat);
color* makeColor(GLfloat, GLfloat, GLfloat);
point* copyPoint(point *);
ray* makeRay(vector*, point*, int);
void freePoint(point *);
void calculateDirection(point*,point*,point*);
void findPointOnRay(ray*,double,point*);
int raySphereIntersect(ray*,sphere*,double*);
int rayPlaneIntersect(ray*, plane*, double*);
int rayEllipsoidIntersect(ray*, ellipsoid*, double*);
int rayConeIntersect(ray*, cone*, double*);
void findSphereNormal(sphere*,point*,vector*);
void findEllipsoidNormal(ellipsoid*, point*, vector*);
void findConeNormal(cone*, point*, vector*);
void reflect(ray*, point*, vector*, ray*);
void through(ray*, point*, vector*, ray*);
GLfloat dot(vector*, vector*);
void normalize(vector*);
void add(vector*, vector*, vector*);

/* functions in light.cpp */
material* makeMaterial(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, int);
light* makeLight(point*, GLfloat, GLfloat, GLfloat);
void shade(point*,vector*,material*,ray*,color*,double, int);

/* global variables */
extern int width;
extern int height;

extern light* lights[];
extern sphere* spheres[];
extern ellipsoid* ellipsoids[];
extern plane* planes[];
extern cone* cones[];
extern sphere* spheres[];

#endif	/* _RAYTRACE_H_ */
