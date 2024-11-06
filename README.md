# Chrome 130 branch

## Build on Windows

1. Install Visual Studio 2022 Community Edition.
1. During install install SDK 10.0.22621 (default at the time of this writing).
1. Download phpdesktop sources by cloning repository.
1. Install latest CMake from http://www.cmake.org/ .
1. Download CEF 130.1.15+g34b05bb Standard Distribution from https://cef-builds.spotifycdn.com/index.html .
1. Build CEF binary according to instructions from CMakeLists.txt file, but add the `-DUSE_SANDBOX=Off` flag to cmake command.
1. Open cef.sln file, change to Release configuration and build.
1. Go to tests/cefclient/Release/ and run cefclient sample application that was built to see if it runs fine.
1. Create "phpdesktop/Release/" directory and copy files and subdirectories from the "cef_binary/Release/" and "cef_binary/Resources/" directories. Exclude ".lib" files.
1. Copy "cef_binary/build/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib" and "cef_binary/Release/libcef.lib" to the "phpdesktop/lib/Release/" directory.
1. Open "phpdesktop/phpdesktop-chrome.sln" file and build solution.

