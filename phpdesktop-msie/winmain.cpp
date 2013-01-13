// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "DEBUG.h"
#include "resource.h"

#include "MainView.h"
#include "MainFrame.h"

#include "Shellapi.h"
#include "srand2.h"
#include "WebBrowser/InternetFeatures.h"

#include "ExecutablePath.h"

CAppModule _Module;

SHELLEXECUTEINFO g_phpshell;
wchar_t g_addressport[] = L"127.0.0.1:54007";
wchar_t g_httpaddressport[] = L"http://127.0.0.1:54007";

void TerminatePHPShell()
{
	TerminateProcess(g_phpshell.hProcess, 0);
}

wchar_t* GetHttpAddressPort()
{
	return g_httpaddressport;
}

void StartPHPServer()
{
	wchar_t shellparams[2048]; // shellparams must be calculated before mainFrame.CreateEx() is called.
  wchar_t wwwDir[1024];
  GetExecutableDir(wwwDir, 1024);
  swprintf_s(wwwDir, 1024, L"%s\\www", wwwDir);
	swprintf_s(shellparams, 2048, L"-S %s -t %s", g_addressport, wwwDir);

	// Run php 5.4 built-in webserver
	// Server must be ready before creating window.
	
	/* This is async - bad
	ShellExecute(mainFrame.m_hWnd, L"Open", 
		L"D:/phpdesktop/bin/php.exe",
		L"-S 127.0.0.1:54007 -t D:/phpdesktop/scripts",
		NULL, SW_HIDE);
	*/
	// We must use ShellExecuteEx() to execute command synchronously.
	
	g_phpshell.cbSize = sizeof(SHELLEXECUTEINFO);
	// SEE_MASK_NOCLOSEPROCESS - so phpshell.hProcess is set.
	g_phpshell.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS; 
	g_phpshell.hwnd = NULL;
	g_phpshell.lpVerb = L"open";

  wchar_t executableDir[1024];
  wchar_t phpExecutable[1024];
  GetExecutableDir(executableDir, 1024);
  swprintf_s(phpExecutable, 1024, L"%s\\php\\php.exe", executableDir);  

	g_phpshell.lpFile = phpExecutable;
	g_phpshell.lpParameters = shellparams;
	g_phpshell.lpDirectory = NULL;
	g_phpshell.nShow = SW_HIDE;
	g_phpshell.hInstApp = NULL;
	BOOL bshell = ShellExecuteEx(&g_phpshell);
	ASSERT_EXIT(bshell, "ShellExecuteEx(php-builtin-webserver) failed");

	// TODO: tutaj sprawdzic czy juz ten proces php nasluchuje na porcie,
	// spauzowaæ tutaj uruchamianie dalej okna dopoki nie zacznie nasluchiwaæ.
	// To jest przypadek gdy firewall zablokuje polaczenie nasluchiwania,
	// proces php jest juz uruchomiony, ale nasluchiwanie jeszcze nie dziala,
	// jesli teraz okno zostanie otworzone i nawigowane to wyswietli sie
	// blad IE ze nie znaleziono strony.
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	//wchar_t test[10] = L"Abc";
	//DEBUG_INT(wcslen(test));
	//exit(-1);

	SetInternetFeatures();
	StartPHPServer();	
	
	RECT rc = {0, 0, 1024, 768};
	MainFrame mainFrame;
	if(mainFrame.CreateEx(NULL, rc) == NULL) {
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
		
	// Disable maximize box.
	//mainFrame.SetWindowLongW(GWL_STYLE, mainFrame.GetWindowLongW(GWL_STYLE) & ~WS_MAXIMIZEBOX);

	// Center window.
	mainFrame.CenterWindow();

	// Window title.
	mainFrame.SetWindowTextW(L"ASD");

	mainFrame.ShowWindow(nCmdShow);		

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
