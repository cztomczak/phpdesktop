// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// Dependency: debug.h (macro ASSERT_EXIT).

#pragma once

bool GetExecutablePath(wchar_t* outpath, int outpath_length);
bool GetExecutablePathQuoted(wchar_t* outpath, int outpath_length);
bool GetExecutableFilename(wchar_t* outdir, int outdir_length);
bool GetExecutableName(wchar_t* outdir, int outdir_length);
bool GetExecutableDir(wchar_t* outdir, int outdir_length);
