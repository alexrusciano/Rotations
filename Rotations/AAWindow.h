#include "AAContext.h"

#ifndef AAWINDOW_H
#define AAWINDOW_H
///
//  WinCreate()
//
//      Create Win32 instance and window
//
GLboolean WinCreate ( AAContext *paaContext, const wchar_t *title );

///
//  WinLoop()
//
//      Start main windows loop
//
void WinLoop ( AAContext *paaContext );
#endif