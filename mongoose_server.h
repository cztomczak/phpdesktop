// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include <string>

bool mongoose_start();
void mongoose_stop();
std::string mongoose_get_port();
int mongoose_get_port_int();
std::string mongoose_get_ip_address();
std::string mongoose_get_url();
