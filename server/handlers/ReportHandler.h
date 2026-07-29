#pragma once

#include "httplib.h"

void handle_report_exam(const httplib::Request& req, httplib::Response& res);
void handle_report_scoreboard(const httplib::Request& req, httplib::Response& res);
void handle_delete_score(const httplib::Request& req, httplib::Response& res);
