#pragma once

#include "httplib.h"

void handle_admin_save(const httplib::Request& req, httplib::Response& res);
void handle_rebuild_used(const httplib::Request& req, httplib::Response& res);
void handle_get_system_settings(const httplib::Request& req, httplib::Response& res);
void handle_post_system_settings(const httplib::Request& req, httplib::Response& res);
