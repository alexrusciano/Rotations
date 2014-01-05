// Rotations.cpp : Defines the entry point for the application.
// texture from http://www.celestiamotherlode.net/catalog/earth.php

#include "Rotations.h"
#include "AAWindow.h"
#include "Shader.h"
#include "Sphere.h"
#include "MTransform.h"
#include "quaternions.h"
#include "FreeImage.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795f

#define ANG_MOMENTUM 2.0 * PI / 10.0

typedef struct
{
	bool animating;
	float current_theta;
	float target_theta;
	float axis[4];
} Animation;

typedef struct
{
   // Handle to a program object
   GLuint programObject;
   
   // Attribute locations
   GLuint earth_texture;
   GLint texLoc;
   GLint  positionLoc;
   GLint mvpLoc;
   GLint texCoordsLoc;
   Sphere* sphere;
   float eye [3];
   float lookat[3];
   Animation* animation;
} UserData;

int Init ( AAContext *aaContext )
{
    UserData *userData = (UserData*) aaContext->userData;
	userData->animation = (Animation*) malloc(sizeof(Animation));
	memset(userData->animation, 0x0, sizeof(Animation));
    glGenTextures(1, &userData->earth_texture);
	glBindTexture(GL_TEXTURE_2D, userData->earth_texture);
    FIBITMAP* bitmap = FreeImage_Load(
        FreeImage_GetFileType("earth.png", 0),
        "earth.png");
    FIBITMAP *pImage = FreeImage_ConvertTo32Bits(bitmap);
    int nWidth = FreeImage_GetWidth(pImage);
    int nHeight = FreeImage_GetHeight(pImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));
	FreeImage_Unload(pImage);
	FreeImage_Unload(bitmap); // correct?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

   const char vShaderStr[] =  
	  "uniform mat4 u_mvp;      					 \n"
	  "attribute vec2 a_texCoords;                   \n"
	  "varying vec2 v_texCoords;						 \n"
      "attribute vec4 a_position;					 \n"
	  "out float debug;								\n"
      "void main()									 \n"
      "{											 \n"
	  "vec4 pos = u_mvp * a_position;		\n"
	  "gl_Position = pos;                      \n"
	  "v_texCoords = a_texCoords;					\n"
	  "debug = a_position.y;                      \n"
      "}                                             \n";
   
   const char fShaderStr[] =  
	  "in float debug;                                     \n"
	  "varying vec2 v_texCoords;								\n"
	  "uniform sampler2D s_earthTexture;                   \n"
      "precision mediump float;                            \n"
      "void main()                                         \n"
      "{                                                   \n"
	  "vec4 color = texture(s_earthTexture, v_texCoords);     \n"
//	  "if(debug>0) \n"
//	  "color = vec4(1.0, 0.0, 0.0, 1.0); \n"
	  "gl_FragColor = color;                               \n"
      "}                                                   \n";

   // Load the shaders and get a linked program object
   userData->programObject = LoadProgram ( vShaderStr, fShaderStr );

   // Get the attribute locations
   userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );
   userData->mvpLoc = glGetUniformLocation(userData->programObject, "u_mvp");

   userData->texCoordsLoc = glGetAttribLocation(userData->programObject, "a_texCoords");
   userData->texLoc = glGetUniformLocation(userData->programObject, "s_earthTexture");
   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   glEnable(GL_DEPTH_TEST);
   return TRUE;
}

void SetAnimation(AAContext *aaContext, float* axis, float current_theta, float target_theta, bool animating)
{
	UserData *userData = (UserData*) aaContext->userData;
	userData->animation->animating = animating;
	userData->animation->current_theta = current_theta;
	userData->animation->target_theta = target_theta;
	MCopy(userData->animation->axis, axis);
}
void UpdateAnimation(AAContext *aaContext)
{
	UserData *userData = (UserData*) aaContext->userData;
	Quaternion temp1, temp2;
	float elapsed = (float)(aaContext->new_elapsed.QuadPart-aaContext->old_elapsed.QuadPart)/
		(float) aaContext->tps.QuadPart;
	float dtheta = ANG_MOMENTUM *elapsed;
	SetAnimation(aaContext, userData->animation->axis, 
		0.0, 
		userData->animation->target_theta-dtheta, 
		true);
	ConstructQ(&temp1, userData->animation->axis, dtheta);
	CopyQ(&temp2, userData->sphere->q);
	MultiplyQ(userData->sphere->q, &temp2, &temp1);
	NormalizeQ(userData->sphere->q);
	QToM(userData->sphere->modelR, userData->sphere->q);
	if(userData->animation->target_theta-userData->animation->current_theta < .01)
		memset(userData->animation, 0x0, sizeof(Animation));
}

void Draw ( AAContext *aaContext )
{
   UserData *userData = (UserData*) aaContext->userData;
   if(userData->animation->animating)
		UpdateAnimation(aaContext);
   MMatrix view;
   MViewingOrigin(&view, userData->eye);
   MMatrix mv;
   MMatrix model;
   MMultiply(&model, userData->sphere->modelT, userData->sphere->modelR);
   MMultiply(&mv, &view, &model);
   float aspect = 1.0;
   float fov = 90.0f;
   float n = 1.0;
   float f = 80.0;
   MMatrix perspective;
   MPerspective(&perspective, aspect, fov, n, f);
   MMatrix mvp;
   MMultiply(&mvp, &perspective, &mv);
      // Set the viewport
   glViewport ( 0, 0, aaContext->width, aaContext->height );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   glUniformMatrix4fv(userData->mvpLoc,1, GL_FALSE, mvp.m[0]);
   glUniform1i(userData->texLoc, 0);
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, userData->earth_texture );
   glVertexAttribPointer(userData->texCoordsLoc, 2, GL_FLOAT,
	   GL_FALSE, 2*sizeof(float), userData->sphere->texCoords);
   glEnableVertexAttribArray ( userData->texCoordsLoc);
   glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, 
	   GL_FALSE, 3*sizeof(float), userData->sphere->vertices);
   glEnableVertexAttribArray ( userData->positionLoc);
   glDrawBuffer(GL_FRONT);
   glDrawElements ( GL_TRIANGLES, userData->sphere->numIndices, GL_UNSIGNED_INT, userData->sphere->indices);
   glFlush(); 

   BOOL swap_success = SwapBuffers(aaContext->display);
}

bool clickIntersection(AAContext* aaContext, POINT point, float *intersectionM)
{
	UserData *userData = (UserData*) aaContext->userData;
	float rayDir[4];
	float viewDir[4];
	RECT rect;
	MMatrix P;
	MMatrix iP;

	ScreenToClient(aaContext->hWnd, &point);
	GetClientRect(aaContext->hWnd, &rect);

	viewDir[0] = 2.0f*point.x / (float) (rect.right-rect.left)-1.0f;
	viewDir[1] = 2.0f*(1.0f-point.y / (float )(rect.bottom-rect.top))-1.0f;
	viewDir[2] = 1.0f;
	viewDir[3] = 1.0f;

	MViewRotation(&iP, userData->eye);
	MTranspose(&P, &iP);
	MMap(rayDir, &P, viewDir);
	return IntersectSphere(userData->sphere, rayDir, userData->eye, intersectionM); 
}

void centerIntersection(AAContext *aaContext, float *intersectionM)
{
	MMatrix iP;
	UserData *userData = (UserData*) aaContext->userData;
	float rayDir[4];
	rayDir[0] = userData->sphere->modelT->m[3][0]-userData->eye[0];
	rayDir[1] = userData->sphere->modelT->m[3][1]-userData->eye[1];
	rayDir[2] = userData->sphere->modelT->m[3][2]-userData->eye[2];
	rayDir[3] = 1.0f;

	IntersectSphere(userData->sphere, rayDir, userData->eye, intersectionM);
}

void KeyFunc(AAContext* aaContext, unsigned char wParam, POINT point)
{
	UserData *userData = (UserData*) aaContext->userData;
	if(userData->animation->animating)
		return;
	if(wParam == VK_LBUTTON)  //wParam has lost information in its cast, potentially
	{
		float v2[4];
		float v1[4];
		float intersectionM[4];
		float axis[4];
		float theta;
		if(!clickIntersection(aaContext, point, intersectionM))
			return;
		centerIntersection(aaContext, v1);
		theta = acos(
			MDotProduct(intersectionM, v1)/
			sqrt(MDotProduct(intersectionM, intersectionM))/
			sqrt(MDotProduct(v1,v1)));
		MGramSchmidt(v2, intersectionM, v1);
		MCrossProduct(axis, v2, v1);
		SetAnimation(aaContext, axis, 0.0, theta, true);
	}
}

void Cleanup(AAContext* aaContext)
{
	UserData *userData = (UserData*) aaContext->userData;
	free(userData->sphere->indices);
	free(userData->sphere->vertices);
	free(userData->sphere->texCoords);
	free(userData->sphere->modelT);
	free(userData->sphere->modelR);
	free(userData->sphere->q);
	free(userData->animation);
	glDeleteProgram(userData->programObject);
	glDeleteTextures(1, &userData->earth_texture);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	AAContext aaContext;
	UserData userData;
	aaInitContext(&aaContext);
	aaContext.userData = &userData;
	aaCreateWindow(&aaContext, L"A Rotating Sphere!", 1000, 1000);
	GLenum glewinit = glewInit();
	Init(&aaContext);
	Sphere sphere;
	userData.sphere = &sphere;
	GenSphere(200, 5.0, &sphere);
	userData.eye[0] = 0.0f;
	userData.eye[1] = 0.0f;
	userData.eye[2] = -8.0f;
	aaRegisterKeyFunc(&aaContext, KeyFunc);
	aaRegisterDrawFunc(&aaContext, Draw);
	WinLoop(&aaContext);
	Cleanup(&aaContext);
}