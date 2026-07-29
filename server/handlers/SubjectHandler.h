#pragma once

#include "httplib.h"

void handle_get_subjects(const httplib::Request& req, httplib::Response& res);
void handle_get_subject_by_id(const httplib::Request& req, httplib::Response& res);
void handle_create_subject(const httplib::Request& req, httplib::Response& res);
void handle_update_subject(const httplib::Request& req, httplib::Response& res);
void handle_delete_subject(const httplib::Request& req, httplib::Response& res);
