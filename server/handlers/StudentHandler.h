#pragma once

#include "httplib.h"

void handle_get_students(const httplib::Request& req, httplib::Response& res);
void handle_get_student_by_id(const httplib::Request& req, httplib::Response& res);
void handle_create_student(const httplib::Request& req, httplib::Response& res);
void handle_update_student(const httplib::Request& req, httplib::Response& res);
void handle_delete_student(const httplib::Request& req, httplib::Response& res);
void handle_bulk_delete_students(const httplib::Request& req, httplib::Response& res);
