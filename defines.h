// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

// #define CEF_USE_SANDBOX 1
// Currently sandbox disabled due to issue. Reported:
// https://magpcss.org/ceforum/viewtopic.php?f=6&t=20037

#define WINVER          _WIN32_WINNT_WIN10
#define _WIN32_WINNT    _WIN32_WINNT_WIN10

#define _AMD64_ 1

// Win messages
#define WM_TRAY_MESSAGE (WM_USER + 1)
