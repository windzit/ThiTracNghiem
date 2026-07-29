#pragma once

#include "httplib.h"

void handle_get_classes(const httplib::Request& req, httplib::Response& res);
void handle_get_class_by_id(const httplib::Request& req, httplib::Response& res);
void handle_create_class(const httplib::Request& req, httplib::Response& res);
void handle_update_class(const httplib::Request& req, httplib::Response& res);
void handle_delete_class(const httplib::Request& req, httplib::Response& res);
