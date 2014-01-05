#include <stdlib.h>
#include <math.h>
#include "Sphere.h"

#define PI 3.1415926535897932384626433832795f


//
// first loop fixes height of circle, starting from the top
// second loop iterates around circle, starting from positive z axis heading toward positive x.
// convenient to use half as many horizontal circles, so that angleStep works out cleanly for both loops
// first vertex of each horizontal circle is repeated
// clockwise, top to bottom drawing order
//
void GenSphere ( int numSlices, float radius, Sphere *sphere)
{
   int i;
   int j;
   int numParallels = numSlices / 2;
   int numVertices = ( numParallels + 1 ) * ( numSlices + 1 );
   int numIndices = numParallels * numSlices * 6;
   float angleStep = (2.0f * PI) / ((float) numSlices);

   // Allocate memory for buffers
      sphere->vertices = (GLfloat*) malloc ( sizeof(GLfloat) * 3 * numVertices );
   
	  sphere->texCoords = (GLfloat*) malloc ( sizeof(GLfloat) * 2 * numVertices );

      sphere->indices = (GLuint*) malloc ( sizeof(GLuint) * numIndices );
	  sphere->modelT = (MMatrix *) malloc(sizeof(MMatrix));
	  sphere->modelR = (MMatrix *) malloc(sizeof(MMatrix));
	  sphere->q = (Quaternion *) malloc(sizeof(Quaternion));
	  sphere->q->m[0] = 1.0;
	  sphere->q->m[1]=sphere->q->m[2]=sphere->q->m[3] = 0.0;
	  MIdentity(sphere->modelT);
	  MIdentity(sphere->modelR);
	  sphere->numIndices = numIndices;
	  sphere->radius = radius;
   for ( i = 0; i < numParallels + 1; i++ )
   {
      for ( j = 0; j < numSlices + 1; j++ )
      {
         int vertex = ( i * (numSlices + 1) + j ) * 3; 
         
            sphere->vertices[vertex + 0] = radius * sinf ( angleStep * (float)i ) *
                                               sinf ( angleStep * (float)j );
            sphere->vertices[vertex + 1] = radius * cosf ( angleStep * (float)i );
            sphere->vertices[vertex + 2] = radius * sinf ( angleStep * (float)i ) *
                                               cosf ( angleStep * (float)j );			
			
			int texIndex = ( i * (numSlices + 1) + j ) * 2;
            sphere->texCoords[texIndex + 0] = 1.0f - (float) j / (float) numSlices;
            sphere->texCoords[texIndex + 1] = 1.0f-(float) i  / (float) (numParallels - 0);
      }
   }

   // Generate the indices
      GLuint *indexBuf = sphere->indices;
      for ( i = 0; i < numParallels ; i++ ) 
      {
         for ( j = 0; j < numSlices; j++ )
         {
            *indexBuf++  = i * ( numSlices + 1 ) + j;
            *indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + j;
            *indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );

            *indexBuf++ = i * ( numSlices + 1 ) + j;
            *indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );
            *indexBuf++ = i * ( numSlices + 1 ) + ( j + 1 );
         }
   }
}

bool IntersectSphere(Sphere *sphere, float *rayDir, float *rayStart, float *intersection)
{
	float a, b, c, discriminant;
	float *center = sphere->modelT->m[3];
	float temp[4];
	float t;
	MMatrix iP;
	// let's assume we just have rigid transformations of the sphere.  intersect 
	// the unrotated sphere, then account for the rotation
	a = rayDir[0]*rayDir[0]+ 
		rayDir[1]*rayDir[1]+ 
		rayDir[2]*rayDir[2];
	b = 2*rayDir[0]*(rayStart[0]-center[0])+
		2*rayDir[1]*(rayStart[1]-center[1])+
		2*rayDir[2]*(rayStart[2]-center[2]);
	c = - sphere->radius * sphere-> radius +
		(rayStart[0]-center[0])*(rayStart[0]-center[0])+
		(rayStart[1]-center[1])* (rayStart[1]-center[1]) + 
		(rayStart[2]-center[2])* (rayStart[2]-center[2]);
	
	discriminant = b*b - 4*a*c;

	if(discriminant<0)
		return false;
	t = (-b - sqrt(discriminant))/2.0f/a;
	if (t > 0)
	{
		intersection[0] = rayStart[0]+rayDir[0]*t;
		intersection[1] = rayStart[1]+rayDir[1]*t;
		intersection[2] = rayStart[2]+rayDir[2]*t;
		intersection[3] = 1.0f;
		MTranspose(&iP, sphere->modelR);
		MMap(temp, &iP, intersection);

		MTranspose(&iP, sphere->modelT);
		MMap(intersection, &iP, temp);
		return true;
	}
	t = (-b + sqrt(discriminant))/2.0f/a;
	if ( t > 0)
	{
		intersection[0] = rayStart[0]+rayDir[0]*t;
		intersection[1] = rayStart[1]+rayDir[1]*t;
		intersection[2] = rayStart[2]+rayDir[2]*t;
		intersection[3] = 1.0f;
		MTranspose(&iP, sphere->modelR);
		MMap(temp, &iP, intersection);

		MTranspose(&iP, sphere->modelT);
		MMap(intersection, &iP, temp);
		return true;
	}
	return false;
}