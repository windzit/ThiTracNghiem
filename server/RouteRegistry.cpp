#include "RouteRegistry.h"
#include "ServerContext.h"
#include <iostream>
#include <exception>

#include "handlers/AuthHandler.h"
#include "handlers/ClassHandler.h"
#include "handlers/StudentHandler.h"
#include "handlers/SubjectHandler.h"
#include "handlers/QuestionHandler.h"
#include "handlers/ExamHandler.h"
#include "handlers/ReportHandler.h"
#include "handlers/AdminHandler.h"

using HandlerFunc = void(*)(const httplib::Request&, httplib::Response&);

static httplib::Server::Handler wrap_safe(HandlerFunc handler) {
    return [handler](const httplib::Request& req, httplib::Response& res) {
        try {
            handler(req, res);
        } catch (const std::exception& e) {
            std::cerr << "[SERVER EXCEPTION CATCH] " << req.method << " " << req.path << " -> " << e.what() << std::endl;
            error_response(res, std::string("Internal Server Error: ") + e.what(), 500);
        } catch (...) {
            std::cerr << "[SERVER EXCEPTION CATCH] " << req.method << " " << req.path << " -> Unknown exception" << std::endl;
            error_response(res, "Internal Server Error: Unknown exception", 500);
        }
    };
}

void registerRoutes(httplib::Server& svr) {
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    svr.Options("/(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Welcome to ThiTracNghiem API", "text/plain");
    });

    // Auth
    svr.Post("/api/login", wrap_safe(handle_login));
    svr.Post("/api/logout", wrap_safe(handle_logout));

    // Admin / System
    svr.Get("/api/system/settings", wrap_safe(handle_get_system_settings));
    svr.Post("/api/system/settings", wrap_safe(handle_post_system_settings));

    // Class
    svr.Get("/api/classes", wrap_safe(handle_get_classes));
    svr.Get("/api/classes/:id", wrap_safe(handle_get_class_by_id));
    svr.Post("/api/classes", wrap_safe(handle_create_class));
    svr.Put("/api/classes/:id", wrap_safe(handle_update_class));
    svr.Delete("/api/classes/:id", wrap_safe(handle_delete_class));

    // Student
    svr.Get("/api/students", wrap_safe(handle_get_students));
    svr.Get("/api/students/:id", wrap_safe(handle_get_student_by_id));
    svr.Post("/api/students", wrap_safe(handle_create_student));
    svr.Post("/api/students/bulk-delete", wrap_safe(handle_bulk_delete_students));
    svr.Put("/api/students/:id", wrap_safe(handle_update_student));
    svr.Delete("/api/students/:id", wrap_safe(handle_delete_student));

    // Subject
    svr.Get("/api/subjects", wrap_safe(handle_get_subjects));
    svr.Get("/api/subjects/:id", wrap_safe(handle_get_subject_by_id));
    svr.Post("/api/subjects", wrap_safe(handle_create_subject));
    svr.Put("/api/subjects/:id", wrap_safe(handle_update_subject));
    svr.Delete("/api/subjects/:id", wrap_safe(handle_delete_subject));

    // Question
    svr.Post("/api/questions", wrap_safe(handle_create_question));
    svr.Post("/api/questions/bulk-delete", wrap_safe(handle_bulk_delete_questions));
    svr.Put("/api/questions/:id", wrap_safe(handle_update_question));
    svr.Put("/api/questions/:id/restore", wrap_safe(handle_restore_question));
    svr.Delete("/api/questions/:id", wrap_safe(handle_delete_question));

    // Exam
    svr.Post("/api/exams/start", wrap_safe(handle_exam_start));
    svr.Get("/api/exams/resume", wrap_safe(handle_exam_resume));
    svr.Put("/api/exams/answer", wrap_safe(handle_exam_answer));
    svr.Post("/api/exams/submit", wrap_safe(handle_exam_submit));

    // Report
    svr.Get("/api/reports/exam", wrap_safe(handle_report_exam));
    svr.Get("/api/reports/scoreboard", wrap_safe(handle_report_scoreboard));
    svr.Delete("/api/scores", wrap_safe(handle_delete_score));
}

