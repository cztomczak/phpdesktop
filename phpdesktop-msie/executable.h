// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// Dependency: debug.h (macro ASSERT_EXIT).

#pragma once

bool GetExecutablePath(wchar_t* outPath, int outPathSize);
bool GetExecutablePathQuoted(wchar_t* outPath, int outPathSize);
bool GetExecutableFilename(wchar_t* outFilename, int outFilenameSize);
bool GetExecutableName(wchar_t* outName, int outNameSize);
bool GetExecutableDirectory(wchar_t* outDirectory, int outDirectorySize);
