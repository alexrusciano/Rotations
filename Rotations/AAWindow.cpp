#include <Windows.h>
#include "AAWindow.h"

///
//  ESWindowProc()
//
//      Main window procedure
//
LRESULT WINAPI AAWindowProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
   LRESULT  lRet = 1;
   switch (uMsg) 
   { 
      case WM_CREATE:
         break;

      case WM_PAINT:
         {
            AAContext *aaContext = (AAContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
		    QueryPerformanceFrequency(&aaContext->tps);
			aaContext->old_elapsed = aaContext->new_elapsed;
		    QueryPerformanceCounter(&aaContext->new_elapsed);

            if ( aaContext && aaContext->drawFunc )
               aaContext->drawFunc ( aaContext );
            
            ValidateRect( aaContext->hWnd, NULL );
         }
         break;

      case WM_DESTROY:
         PostQuitMessage(0);             
         break; 
      
      case WM_LBUTTONDOWN:
         {
            POINT      point;
            AAContext *aaContext = (AAContext*)(LONG_PTR) GetWindowLongPtr ( hWnd, GWL_USERDATA );
            
            GetCursorPos( &point );
		    QueryPerformanceFrequency(&aaContext->tps);
			aaContext->old_elapsed = aaContext->new_elapsed;
		    QueryPerformanceCounter(&aaContext->new_elapsed);

            if ( aaContext && aaContext->keyFunc )
	            aaContext->keyFunc ( aaContext, (unsigned char) wParam, 
		                             point );
}
         break;
         
      default: 
         lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
         break; 
   } 

   return lRet; 
}

//////////////////////////////////////////////////////////////////
//
//  Public Functions
//
//

///
//  WinCreate()
//
//      Create Win32 instance and window
// title should be converted to unicode somehow
GLboolean WinCreate ( AAContext *aaContext, const wchar_t *title )
{
   WNDCLASS wndclass = {0}; 
   DWORD    wStyle   = 0;
   RECT     windowRect;
   HINSTANCE hInstance = GetModuleHandle(NULL);


   wndclass.style         = CS_OWNDC;
   wndclass.lpfnWndProc   = (WNDPROC)AAWindowProc; 
   wndclass.hInstance     = hInstance; 
   wndclass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); 
   wndclass.lpszClassName = L"AAWindow"; 

   if (!RegisterClass (&wndclass) ) 
      return FALSE; 

   wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;
   
   // Adjust the window rectangle so that the client area has
   // the correct number of pixels
   windowRect.left = 0;
   windowRect.top = 0;
   windowRect.right = aaContext->width;
   windowRect.bottom = aaContext->height;

   AdjustWindowRect ( &windowRect, wStyle, FALSE );

   aaContext->hWnd = CreateWindow(
						 wndclass.lpszClassName,
                         title,
                         wStyle,
                         0,
                         0,
                         windowRect.right - windowRect.left,
                         windowRect.bottom - windowRect.top,
                         NULL,
                         NULL,
                         hInstance,
                         NULL);

   // Set the WGLContext* to the GWL_USERDATA so that it is available to the 
   // AAWindowProc
   SetWindowLongPtr (  aaContext->hWnd, GWL_USERDATA, (LONG) (LONG_PTR) aaContext );


   if ( aaContext->hWnd == NULL )
      return GL_FALSE;

   ShowWindow ( aaContext->hWnd, TRUE );

   return GL_TRUE;
}

///
//  winLoop()
//
//      Start main windows loop
//
void WinLoop ( AAContext *aaContext )
{
   MSG msg = { 0 };
   int done = 0;
   DWORD lastTime = GetTickCount();
   
   while (!done)
   {
      int gotMsg = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0);
      DWORD curTime = GetTickCount();
      float deltaTime = (float)( curTime - lastTime ) / 1000.0f;
      lastTime = curTime;

      if ( gotMsg )
      {
         if (msg.message==WM_QUIT)
         {
             done=1; 
         }
         else
         {
             TranslateMessage(&msg); 
             DispatchMessage(&msg); 
         }
      }
      else  //if no message was received, just tell the window to keep itself painted  (what if not here?)
         SendMessage( aaContext->hWnd, WM_PAINT, 0, 0 );

      // Call update function if registered
      if ( aaContext->updateFunc != NULL )
         aaContext->updateFunc ( aaContext, deltaTime );
   }
}
