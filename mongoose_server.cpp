// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "mongoose.h"
#include "utils.h"
#include "settings.h"

#include <dirent.h>
#include <errno.h>

#include "include/base/cef_logging.h"

// If port was set to 0, then real port will be known
// only after the webserver was started.

std::string g_mongoose_port = "";
int g_mongoose_port_int = 0;
std::string g_mongoose_ip_address = "";
std::string g_mongoose_url = "";

struct mg_context* g_mongoose_context = 0;
extern std::string g_cgi_env_from_argv;

static int mongoose_error_message(const struct mg_connection* conn,
                                const char *message) {
    // Called when mongoose is about to log an error message.
    // If callback returns non-zero, mongoose does not log anything
    // to a file ("error_log_file" option).
    LOG(ERROR) << message;
    return 0;
}

static void mongoose_end_request(const struct mg_connection* conn,
                                 int reply_status_code) {
    // Called when mongoose has finished processing request.
    mg_request_info* request = mg_get_request_info(
            const_cast<mg_connection*>(conn));
    std::string message;
    message.append(request->request_method);
    message.append(" ");
    std::stringstream ss;
    ss << reply_status_code;
    message.append(ss.str());
    message.append(" ");
    message.append(request->uri);
    if (request->query_string) {
        message.append("?");
        message.append(request->query_string);
    }
    LOG(INFO) << message;
}

bool mongoose_start() {
    LOG(INFO) << "Start Mongoose server";
    json_value* app_settings = Settings();

    // ip_address, port_str
    std::string ip_address((*app_settings)["web_server"]["listen_on"][0]);
    int port_int = (*app_settings)["web_server"]["listen_on"][1];
    if (ip_address.empty()) {
        ip_address = "127.0.0.1";
    }
    g_mongoose_ip_address = ip_address;
    std::stringstream port_ss1;
    port_ss1 << port_int;
    std::string port_str = port_ss1.str();
    if (port_str.empty()) {
        port_str = "0";
    }

    // listening_ports (from ip_address and port_str)
    std::string listening_ports;
    if (ip_address == "*") {
        listening_ports = port_str;
    } else {
        listening_ports = ip_address + ":" + port_str;
    }
    LOG(INFO) << "listening_ports=" << listening_ports;

    // www_directory
    std::string www_directory((*app_settings)["web_server"]["www_directory"]);
    if (www_directory.at(0) != '/') {
        www_directory = GetResourcesDir().append("/").append(www_directory);
    }
    LOG(INFO) << "www_directory=" << www_directory;

    // index_files
    const json_value index_files_array =
            (*app_settings)["web_server"]["index_files"];
    std::string index_files;
    for (int i = 0; i < 32; i++) {
        const char* file = index_files_array[i];
        if (strlen(file)) {
            if (index_files.length()) {
                index_files.append(",");
            }
            index_files.append(file);
        }
    }
    if (index_files.empty()) {
        index_files = "index.html,index.php";
    }
    LOG(INFO) << "index_files=" << index_files;

    // cgi_interpreter
    std::string cgi_interpreter((*app_settings)["web_server"]["cgi_interpreter"]);
    cgi_interpreter = GetFullPath(cgi_interpreter);
    LOG(INFO) << "cgi_interpreter=" << cgi_interpreter;

    // cgi_pattern (from cgi_extensions)
    const json_value cgi_extensions =
            (*app_settings)["web_server"]["cgi_extensions"];
    std::string cgi_pattern;
    for (int i = 0; i < 32; i++) {
        const char* extension = cgi_extensions[i];
        if (strlen(extension)) {
            if (cgi_pattern.length()) {
                cgi_pattern.append("|");
            }
            cgi_pattern.append("**.").append(extension).append("$");
        }
    }
    if (cgi_pattern.empty()) {
        cgi_pattern = "**.php$";
    }
    LOG(INFO) << "cgi_pattern=" << cgi_pattern;

    // cgi_temp_dir - doesn't work
    // std::string cgi_temp_dir((*app_settings)["web_server"]["cgi_temp_dir"]);
    // if (cgi_temp_dir.empty()) {
    //     char const* tmp = getenv("TMP");
    //     if (tmp != NULL) {
    //         cgi_temp_dir.assign(tmp);
    //     } else {
    //         char const* temp = getenv("TEMP");
    //         if (temp != NULL) {
    //             cgi_temp_dir.assign(temp);
    //         } else {
    //             char const* tmpdir = getenv("TMPDIR");
    //             if (tmpdir != NULL) {
    //                 cgi_temp_dir.assign(tmpdir);
    //             } else {
    //                 cgi_temp_dir.assign("/tmp");
    //             }
    //         }
    //     }
    // } else {
    //     cgi_temp_dir = GetFullPath(cgi_temp_dir);
    // }
    // DIR* temp_dir = opendir(cgi_temp_dir.c_str());
    // if (temp_dir) {
    //     closedir(temp_dir);
    //     LOG(INFO) << "cgi_temp_dir=" << cgi_temp_dir;
    // } else if (ENOENT == errno) {
    //     LOG(ERROR) << "Temp directory does not exist: cgi_temp_dir="
    //                << cgi_temp_dir;
    //     cgi_temp_dir = "/tmp";
    // }

    // 404_handler
    std::string _404_handler((*app_settings)["web_server"]["404_handler"]);

    // Hide files patterns.
    const json_value hide_files = (*app_settings)["web_server"]["hide_files"];
    std::string hide_files_patterns = "";
    for (int i = 0; i < 100; i++) {
        const char* pattern = hide_files[i];
        if (strlen(pattern)) {
            if (hide_files_patterns.length()) {
                hide_files_patterns.append("|");
            }
            hide_files_patterns.append("**/").append(pattern).append("$");
        }
    }
    LOG(INFO) << "hide_files_patterns=" << hide_files_patterns;

    // cgi_environment (same as cgi_env)
    std::string cgi_env = "";
    cgi_env.append("TMP=").append("/tmp").append(",");
    cgi_env.append("TEMP=").append("/tmp").append(",");
    cgi_env.append("TMPDIR=").append("/tmp").append(",");
    if (ip_address != "*") {
        cgi_env.append("SERVER_NAME=").append(ip_address)
               .append(",");
        cgi_env.append("SERVER_ADDR=").append(ip_address)
               .append(",");
    }

    // Set DYLD_LIBRARY_PATH, so that php-cgi dylib libraries are loaded properly on Mac.
    // Alternatively you could change name to "@rpath/libssl.3.dylib" using install_name_tool.
    cgi_env.append("DYLD_LIBRARY_PATH=").append(GetExecutableDir()).append(",");

    // Set openssl config, otherwise it uses homebrew config which may be missing.
    // User may uverwrite it using --cgi-environment command line arg passed to app.
    cgi_env.append("OPENSSL_CONF=").append(GetExecutableDir().append("/openssl.cnf")).append(",");

    // PHPRC env variable to help PHP find php.ini configuration file.
    // Ref: https://www.php.net/configuration.file
    // It is found automatically when put next to php-cgi executable, so not needed currently.
    // cgi_env.append("PHPRC=").append(GetExecutableDir().append("/php.ini")).append(",");

    cgi_env.append("PHPDESKTOP_VERSION=").append(PHPDESKTOP_VERSION);
    if (g_cgi_env_from_argv.length()) {
        cgi_env.append(",").append(g_cgi_env_from_argv);
    }
    LOG(INFO) << "cgi_environment=" << cgi_env;

    // Mongoose options
    const char* options[] = {
        "document_root", www_directory.c_str(),
        "listening_ports", listening_ports.c_str(),
        "index_files", index_files.c_str(),
        "cgi_interpreter", cgi_interpreter.c_str(),
        "cgi_pattern", cgi_pattern.c_str(),
        "cgi_environment", cgi_env.c_str(),
        "404_handler", _404_handler.c_str(),
        "hide_files_patterns", hide_files_patterns.c_str(),
        NULL
    };

    // Start mongoose
    mg_callbacks callbacks = {0};
    callbacks.log_message = &mongoose_error_message;
    callbacks.end_request = &mongoose_end_request;
    g_mongoose_context = mg_start(&callbacks, NULL, options);
    if (g_mongoose_context == NULL) {
        LOG(ERROR) << "mg_start() failed";
        return false;
    }

    // When port was set to 0 then a random free port was assigned
    // by OS.
    int port = mg_get_listening_port(g_mongoose_context);
    g_mongoose_port_int = port;
    std::stringstream port_ss2;
    port_ss2 << port;
    g_mongoose_port = port_ss2.str();
    if (ip_address == "*") {
        g_mongoose_url = "http://127.0.0.1:" + port_ss2.str() + "/";
    } else {
        g_mongoose_url = "http://" + ip_address + ":" + port_ss2.str() + "/";
    }
    LOG(INFO) << "Mongoose url: " << g_mongoose_url;

    return true;
}

void mongoose_stop() {
    // Don't call mg_stop(), as there were issues in the past
    // with doing so. It is not necessary to stop mongoose server,
    // as this is done only when application quits and mongoose
    // server will automatically stop when application quits.
}

std::string mongoose_get_port() {
    return g_mongoose_port;
}

int mongoose_get_port_int() {
    return g_mongoose_port_int;
}

std::string mongoose_get_ip_address() {
    return g_mongoose_ip_address;
}

std::string mongoose_get_url() {
    return g_mongoose_url;
}
