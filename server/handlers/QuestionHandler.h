#pragma once

#include "httplib.h"

void handle_create_question(const httplib::Request& req, httplib::Response& res);
void handle_update_question(const httplib::Request& req, httplib::Response& res);
void handle_delete_question(const httplib::Request& req, httplib::Response& res);
void handle_bulk_delete_questions(const httplib::Request& req, httplib::Response& res);
void handle_restore_question(const httplib::Request& req, httplib::Response& res);
