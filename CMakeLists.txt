# Versions
set(PHPDESKTOP_VERSION "127.0")
set(CEF_VERSION "127.3.2+gf1af617+chromium-127.0.6533.100")
cmake_minimum_required(VERSION 3.21)

# Project and general options
project(phpdesktop)
set(DOWNLOADS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/downloads")
set(CMAKE_CONFIGURATION_TYPES Debug Release)
add_definitions(-DPHPDESKTOP_VERSION="${PHPDESKTOP_VERSION}")
set(CMAKE_CXX_FLAGS "-std=c++20 -Wfatal-errors")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG_CONFIGURATION")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# Download CEF and configure
if ("${PROJECT_ARCH}" STREQUAL "arm64")
    set(CEF_PLATFORM "macosarm64")
elseif ("${PROJECT_ARCH}" STREQUAL "x86_64")
    set(CEF_PLATFORM "macosx64")
elseif ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "arm64")
    set(PROJECT_ARCH "arm64")
    set(CEF_PLATFORM "macosarm64")
else()
    set(PROJECT_ARCH "x86_64")
    set(CEF_PLATFORM "macosx64")
endif()
include(DownloadCEF)
DownloadCEF("${CEF_PLATFORM}" "${CEF_VERSION}" "${CMAKE_SOURCE_DIR}/downloads")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
include_directories(${CEF_ROOT})
PRINT_CEF_CONFIG()

# Target phpdesktop
set(PHPDESKTOP_SOURCES
    app.cpp
    client.cpp
    json.c
    main.mm
    mongoose_server.cpp
    mongoose.c
    settings.cpp
    utils.mm
)
set(HELPER_SOURCES
    helper.cpp
)
set(RESOURCES
    icon.icns
    settings.json
    www/_pdo.php
    www/cookies.php
    www/dialogs.php
    www/download.php
    www/env-variables.php
    www/forms.php
    www/google.php
    www/html5-video.php
    www/index.php
    www/loading-error.php
    www/php-error.php
    www/phpinfo.php
    www/popup.php
    www/pretty-urls.php
    www/request-quota.php
    www/session.php
    www/sqlite.php
    www/style.css
    www/upload.php
    www/window-close.php
)
set(PHPDESKTOP_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(PHPDESKTOP_APP "${PHPDESKTOP_OUT_DIR}/PHP Desktop.app")
add_executable(phpdesktop MACOSX_BUNDLE ${PHPDESKTOP_SOURCES})
# SET_EXECUTABLE_TARGET_PROPERTIES(phpdesktop)  # This would overwrite -std=c++20
set_target_properties(phpdesktop PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY "${PHPDESKTOP_OUT_DIR}"
                        RUNTIME_OUTPUT_DIRECTORY "${PHPDESKTOP_OUT_DIR}"
                        LIBRARY_OUTPUT_DIRECTORY "${PHPDESKTOP_OUT_DIR}"
                        OUTPUT_NAME "PHP Desktop"
)
add_dependencies(phpdesktop libcef_dll_wrapper)
target_link_libraries(phpdesktop libcef_dll_wrapper ${CEF_STANDARD_LIBS})
ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
set_target_properties(phpdesktop PROPERTIES
    RESOURCE "${RESOURCES}"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
)

# Copy the CEF framework into the Frameworks directory.
add_custom_command(
    TARGET phpdesktop
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${PHPDESKTOP_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
)

# Create the multiple Helper app bundle targets.
foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
    # Convert to a list and extract the suffix values.
    string(REPLACE ":" ";" _suffix_list ${_suffix_list})
    list(GET _suffix_list 0 _name_suffix)
    list(GET _suffix_list 1 _target_suffix)
    list(GET _suffix_list 2 _plist_suffix)

    # Define Helper target and output names.
    set(_helper_target "phpdesktop_helper${_target_suffix}")
    set(_helper_output_name "PHP Desktop Helper${_name_suffix}")

    # Create Helper-specific variants of the helper-Info.plist file. Do this
    # manually because the configure_file command (which is executed as part of
    # MACOSX_BUNDLE_INFO_PLIST) uses global env variables and would insert the
    # wrong values with multiple targets.
    set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist.in")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/helper-Info.plist.in" _plist_contents)
    string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
    file(WRITE ${_helper_info_plist} ${_plist_contents})

    # Create Helper executable target.
    add_executable(${_helper_target} MACOSX_BUNDLE ${HELPER_SOURCES})
    # SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
    set_target_properties(${_helper_target} PROPERTIES
                            ARCHIVE_OUTPUT_DIRECTORY "${PHPDESKTOP_OUT_DIR}"
                            RUNTIME_OUTPUT_DIRECTORY "${PHPDESKTOP_OUT_DIR}"
                            LIBRARY_OUTPUT_DIRECTORY "${PHPDESKTOP_OUT_DIR}")
    # add_dependencies(${_helper_target} shared_helper)
    # target_link_libraries(${_helper_target} shared_helper)
    add_dependencies(${_helper_target} libcef_dll_wrapper)
    target_link_libraries(${_helper_target} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${_helper_target} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
        OUTPUT_NAME ${_helper_output_name}
    )
    target_link_libraries(${_helper_target} cef_sandbox_lib)

    # Add the Helper as a dependency of the main executable target.
    add_dependencies(phpdesktop "${_helper_target}")

    # Copy the Helper app bundle into the Frameworks directory.
    add_custom_command(
        TARGET phpdesktop
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PHPDESKTOP_OUT_DIR}/${_helper_output_name}.app"
                "${PHPDESKTOP_APP}/Contents/Frameworks/${_helper_output_name}.app"
        VERBATIM
        )
endforeach()

COPY_MAC_RESOURCES("${RESOURCES}" "" "phpdesktop" "${CMAKE_CURRENT_SOURCE_DIR}" "${PHPDESKTOP_APP}")
