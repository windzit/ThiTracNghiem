#pragma once

#include "httplib.h"

void handle_login(const httplib::Request& req, httplib::Response& res);
void handle_logout(const httplib::Request& req, httplib::Response& res);
