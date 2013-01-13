// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// Dependency: debug.h (macro ASSERT_EXIT).

#pragma once

void GetExecutablePath(wchar_t* out, int length);
void GetExecutableDir(wchar_t* out, int length);
void GetExecutableFilename(wchar_t* out, int length);
void GetExecutableName(wchar_t* out, int length);

/*
	Example:

	wchar_t path[1024];
	GetExecutablePath(path, _countof(path));
	DEBUGW(path);

	Case: when exe file is deleted but program still runs a failed assertion will be thrown.
*/

void GetExecutablePath(wchar_t* outpath, int outpath_length)
{
	// This path is longer than MAX_PATH, do not use it with
	// windows api functions as you might get buffer overflows.

	wchar_t shortpath[1024];
	wchar_t longpath[1024];

	DWORD dw;
	
	dw = GetModuleFileName(NULL, shortpath, 1024);
	ASSERT_EXIT(dw, "GetModuleFileName()");

	dw = GetLongPathName(shortpath, longpath, 1024);
	ASSERT_EXIT(dw, "GetLongPathName()");

	swprintf_s(outpath, outpath_length, L"%s", longpath);	
}

void GetExecutablePathQuoted(wchar_t* outpath, int outpath_length)
{
	wchar_t* temppath = new wchar_t[outpath_length];
	GetExecutablePath(temppath, outpath_length);
	int result = swprintf_s(outpath, outpath_length, L"\"%s\"", temppath);
	ASSERT_EXIT((-1 != result), "swprintf_s(outpath)");
	delete[] temppath;
	temppath = 0;	
}

void GetExecutableDir(wchar_t* outdir, int outdir_length)
{
	// Without slash at end.

	// This path is longer than MAX_PATH, do not use it with
	// windows api functions as you might get buffer overflows.

	wchar_t longpath[1024];
	GetExecutablePath(longpath, _countof(longpath));

	wchar_t drive[3];
	wchar_t dir[768];
	wchar_t fname[256];
	wchar_t ext[32];

	errno_t result = _wsplitpath_s(longpath, drive, _countof(drive), dir, _countof(dir), fname, _countof(fname), ext, _countof(ext));
	ASSERT_EXIT((result == 0), "_wsplitpath_s(longpath)");

	swprintf_s(outdir, outdir_length, L"%s%s", drive, dir);
	
	// remove slash at end
	int len = wcslen(outdir);
	if (outdir[len-1] == '\\' || outdir[len-1] == '/') {
		outdir[len-1] = 0;
	}
}

void GetExecutableFilename(wchar_t* outdir, int outdir_length)
{
	// Filename with extension.

	wchar_t longpath[1024];
	GetExecutablePath(longpath, _countof(longpath));

	wchar_t drive[3];
	wchar_t dir[768];
	wchar_t fname[256];
	wchar_t ext[32];

	errno_t result = _wsplitpath_s(longpath, drive, _countof(drive), dir, _countof(dir), fname, _countof(fname), ext, _countof(ext));
	ASSERT_EXIT((result == 0), "_wsplitpath_s(longpath)");

	swprintf_s(outdir, outdir_length, L"%s%s", fname, ext);
}

void GetExecutableName(wchar_t* outdir, int outdir_length)
{
	// Filename without extension.

	wchar_t longpath[1024];
	GetExecutablePath(longpath, _countof(longpath));

	wchar_t drive[3];
	wchar_t dir[768];
	wchar_t fname[256];
	wchar_t ext[32];

	errno_t result = _wsplitpath_s(longpath, drive, _countof(drive), dir, _countof(dir), fname, _countof(fname), ext, _countof(ext));
	ASSERT_EXIT((result == 0), "_wsplitpath_s(longpath)");

	swprintf_s(outdir, outdir_length, L"%s", fname);
}
