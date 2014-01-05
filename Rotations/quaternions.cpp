#include <math.h>
#include "quaternions.h"

void NormalizeQ(Quaternion *q)
{
	double magnitude = sqrt(q->m[0]*q->m[0]+q->m[1]*q->m[1]+q->m[2]*q->m[2]+q->m[3]*q->m[3]);
	q->m[0]=q->m[0]/magnitude;
	q->m[1]=q->m[1]/magnitude;
	q->m[2]=q->m[2]/magnitude;
	q->m[3]=q->m[3]/magnitude;
}

void CopyQ(Quaternion *q, Quaternion *src)
{
	for(int i = 0; i<4; i++)
		q->m[i] = src->m[i];
}
void ConstructQ(Quaternion *q, float* ray, float angle)
{
	double norm = sqrt(MDotProduct(ray, ray));
	double s = sin(angle/2.0f);
	q->m[0] = cos(angle/2.0f);
	q->m[1] = s*ray[0]/norm;
	q->m[2] = s*ray[1]/norm;
	q->m[3] = s*ray[2]/norm;
}

void QToM(MMatrix *M, Quaternion *q)
{
	double wx, wy, wz, xx, xy, xz, yy, yz, zz;
	float* mat = M->m[0];
    wx = q->m[0] * q->m[1];
    wy = q->m[0] * q->m[2];
    wz = q->m[0] * q->m[3];

    xx = q->m[1] * q->m[1];
    xy = q->m[1] * q->m[2];
    xz = q->m[1] * q->m[3];

    yy = q->m[2] * q->m[2];
    yz = q->m[2] * q->m[3];

	zz = q->m[3]*q->m[3];

    mat[0]  = (float) (1.0 - 2.0f * ( yy + zz ));
    mat[1]  = (float) (2.0 * ( xy + wz ));
    mat[2]  = (float) (2.0 * ( xz - wy ));

    mat[4]  = (float) (2.0 * ( xy - wz ));
    mat[5]  = (float) (1.0 - 2.0 * ( xx + zz ));
    mat[6]  = (float) (2.0 * ( yz + wx ));

    mat[8]  = (float) (2.0 * ( xz + wy ));
    mat[9]  = (float) (2.0 * ( yz - wx ));
    mat[10] = (float) (1.0 - 2.0 * ( xx + yy ));

    mat[3]  = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0.0f;
    mat[15] = 1.0f;
}

void MultiplyQ(Quaternion *a, Quaternion *b, Quaternion *c)
{
    a->m[0] = -b->m[1] * c->m[1] - b->m[2] * c->m[2] - b->m[3]* c->m[3]+ b->m[0] * c->m[0];
	a->m[1]  = b->m[1]* c->m[0] + b->m[2] * c->m[3] - b->m[3] * c->m[2] + b->m[0] * c->m[1];
    a->m[2] = -b->m[1]* c->m[3]+ b->m[2] * c->m[0] + b->m[3]* c->m[1] + b->m[0] * c->m[2];
    a->m[3] =  b->m[1] * c->m[2] - b->m[2] * c->m[1] + b->m[3] * c->m[0] + b->m[0] * c->m[3];
}