#pragma once

#include "httplib.h"

void handle_exam_start(const httplib::Request& req, httplib::Response& res);
void handle_exam_resume(const httplib::Request& req, httplib::Response& res);
void handle_exam_answer(const httplib::Request& req, httplib::Response& res);
void handle_exam_submit(const httplib::Request& req, httplib::Response& res);
