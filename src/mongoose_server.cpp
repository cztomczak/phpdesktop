// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "mongoose.h"
#include "utils.h"
#include "version.h"
#include "include/base/cef_logging.h"

std::string g_mongoose_port = "0";
int g_mongoose_port_int = 0;
std::string g_mongoose_ip_address = "127.0.0.1"; // @TODO from settings.json
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

    // Temp directory
    std::string cgi_temp_dir = "/tmp";
    // @TODO load cgi_temp_dir from settings.json
    char const* tmp = getenv("TMP");
    if (tmp != NULL) {
        cgi_temp_dir.assign(tmp);
    } else {
        char const* temp = getenv("TEMP");
        if (temp != NULL) {
            cgi_temp_dir.assign(temp);
        } else {
            char const* tmpdir = getenv("TMPDIR");
            if (tmpdir != NULL) {
                cgi_temp_dir.assign(tmpdir);
            }
        }
    }

    // CGI environment variables
    std::string cgi_env = "";
    cgi_env.append("TMP=").append(cgi_temp_dir).append(",");
    cgi_env.append("TEMP=").append(cgi_temp_dir).append(",");
    cgi_env.append("TMPDIR=").append(cgi_temp_dir).append(",");
    if (g_mongoose_ip_address != "*") {
        cgi_env.append("SERVER_NAME=").append(g_mongoose_ip_address)
               .append(",");
        cgi_env.append("SERVER_ADDR=").append(g_mongoose_ip_address)
               .append(",");
    }
    cgi_env.append("PHPDESKTOP_VERSION=").append(PHPDESKTOP_VERSION);
    if (g_cgi_env_from_argv.length()) {
        cgi_env.append(",").append(g_cgi_env_from_argv);
    }
    LOG(INFO) << "CGI environment variables set: " << cgi_env;

    // Document root
    std::string document_root = get_executable_dir().append("/www");
    LOG(INFO) << "document_root=" << document_root;
    
    // Listening ports
    // @TODO from settings.json
    std::string listening_ports;
    if (g_mongoose_ip_address == "*") {
        listening_ports = g_mongoose_port;
    } else {
        listening_ports = g_mongoose_ip_address + ":" + g_mongoose_port;
    }

    // CGI interpreter
    std::string cgi_interpreter = get_executable_dir().append("/php-cgi");

    // Mongoose options
    const char* options[] = {
        "document_root", document_root.c_str(),
        "listening_ports", listening_ports.c_str(),
        "index_files", "index.html,index.php", // @TODO from settings.json
        "cgi_interpreter", cgi_interpreter.c_str(),
        "cgi_pattern", "**.php$", // @TODO from settings.json
        "cgi_environment", cgi_env.c_str(),
        "404_handler", "/pretty-urls.php", // @TODO from settings.json
        "hide_files_patterns", "", // @TODO from settings.json
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
    std::stringstream port_ss;
    port_ss << port;
    g_mongoose_port = port_ss.str();
    if (g_mongoose_ip_address == "*") {
        g_mongoose_url = "http://127.0.0.1:" + port_ss.str() + "/";
    } else {
        g_mongoose_ip_address = g_mongoose_ip_address;
        g_mongoose_url = "http://" + g_mongoose_ip_address + ":"
                         + g_mongoose_port + "/";
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
