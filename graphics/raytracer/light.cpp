/******************************************************************/
/*         Lighting functions                                     */
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

material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat ambient, GLfloat diffuse, GLfloat specular, GLfloat reflectivity, GLfloat translucence, int shininess) {
  material* m;
  
  /* allocate memory */
  m = (material*) malloc(sizeof(material));
  /* put stuff in it */
  m->r = r;
  m->g = g;
  m->b = b;
  m->ambient = ambient;
  m->diffuse = diffuse;
  m->specular = specular;
  m->shininess = shininess;
  m->reflectivity = reflectivity;
  m->translucence = translucence;
  return(m);
}

light* makeLight(point*p, GLfloat ambient, GLfloat diffuse, GLfloat specular) {
  light* l;

  l = (light*) malloc(sizeof(light));
  l->position = p;
  l->ambient = ambient;
  l->diffuse = diffuse;
  l->specular = specular;
  point* j = makePoint(-p->x, -p->y, -p->z);
  l->pane = makePlane(j, p);
  return(l);

}

/* LIGHTING CALCULATIONS */

/* shade */
/* color of point p with normal vector n and material m returned in c */
/* in is the direction of the incoming ray and d is the recusive depth */
void shade(point* p, vector* n, material* m, ray* r, color* c, double fact, int d) {

  vector* in = r->dir;
  double kappa = 1.0/((double)d * d) * fact * (1.0 - m->translucence);

  normalize(n);
  for(int x = 0; x < NUM_LIGHTS; x++) {

    if(d != 0) {
     c->r += m->ambient * m->r * kappa * lights[x]->ambient;
     c->g += m->ambient * m->g * kappa * lights[x]->ambient;
     c->b += m->ambient * m->b * kappa * lights[x]->ambient;
    }

    double factor = 1.0;
    vector lightVector;
    calculateDirection(p, lights[x]->position, &lightVector);
    ray* temp = makeRay(&lightVector, p, 0);

    int hit = FALSE;
    for(int i = 0; i < NUM_SPHERES; i++) {
      double t = INF;
      int tempHit = raySphereIntersect(temp, spheres[i], &t);
      if (tempHit && spheres[i]->m->translucence > 0.0) {
        tempHit = FALSE; 
        factor *= spheres[i]->m->translucence;
      }
      hit = hit || tempHit;
    }

    for(int i = 0; i < NUM_PLANES; i++) {
      double t = INF;
      int tempHit = rayPlaneIntersect(temp, planes[i], &t);
      if (tempHit && planes[i]->m->translucence > 0.0) {
        tempHit = FALSE; 
        factor *= planes[i]->m->translucence;
      }
      hit = hit || tempHit;
    }

    for(int i = 0; i < NUM_ELLIPSOIDS; i++) {
      double t = INF;
      int tempHit = rayEllipsoidIntersect(temp, ellipsoids[i], &t);
      if (tempHit && ellipsoids[i]->m->translucence > 0.0) {
        tempHit = FALSE; 
        factor *= ellipsoids[i]->m->translucence;
      }
      hit = hit || tempHit;
    }

    for(int i = 0; i < NUM_CONES; i++) {
      double t = INF;
      int tempHit = rayConeIntersect(temp, cones[i], &t);
      if (tempHit && cones[i]->m->translucence > 0.0) {
        tempHit = FALSE; 
        factor *= cones[i]->m->translucence;
      }
      hit = hit || tempHit;
    }

    if(!hit) {
      vector* h = makePoint(0.0, 0.0, 0.0);
      vector lightVectorS;
      calculateDirection(p, lights[x]->position, &lightVectorS);
      add(&lightVectorS, in, h);
      normalize(h);
      GLfloat shin = dot(h, n);
      
      if(shin < 0) {
        shin = 0;
      }

      GLfloat specular = pow(shin * lights[x]->specular, m->shininess) * kappa * factor;

      c->r += m->specular * specular;
      c->g += m->specular * specular;
      c->b += m->specular * specular;

      vector lightVectorD;
      calculateDirection(p, lights[x]->position, &lightVectorD);
      GLfloat diff = dot(n, &lightVectorD);
      
      if(diff < 0) {
        diff = 0;
      }

      GLfloat diffuse = diff * lights[x]->diffuse * kappa * factor;

      c->r += m->diffuse * diffuse;
      c->g += m->diffuse * diffuse;
      c->b += m->diffuse * diffuse;
    }   
  }

  /* clamp color values to 1.0 */
  if (c->r > 1.0) {
    c->r = 1.0;
  }
  if (c->g > 1.0) {
    c->g = 1.0;
  }
  if (c->b > 1.0) {
    c->b = 1.0;
  }
}

