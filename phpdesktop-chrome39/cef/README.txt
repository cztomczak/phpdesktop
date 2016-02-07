Chromium Embedded Framework (CEF) Standard Binary Distribution for Windows
-------------------------------------------------------------------------------

Date:             November 03, 2014

CEF Version:      3.2171.1902
CEF URL:          https://chromiumembedded@bitbucket.org/chromiumembedded/branches-2171-cef3.git
                  @1902

Chromium Verison: 39.0.2171.36
Chromium URL:     https://chromium.googlesource.com/chromium/src.git
                  @438546a6d26d33f457aa7cbbbfcd2f762c4a1611

This distribution contains all components necessary to build and distribute an
application using CEF on the Windows platform. Please see the LICENSING
section of this document for licensing terms and conditions.


CONTENTS
--------

cefclient   Contains the cefclient sample application configured to build
            using the files in this distribution. This application demonstrates
            a wide range of CEF functionalities.

cefsimple   Contains the cefsimple sample application configured to build
            using the files in this distribution. This application demonstrates
            the minimal functionality required to create a browser window.

Debug       Contains libcef.dll, libcef.lib and other components required to
            build and run the debug version of CEF-based applications. By
            default these files should be placed in the same directory as the
            executable and will be copied there as part of the build process.

include     Contains all required CEF header files.

libcef_dll  Contains the source code for the libcef_dll_wrapper static library
            that all applications using the CEF C++ API must link against.

Release     Contains libcef.dll, libcef.lib and other components required to
            build and run the release version of CEF-based applications. By
            default these files should be placed in the same directory as the
            executable and will be copied there as part of the build process.

Resources   Contains resources required by libcef.dll. By default these files
            should be placed in the same directory as libcef.dll. By default
            these files should be placed in the same directory as libcef.dll
            and will be copied there as part of the build process.


USAGE
-----

Building using CMake:
  CMake can be used to generate project files in many different formats. See
  usage instructions at the top of the CMakeLists.txt file.

Building using the pre-existing Visual Studio solution (DEPRECATED):
  Visual Studio 2013 and newer:
    Open the cefclient2010.sln solution in Visual Studio and build.

  Visual Studio 2008:
    Open the cefclient2008.sln solution in Visual Studio and build.

  Visual Studio 2005:
    1. Open the cefclient.vcproj and libcef_dll_wrapper.vcproj files in a text
       editor. Change Version="9.00" to Version="8.00".
    2. Open the cefclient2005.sln file in a text editor. Change "Version 9.00"
       to "Version 8.00".
    3. Open the cefclient2005.sln solution in Visual Studio and build.

Please visit the CEF Website for additional usage information.

http://code.google.com/p/chromiumembedded


REDISTRIBUTION
--------------

This binary distribution contains the below components. Components listed under
the "required" section must be redistributed with all applications using CEF.
Components listed under the "optional" section may be excluded if the related
features will not be used.

Required components:

* CEF core library
    libcef.dll

* Unicode support
    icudtl.dat

Optional components:

* Localized resources
    locales/
  Note: Contains localized strings for WebKit UI controls. A .pak file is loaded
  from this folder based on the CefSettings.locale value. Only configured
  locales need to be distributed. If no locale is configured the default locale
  of "en-US" will be used. Locale file loading can be disabled completely using
  CefSettings.pack_loading_disabled. The locales folder path can be customized
  using CefSettings.locales_dir_path.

* Other resources
    cef.pak
    cef_100_percent.pak
    cef_200_percent.pak
    devtools_resources.pak
  Note: Contains WebKit image and inspector resources. Pack file loading can be
  disabled completely using CefSettings.pack_loading_disabled. The resources
  directory path can be customized using CefSettings.resources_dir_path.

* FFmpeg audio and video support
    ffmpegsumo.dll
  Note: Without this component HTML5 audio and video will not function.

* PDF support
    pdf.dll
  Note: Without this component printing will not function.

* Angle and Direct3D support
    d3dcompiler_43.dll (required for Windows XP)
    d3dcompiler_46.dll (required for Windows Vista and newer)
    libEGL.dll
    libGLESv2.dll
  Note: Without these components HTML5 accelerated content like 2D canvas, 3D
  CSS and WebGL will not function.

* Windows Vista 64-bit sandbox support (32-bit distributions only)
    wow_helper.exe
  Note: Without this component the 32-bit build of CEF will not run on 64-bit
  Vista machines with the sandbox enabled.


LICENSING
---------

The CEF project is BSD licensed. Please read the LICENSE.txt file included with
this binary distribution for licensing terms and conditions. Other software
included in this distribution is provided under other licenses. Please visit
"about:credits" in a CEF-based application for complete Chromium and third-party
licensing information.
