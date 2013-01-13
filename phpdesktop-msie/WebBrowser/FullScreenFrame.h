//
// FullScreenFrame.h
// Copyright (c) Czarek Tomczak. All rights reserved.
//

#pragma once

/*
	Setting full screen immediately by hiding taskbar and menubar.

	There is one caveat with this solution, if your application crashes
	or is killed through task manager, then user losses taskbar on his
	system permanently! (unless he runs your application again, goes
	into fullscreen and exits, then he will see the taskbar again).

	The simple way is to call:
	SetWindowPos(window, HWND_TOP, 0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
	But it doesn't always work, for example when you do it in WM_CREATE event.

	Author: Czarek Tomczak <czarek.tomczak@@gmail.com>.
*/

template <class T, bool t_bHasSip = true>
class FullScreenFrame
{
public:
	bool isfullscreen;
	LONG windowstyles;
	WINDOWPLACEMENT windowplacement;

	FullScreenFrame()
		:
		isfullscreen(false),
		windowstyles(0)
	{
		ZeroMemory(&windowplacement, sizeof WINDOWPLACEMENT);
	}

	/*
		If you call this function you have to call ShowTaskBar(true) in your WM_CLOSE message,
		otherwise after closing application user will have no access to the taskbar.
	*/
	void SetFullScreen(bool setfullscreen)
	{
		ShowTaskBar(!setfullscreen);

		T* self = static_cast<T*>(this);
		ASSERT_EXIT(self->IsWindow(), "self->IsWindow()");

		if (setfullscreen) {
			if (!isfullscreen) {
				windowstyles = self->GetWindowLongW(GWL_STYLE);
				self->GetWindowPlacement(&windowplacement);
			}

		}

		// SM_CXSCREEN gives primary monitor, for multiple monitors use SM_CXVIRTUALSCREEN.
		RECT fullrect = { 0 };
		SetRect(&fullrect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

		WINDOWPLACEMENT newplacement = windowplacement;
		newplacement.showCmd = SW_SHOWNORMAL;
		newplacement.rcNormalPosition = fullrect;

		if (setfullscreen) {
			self->SetWindowPlacement(&newplacement);
			self->SetWindowLongW(GWL_STYLE,  WS_VISIBLE);
			self->UpdateWindow();
		} else {
			if (isfullscreen) {
				self->SetWindowPlacement(&windowplacement);
				self->SetWindowLongW(GWL_STYLE, windowstyles);
				self->UpdateWindow();
			}
		}

		isfullscreen = setfullscreen;
	}

	void ShowTaskBar(bool show)
	{
		HWND taskbar = FindWindow(_T("Shell_TrayWnd"), NULL);
		HWND start = FindWindow(_T("Button"), NULL);

		if (taskbar != NULL) {
			ShowWindow(taskbar, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(taskbar);
		}
		if (start != NULL) {
			// Vista
			ShowWindow(start, show ? SW_SHOW : SW_HIDE);
			UpdateWindow(start);
		}
	}
};
