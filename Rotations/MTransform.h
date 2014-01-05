#ifndef MTRANSFORM_H
#define MTRANSFORM_H

#include <GL/glew.h>

// a column major matrix library
typedef struct
{
    GLfloat   m[4][4];
} MMatrix;

float MDotProduct(float *a, float *b);

void MCopy(float *cpy, float *src);

void MNormalize(float* a);

void MGramSchmidt(float *a, float *b, float *c);

void MCrossProduct(float *a, float *b, float *c);

void MMultiply(MMatrix *result, MMatrix *A, MMatrix *B);

void MMap(float result[4], MMatrix *T, float input[4]);

void MIdentity(MMatrix *result);

void MTranspose(MMatrix *result, MMatrix *src);

void MTranslate(MMatrix *result, const GLfloat tx, const GLfloat ty, const GLfloat tz);

void MPerspective(MMatrix *result, const float aspect, const float fov, const float n, const float f);

void MViewRotation(MMatrix *result, float *camera);

void MViewingOrigin(MMatrix *result, float *camera);

#endif