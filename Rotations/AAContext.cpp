#include "AAContext.h"
#include "AAWindow.h"

///
// CreateGLContext()
//
//    Creates an EGL rendering context and all associated elements
//
GLboolean CreateGLContext ( HWND hWnd, HDC* pDisplay,
                              HGLRC* pContext)
{
   int pixelFormat;
   HDC hdc;
   HGLRC context;
   PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                        //Number of bits for the depthbuffer
        8,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

// Here we start setting up the GL context.
   hdc = GetDC(hWnd);
   if ( hdc == NULL )
   {
      return FALSE;
   }

   pixelFormat = ChoosePixelFormat(hdc, &pfd);
   if(pixelFormat == 0)
   {
      return FALSE;
   }

   if(!SetPixelFormat(hdc, pixelFormat, &pfd))
   {
      return FALSE;
   }

   context = wglCreateContext(hdc);
   if(context == NULL)
   {
      return FALSE;
   }
 
   // Make the context current
   if ( !wglMakeCurrent(hdc, context) )
   {
      return FALSE;
   }
   *pDisplay = hdc;
   *pContext = context;
   return GL_TRUE;
} 

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  esInitContext()
//
//      Initialize ES utility context.  This must be called before calling any other
//      functions.
//
void aaInitContext ( AAContext *aaContext )
{
   if ( aaContext != NULL )
   {
      memset( aaContext, 0, sizeof( AAContext) );
   }
}

///
//  aaCreateWindow()
//
//      title - name for title bar of window
//      width - width of window to create
//      height - height of window to create
//
GLboolean aaCreateWindow ( AAContext *paaContext, const wchar_t* title, GLint width, GLint height)
{
   
   if ( paaContext == NULL )
   {
      return GL_FALSE;
   }

   paaContext->width = width;
   paaContext->height = height;

   if ( !WinCreate ( paaContext, title) )
   {
      return GL_FALSE;
   }

  
   if ( !CreateGLContext ( paaContext->hWnd,
                            &paaContext->display,
                            &paaContext->context))
   {
      return GL_FALSE;
   }
   

   return GL_TRUE;
}

///
//  aaMainLoop()
//
//    Start the main loop for the application
//
void aaMainLoop ( AAContext *paaContext )
{
   WinLoop ( paaContext );
}


///
//  aaRegisterDrawFunc()
//
void aaRegisterDrawFunc ( AAContext *paaContext, void (*drawFunc) (AAContext* ) )
{
   paaContext->drawFunc = drawFunc;
}


///
//  esRegisterUpdateFunc()
//
void aaRegisterUpdateFunc ( AAContext *paaContext, void (*updateFunc) ( AAContext*, float ) )
{
   paaContext->updateFunc = updateFunc;
}


///
//  esRegisterKeyFunc()
//
void aaRegisterKeyFunc ( AAContext *paaContext,
                                    void (*keyFunc) (AAContext*, unsigned char, POINT point) )
{
   paaContext->keyFunc = keyFunc;
}