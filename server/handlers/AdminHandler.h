#pragma once

#include "httplib.h"

void handle_get_system_settings(const httplib::Request& req, httplib::Response& res);
void handle_post_system_settings(const httplib::Request& req, httplib::Response& res);
