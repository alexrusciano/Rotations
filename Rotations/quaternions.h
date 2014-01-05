#ifndef QUATERIONS_H
#define QUATERIONS_H

#include "MTransform.h"

typedef struct
{
	double m[4];
} Quaternion;

void NormalizeQ(Quaternion *q);

void CopyQ(Quaternion *q, Quaternion *src);

void ConstructQ(Quaternion *q, float* ray, float angle);

void QToM(MMatrix *M, Quaternion *q);

void MultiplyQ(Quaternion *a, Quaternion *b, Quaternion *c);

#endif