/******************************************************************/
/*         Geometry functions                                     */
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
#include "common.h"
#include "raytrace.h"

point* makePoint(GLfloat x, GLfloat y, GLfloat z) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));
  /* put stuff in it */
  p->x = x; p->y = y; p->z = z; 
  p->w = 1.0;
  return (p);
}

ray* makeRay(vector* dir, point* start, int depth) {
  ray* r;
  r = (ray*) malloc(sizeof(ray));
  r->dir = dir;
  r->start = start;
  return(r);
}

color* makeColor(GLfloat r, GLfloat g, GLfloat b) {
  color* c;
  c = (color*) malloc(sizeof(color));
  c->r = r;
  c->g = g;
  c->b = b;
  return(c);
}

/* makes copy of point (or vector) */
point* copyPoint(point *p0) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));

  p->x = p0->x;
  p->y = p0->y;
  p->z = p0->z;
  p->w = p0->w;         /* copies over vector or point status */
  return (p);
}

/* unallocates a point */
void freePoint(point *p) {
  if (p != NULL) {
    free(p);
  }
}

/* vector from point p to point q is returned in v */
void calculateDirection(point* p, point* q, point* v) {
  v->x = q->x - p->x;
  v->y = q->y - p->y;
  v->z = q->z - p->z;

  /* a direction is a point at infinity */
  v->w = 0.0;
  normalize(v);

  /* NOTE: v is not unit length currently, but probably should be */
}

void reflect(ray* r, point* p, vector* n, ray* refl) {

  GLfloat factor = 2 * (dot(n, r->dir));
  refl->dir->x = r->dir->x - factor * n->x;
  refl->dir->y = r->dir->y - factor * n->y;
  refl->dir->z = r->dir->z - factor * n->z;
  refl->dir->w = 0.0;
  normalize(refl->dir);

  refl->start->x = p->x;
  refl->start->y = p->y;
  refl->start->z = p->z;
}

void through(ray* r, point* p, vector* n, ray* thro) {

  thro->dir->x = r->dir->x;
  thro->dir->y = r->dir->y;
  thro->dir->z = r->dir->z;
  thro->dir->w = 0.0;
  normalize(thro->dir);

  thro->start->x = p->x;
  thro->start->y = p->y;
  thro->start->z = p->z;
}

void add(vector* a, vector* b, vector* v) {
  v->x = a->x + b->x;
  v->y = a->y + b->y;
  v->z = a->z + b->z;
}

GLfloat dot(vector* a, vector*b) {
  return a->x * b->x + a->y * b->y + a->z * b-> z;
}

/* given a vector, sets its contents to unit length */
void normalize(vector* v) {
	GLfloat x = v->x * v->x;
  GLfloat y = v->y * v->y;
  GLfloat z = v->z * v->z;
  GLfloat factor = sqrt(x + y + z);
  v->x = v->x / factor;
  v->y = v->y / factor;
  v->z = v->z / factor;
}

/* point on ray r parameterized by t is returned in p */
void findPointOnRay(ray* r,double t,point* p) {
  p->x = r->start->x + t * r->dir->x;
  p->y = r->start->y + t * r->dir->y;
  p->z = r->start->z + t * r->dir->z;
  p->w = 1.0;
}

/* SPHERES */
sphere* makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
  sphere* s;
  /* allocate memory */
  s = (sphere*) malloc(sizeof(sphere));

  /* put stuff in it */
  s->c = makePoint(x,y,z);   /* center */
  s->r = r;   /* radius */
  s->m = NULL;   /* material */
  return(s);
}

plane* makePlane(vector* n, point* p) {
  plane* p1;
  /* allocate memory */
  p1 = (plane*) malloc(sizeof(plane));

  /* put stuff in it */
  p1->normal = n;   
  normalize(p1->normal);
  p1->position = p;
  return(p1);
}

cone* makeCone(GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat curveX, GLfloat curveY, GLfloat curveZ, GLfloat height) {
  cone* c;
  c = (cone *) malloc(sizeof(cone));
  c->center = makePoint(centerX, centerY, centerZ);
  c->curvature = makePoint(curveX, curveY, curveZ);
  c->height = height;
  return c;
}

ellipsoid* makeEllipsoid(GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat curveX, GLfloat curveY, GLfloat curveZ) {
  ellipsoid* e;

  e = (ellipsoid*) malloc(sizeof(ellipsoid));
  e->center = makePoint(centerX, centerY, centerZ);
  e->curvature = makePoint(curveX, curveY, curveZ);
  return e;
}

/* returns TRUE if ray r hits sphere s, with parameter value in t */
int raySphereIntersect(ray* r,sphere* s,double* t) {
  point p;   /* start of transformed ray */
  double a,b,c;  /* coefficients of quadratic equation */
  double determinate;    /* discriminant */
  point* v;
  double t1;
  double t2;
  
  /* transform ray so that sphere center is at origin */
  /* don't use matrix, just translate! */
  p.x = r->start->x - s->c->x;
  p.y = r->start->y - s->c->y;
  p.z = r->start->z - s->c->z;
  v = r->dir; /* point to direction vector */


  a = v->x * v->x  +  v->y * v->y  +  v->z * v->z;
  b = 2*( v->x * p.x  +  v->y * p.y  +  v->z * p.z);
  c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

  determinate = b * b - 4 * a * c;
  
  if (determinate < 0) {  /* no intersection */
    return FALSE;
  }
  else {
    determinate = sqrt(determinate);
    t1 = (-b + determinate) / (2*a);
    t2 = (-b - determinate) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if ((t1 > 0.0001 && t1 < t2) || (t1 > 0.0001 && t2 <= 0.0001)) {
      *t = t1;
      return TRUE;
    }

    if((t2 > 0.0001 && t2 < t1) || (t2 > 0.0001 && t1 <= 0.0001)) {
      *t = t2;
      return TRUE;
    }
  }
  return FALSE;
}

int rayEllipsoidIntersect(ray* r, ellipsoid* elli, double* t) {
  point p;   /* start of transformed ray */
  double a,b,c;  /* coefficients of quadratic equation */
  double determinate;    /* discriminant */
  point* v;
  double t1;
  double t2;
  *t = INF;
  
  /* transform ray so that sphere center is at origin */
  /* don't use matrix, just translate! */
  p.x = r->start->x - elli->center->x;
  p.y = r->start->y - elli->center->y;
  p.z = r->start->z - elli->center->z;
  v = r->dir; /* point to direction vector */


  a = v->x * v->x/elli->curvature->x  +  v->y * v->y/elli->curvature->y  +  v->z * v->z/elli->curvature->z;
  b = 2*( v->x * p.x/elli->curvature->x  +  v->y * p.y/elli->curvature->y  +  v->z * p.z/elli->curvature->z);
  c = p.x * p.x/elli->curvature->x + p.y * p.y/elli->curvature->y + p.z * p.z/elli->curvature->z - 1.0;

  determinate = b * b - 4 * a * c;
  
  if (determinate < 0) {  /* no intersection */
    return (FALSE);
  }
  else {
    determinate = sqrt(determinate);
    /* First check the root with the lower value of t: */
    /* this one, since D is positive */

    t1 = (-b + determinate) / (2*a);
    t2 = (-b - determinate) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if ((t1 > 0.0001 && t1 < t2) || (t1 > 0.0001 && t2 <= 0.0001)) {
      *t = t1;
      return TRUE;
    }

    if((t2 > 0.0001 && t2 < t1) || (t2 > 0.0001 && t1 <= 0.0001)) {
      *t = t2;
      return TRUE;
    }
    return FALSE;
  }
}

int rayPlaneIntersect(ray* r, plane* plan, double* t) {
  *t = (plan->normal->x * (plan->position->x - r->start->x)+ plan->normal->y * (plan->position->y - r->start->y) + plan->normal->z * (plan->position->z - r->start->z)) / (plan->normal->x * r->dir->x + plan->normal->y * r->dir->y + plan->normal->z * r->dir->z);
  if (*t > 0) {
    return TRUE;
  }
  if (*t <= 0.0001) {
    *t = INF;
    return FALSE;
  }
  *t = INF;
  return FALSE;
}

int rayConeIntersect(ray* r, cone* con, double* t) {
  point p;   /* start of transformed ray */
  point k;
  double a,b,c;  /* coefficients of quadratic equation */
  double determinate;    /* discriminant */
  point* v;
  double t1;
  double t2;
  bool val = FALSE;
  *t = INF;
  
  /* transform ray so that sphere center is at origin */
  /* don't use matrix, just translate! */
  p.x = r->start->x - con->center->x;
  p.y = r->start->y - con->center->y;
  p.z = r->start->z - con->center->z;
  v = r->dir; /* point to direction vector */


  a = v->x * v->x/con->curvature->x  -  v->y * v->y/con->curvature->y  +  v->z * v->z/con->curvature->z;
  b = 2*( v->x * p.x/con->curvature->x  -  v->y * p.y/con->curvature->y  +  v->z * p.z/con->curvature->z);
  c = p.x * p.x/con->curvature->x - p.y * p.y/con->curvature->y + p.z * p.z/con->curvature->z;

  determinate = b * b - 4 * a * c;
  
  if (determinate < 0) {  /* no intersection */
    val = FALSE;
  }
  else {
    determinate = sqrt(determinate);
    /* First check the root with the lower value of t: */
    /* this one, since D is positive */

    t1 = (-b + determinate) / (2*a);
    t2 = (-b - determinate) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if ((t1 > 0.0001 && t1 < t2) || (t1 > 0.0001 && t2 <= 0.0001)) {
      *t = t1;
      val = TRUE;
    }

    if((t2 > 0.0001 && t2 < t1) || (t2 > 0.0001 && t1 <= 0.0001)) {
      *t = t2;
      val = TRUE;
    }
  }

  if (val) {
    findPointOnRay(r, *t, &k);
    if(k.y - con->center->y < -con->height || k.y - con->center->y > 0.0) {
      *t = INF;
      val = FALSE;
    }
  }
  return val;
}

/* normal vector of s at p is returned in n */
void findSphereNormal(sphere* s, point* p, vector* n) {
  n->x = (p->x - s->c->x) / s->r;  
  n->y = (p->y - s->c->y) / s->r;
  n->z = (p->z - s->c->z) / s->r;
  n->w = 0.0;
}

void findEllipsoidNormal(ellipsoid* elli, point* p, vector* n) {
  n->x = (p->x - elli->center->x); 
  n->y = (p->y - elli->center->y); 
  n->z = (p->z - elli->center->z); 
  n->w = 0.0;
}

void findConeNormal(cone* c, point* p, vector* n) {
  n->x = (p->x - c->center->x);
  n->y = 0.0;
  n->z = (p->z - c->center->z);
  n->w = 0.0;
  normalize(n);
}






