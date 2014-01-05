#ifndef AACONTEXT_H
#define AACONTEXT_H

#include <Windows.h>
#include <GL/glew.h>

///
// Types
//

typedef struct AAContext
{
   void*       userData;

   GLint       width;

   GLint       height;

   LARGE_INTEGER old_elapsed;

   LARGE_INTEGER new_elapsed;

   LARGE_INTEGER tps;

   HWND hWnd;

   HDC display;
      
   HGLRC context;

   /// Callbacks
   void (*drawFunc) ( struct AAContext* );
   void (*keyFunc) ( struct AAContext*, unsigned char, POINT point);
   void (*updateFunc) ( struct AAContext*, float deltaTime );
} AAContext;


///
//  Public Functions
//

//
///
/// \brief Initialize ES framework context.  This must be called before calling any other functions.
/// \param esContext Application context
//
void aaInitContext ( AAContext *paaContext );

//
/// \brief Create a window with the specified parameters
/// \param paaContext Application context
/// \param title Name for title bar of window
/// \param width Width in pixels of window to create
/// \param height Height in pixels of window to create
/// \return GL_TRUE if window creation is succesful, GL_FALSE otherwise
GLboolean aaCreateWindow ( AAContext *aaContext, const wchar_t *title, GLint width, GLint height);

//
/// \brief Start the main loop for the OpenGL ES application
/// \param esContext Application context
//
void aaMainLoop ( AAContext *paaContext );

void aaRegisterDrawFunc ( AAContext *paaContext, void (*drawFunc) ( AAContext* ) );

void aaRegisterUpdateFunc ( AAContext *paaContext, void (*updateFunc) ( AAContext*, float ) );


void aaRegisterKeyFunc ( AAContext *aaContext, 
                                    void (*drawFunc) ( AAContext*, unsigned char, POINT) );

#endif