#include "AuthHandler.h"
#include "../ServerContext.h"
#include "Auth.h"
#include "Exam.h"

using namespace std;

void handle_login(const httplib::Request& req, httplib::Response& res) {
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string username = body.value("username", "");
    string password = body.value("password", "");
    string role = body.value("role", "");

    if (username.empty() || password.empty()) {
        error_response(res, "Username and password required", 400); return;
    }

    if (role == "teacher") {
        if (login_teacher(username, password)) {
            json_response(res, {{"role","teacher"},{"username",username}});
        } else { error_response(res, "Invalid credentials", 401); }
    } else if (role == "student") {
        SinhVien* sv = findStudentGlobal(username, nullptr);
        if (sv && sv->passsword == password) {
            json_response(res, {
                {"role","student"},
                {"masv",sv->MASV},
                {"ho",sv->HO},
                {"ten",sv->TEN},
                {"phai",sv->PHAI}
            });
        } else { error_response(res, "Invalid credentials", 401); }
    } else { error_response(res, "Role must be teacher or student", 400); }
}

void handle_logout(const httplib::Request& req, httplib::Response& res) {
    json body;
    try { body = json::parse(req.body); }
    catch (...) { body = json::object(); }

    string userId = body.value("userId", req.get_param_value("userId"));
    string role = body.value("role", req.get_param_value("role"));

    if (role == "student" || role == "STUDENT") {
        ExamSession activeExam;
        if (loadExamSession(userId, activeExam) && activeExam.in_progress && (calculateRemainingSeconds(activeExam) > 0)) {
            custom_json_response(res, {
                {"success", false},
                {"error", "Forbidden"},
                {"message", "Khong the dang xuat khi dang lam bai thi"}
            }, 403);
            return;
        }
    }

    json_response(res, {{"success", true}});
}
