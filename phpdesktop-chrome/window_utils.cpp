// Copyright (c) 2012-2014 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include "log.h"

void CenterWindow(HWND wnd) {
   RECT r,r1;
   GetWindowRect(wnd,&r);
   GetWindowRect(GetDesktopWindow(),&r1);
   MoveWindow(wnd,((r1.right-r1.left)-(r.right-r.left))/2,
      ((r1.bottom-r1.top)-(r.bottom-r.top))/2,
      (r.right-r.left), (r.bottom-r.top),0);
}
