// Copyright (c) 2024 Marshall A. Greenblatt. All rights reserved.
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

#ifndef CEF_INCLUDE_INTERNAL_CEF_DUMP_WITHOUT_CRASHING_INTERNAL_H_
#define CEF_INCLUDE_INTERNAL_CEF_DUMP_WITHOUT_CRASHING_INTERNAL_H_
#pragma once

#include <stddef.h>

#include "include/internal/cef_export.h"

#ifdef __cplusplus
extern "C" {
#endif

// See include/base/cef_dump_without_crashing.h for intended usage.

///
/// cef_dump_without_crashing allows for capturing crash dumps in a throttled
/// manner. This function should only be called after CefInitialize has been
/// successfully called. For detailed behavior, usage instructions, and
/// considerations, refer to the documentation of DumpWithoutCrashing in
/// base/debug/dump_without_crashing.h.
///
CEF_EXPORT int cef_dump_without_crashing(long long mseconds_between_dumps,
                                         const char* function_name,
                                         const char* file_name,
                                         int line_number);
///
/// cef_dump_without_crashing_unthrottled allows for capturing crash dumps
/// without any throttling constraints. This function should also only be called
/// after CefInitialize has been successfully called. For detailed behavior,
/// usage instructions, and considerations, refer to the documentation of
/// DumpWithoutCrashingUnthrottled in base/debug/dump_without_crashing.h.
///
CEF_EXPORT int cef_dump_without_crashing_unthrottled();

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // CEF_INCLUDE_INTERNAL_CEF_DUMP_WITHOUT_CRASHING_INTERNAL_H_
