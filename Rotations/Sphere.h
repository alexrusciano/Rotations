#ifndef SPHERE_H
#define SPHERE_H

#include <GL/glew.h>
#include "MTransform.h"
#include "quaternions.h"

typedef struct
{
   GLfloat *vertices;
   GLuint  *indices;
   GLfloat *texCoords;
   GLuint numIndices;
   MMatrix *modelT;
   MMatrix *modelR;
   Quaternion *q;
   GLfloat radius;
} Sphere;

void GenSphere ( int numSlices, float radius, Sphere *sphere);

bool IntersectSphere(Sphere *sphere, float *rayDir, float* rayStart, float* intersection);

#endif