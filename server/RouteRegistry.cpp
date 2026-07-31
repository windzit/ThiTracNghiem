#include "RouteRegistry.h"
#include "handlers/AuthHandler.h"
#include "handlers/ClassHandler.h"
#include "handlers/StudentHandler.h"
#include "handlers/SubjectHandler.h"
#include "handlers/QuestionHandler.h"
#include "handlers/ExamHandler.h"
#include "handlers/ReportHandler.h"
#include "handlers/AdminHandler.h"

void registerRoutes(httplib::Server& svr) {
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "http://localhost:5173"},
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
    svr.Post("/api/login", handle_login);
    svr.Post("/api/logout", handle_logout);

    // Admin / System
    svr.Post("/api/admin/save", handle_admin_save);
    svr.Post("/api/admin/rebuild-used", handle_rebuild_used);
    svr.Get("/api/system/settings", handle_get_system_settings);
    svr.Post("/api/system/settings", handle_post_system_settings);

    // Class
    svr.Get("/api/classes", handle_get_classes);
    svr.Get("/api/classes/:id", handle_get_class_by_id);
    svr.Post("/api/classes", handle_create_class);
    svr.Put("/api/classes/:id", handle_update_class);
    svr.Delete("/api/classes/:id", handle_delete_class);

    // Student
    svr.Get("/api/students", handle_get_students);
    svr.Get("/api/students/:id", handle_get_student_by_id);
    svr.Post("/api/students", handle_create_student);
    svr.Post("/api/students/bulk-delete", handle_bulk_delete_students);
    svr.Put("/api/students/:id", handle_update_student);
    svr.Delete("/api/students/:id", handle_delete_student);

    // Subject
    svr.Get("/api/subjects", handle_get_subjects);
    svr.Get("/api/subjects/:id", handle_get_subject_by_id);
    svr.Post("/api/subjects", handle_create_subject);
    svr.Put("/api/subjects/:id", handle_update_subject);
    svr.Delete("/api/subjects/:id", handle_delete_subject);

    // Question
    svr.Post("/api/questions", handle_create_question);
    svr.Post("/api/questions/bulk-delete", handle_bulk_delete_questions);
    svr.Put("/api/questions/:id", handle_update_question);
    svr.Put("/api/questions/:id/restore", handle_restore_question);
    svr.Post("/api/questions/:id/restore", handle_restore_question);
    svr.Delete("/api/questions/:id", handle_delete_question);

    // Exam
    svr.Post("/api/exams/start", handle_exam_start);
    svr.Get("/api/exams/resume", handle_exam_resume);
    svr.Put("/api/exams/answer", handle_exam_answer);
    svr.Post("/api/exams/submit", handle_exam_submit);

    // Report
    svr.Get("/api/reports/exam", handle_report_exam);
    svr.Get("/api/reports/scoreboard", handle_report_scoreboard);
    svr.Delete("/api/scores", handle_delete_score);
}
