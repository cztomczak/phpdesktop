// Copyright (c) 2024 Marshall A. Greenblatt. Portions copyright (c) 2012
// Google Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

///
/// \file
/// Provides functions for generating crash dumps.
///
/// NOTE: The contents of this file are only available to applications that link
/// against the libcef_dll_wrapper target.
///
/// NOTE: Ensure crash reporting is configured before use. See
/// https://bitbucket.org/chromiumembedded/cef/wiki/CrashReporting.md for more
/// information
///
/// WARNING: Crash reporting should not be used in the main/browser process
/// before calling CefInitialize or in sub-processes before CefExecuteProcess.
///

#ifndef CEF_INCLUDE_BASE_CEF_DUMP_WITHOUT_CRASHING_H_
#define CEF_INCLUDE_BASE_CEF_DUMP_WITHOUT_CRASHING_H_
#pragma once

constexpr long long kOneDayInMilliseconds = 86400000;

///
/// CefDumpWithoutCrashing allows for generating crash dumps with a throttling
/// mechanism, preventing frequent dumps from being generated in a short period
/// of time from the same location. The |function_name|, |file_name|, and
/// |line_number| determine the location of the dump. The
/// |mseconds_between_dumps| is an interval between consecutive dumps in
/// milliseconds from the same location.
///
/// Returns true if the dump was successfully generated, false otherwise.
///
bool CefDumpWithoutCrashing(
    long long mseconds_between_dumps = kOneDayInMilliseconds,
    const char* function_name = __builtin_FUNCTION(),
    const char* file_name = __builtin_FILE(),
    int line_number = __builtin_LINE());

///
/// CefDumpWithoutCrashingUnthrottled allows for immediate crash dumping without
/// any throttling constraints.
///
/// Returns true if the dump was successfully generated, false otherwise.
///
bool CefDumpWithoutCrashingUnthrottled();

#endif  // CEF_INCLUDE_BASE_CEF_DUMP_WITHOUT_CRASHING_H_
