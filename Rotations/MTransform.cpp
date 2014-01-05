#include <stdlib.h>
#include "MTransform.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795f

float MDotProduct(float *a, float *b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void MCopy(float *cpy, float *src)
{
	for (int i = 0; i<4; i++)
		cpy[i] = src[i];
}

void MNormalize(float* a)
{
	float magnitude = sqrt(MDotProduct(a, a));
	a[0]=a[0]/magnitude;
	a[1]=a[1]/magnitude;
	a[2]=a[2]/magnitude;
}
// on b, c should be replaced with a list
void MGramSchmidt(float *a, float *b, float *c)
{
	float magnitude;
	float scale;
	scale = MDotProduct(b, c) / MDotProduct(c, c);
	a[0] = b[0]-scale*c[0];
	a[1] = b[1]-scale*c[1];
	float test = b[2];
	float test2 = c[2];
	float test3 = test-scale*test2;
	a[2] = b[2]-scale*c[2];
	a[3] = 1.0f;
	MNormalize(a);
}

void MCrossProduct(float *a, float *b, float *c)
{
	a[0] = (b[1]*c[2]-b[2]*c[1]);
	a[1] = (b[2]*c[0]-b[0]*c[2]);
	a[2] = (b[0]*c[1]-b[1]*c[0]);
	a[3] = 1.0f;
}
void MMultiply(MMatrix *result, MMatrix *srcB, MMatrix *srcA)
{
    MMatrix    tmp;
    int         i;

	for (i=0; i<4; i++)
	{
		tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
						(srcA->m[i][1] * srcB->m[1][0]) +
						(srcA->m[i][2] * srcB->m[2][0]) +
						(srcA->m[i][3] * srcB->m[3][0]) ;

		tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) + 
						(srcA->m[i][1] * srcB->m[1][1]) +
						(srcA->m[i][2] * srcB->m[2][1]) +
						(srcA->m[i][3] * srcB->m[3][1]) ;

		tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) + 
						(srcA->m[i][1] * srcB->m[1][2]) +
						(srcA->m[i][2] * srcB->m[2][2]) +
						(srcA->m[i][3] * srcB->m[3][2]) ;

		tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) + 
						(srcA->m[i][1] * srcB->m[1][3]) +
						(srcA->m[i][2] * srcB->m[2][3]) +
						(srcA->m[i][3] * srcB->m[3][3]) ;
	}
    memcpy(result, &tmp, sizeof(MMatrix));
}

void MMap(float result[4], MMatrix *T, float input[4])
{
	int i;
	int j;
	result[0] = 0;
	result[1] = 0;
	result[2] = 0;
	result[3] = 0;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			result[j] += (*T).m[i][j] * input[i];
}

void MIdentity(MMatrix *result)
{
    memset(result, 0x0, sizeof(MMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void MTranspose(MMatrix *result, MMatrix *src)
{
	int i, j;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			result->m[i][j]= src->m[j][i];
}

void MTranslate(MMatrix *result, const GLfloat tx, const GLfloat ty, const GLfloat tz)
{
	MIdentity(result);
    result->m[3][0] = tx;
    result->m[3][1] = ty;
    result->m[3][2] = tz;
}

//       1/tan(fov)
//                    1/(aspect*tan(fov)
//                                      f/(f-n)    -2*f*n/(f-n)
//                                      1  
//
//       for a left handed coordinate system, camera facing z+ direction, producing left handed NDC
//       fov in degrees
//       will multiply column vectors
//		 assumes eye is at origin

void MPerspective(MMatrix *result, const float aspect, const float fov, const float n, const float f)
{
	GLfloat a00, a11, a22, a32, a23;
	a00 = 1.0f / tanf(fov*PI/360.0f);
	a11 = a00/aspect;
	a22 = f/(f-n);
	a32 = 1.0f;
	a23 = -2*f*n/(f-n);

	memset(result->m, 0, sizeof(float)*16);
	result->m[0][0] = a00;
	result->m[1][1]=a11;
	result->m[2][2]=a22;
	result->m[2][3]=a32;
	result->m[3][2]=a23;

}

void MViewRotation(MMatrix *result, float *camera)
{
	float x_axis[4];
	float z_axis[4];
	float y_axis[4];
	float up[4] = {0.0f, 1.0f, 0.0f, 0.0f};
	float magnitude;
	float* rotation = result->m[0];
	magnitude = sqrt(MDotProduct(camera, camera));
	z_axis[0]=-camera[0]/magnitude;
	z_axis[1]=-camera[1]/magnitude;
	z_axis[2]=-camera[2]/magnitude;
	z_axis[3] = 0.0f;
	MGramSchmidt(y_axis, up, z_axis);
	y_axis[3]=0.0f;
	MCrossProduct(x_axis, y_axis, z_axis);
	x_axis[3] = 0.0f;
	for(int i =0; i < 4; i++)
	{
		rotation[4*i] = x_axis[i];
		rotation[4*i+1] = y_axis[i];
		rotation[4*i+2] = z_axis[i];
		rotation[4*i+3] = (4*i+3)/15;
	}
}
void MViewingOrigin(MMatrix *result, float *camera)
{
	MMatrix rotation;
	MMatrix translation;
	MViewRotation(&rotation, camera);
	MTranslate(&translation, -camera[0], -camera[1], -camera[2]);
	MMultiply(result, &rotation, &translation);
}