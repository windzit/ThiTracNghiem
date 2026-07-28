#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <shared_mutex>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "Class.h"
#include "Subject.h"
#include "Student.h"
#include "Question.h"
#include "Score.h"
#include "Auth.h"
#include "Report.h"
#include "Storage.h"
#include "StorageManager.h"
#include "StorageValidator.h"
#include "StringNormalizer.h"
#include "Exam.h"
#include "Utils.h"
#include "DArray.h"
#include "Utility/Swap.h"
#include "Utility/Random.h"

using json = nlohmann::json;
using namespace std;

static Class dsl;
static Subject dsmh;

// Global Read/Write lock:
//   shared_lock  → GET (concurrent reads allowed)
//   unique_lock  → POST/PUT/DELETE (exclusive write, blocks all readers)
static std::shared_mutex g_dbMutex;

// Convenience lock macros
#define DB_READ_LOCK  std::shared_lock<std::shared_mutex> _rl(g_dbMutex)
#define DB_WRITE_LOCK std::unique_lock<std::shared_mutex> _wl(g_dbMutex)

static void json_response(httplib::Response& res, const json& data, int status = 200) {
    json body = {{"success", true}, {"data", data}};
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

static void error_response(httplib::Response& res, const string& msg, int status) {
    json body = {{"success", false}, {"message", msg}};
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

static void custom_json_response(httplib::Response& res, const json& body, int status = 200) {
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

static void set_cors_headers(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "http://localhost:5173");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

// Helper: get path parameter by name from named route
// cpp-httplib 0.50.1 supports named routes like /api/classes/:id
static void handle_delete_score(const httplib::Request& req, httplib::Response& res);
static void handle_rebuild_used(const httplib::Request& req, httplib::Response& res);

// Global student lookup forward declaration
static SinhVien* findStudentGlobal(const string& masv, Lop** outLop);

static std::string get_path_param(const httplib::Request& req, const std::string& name) {
    auto it = req.path_params.find(name);
    if (it != req.path_params.end()) {
        return it->second;
    }
    return "";
}

// Forward declarations
static void handle_login(const httplib::Request& req, httplib::Response& res);
static void handle_logout(const httplib::Request& req, httplib::Response& res);
static void handle_get_classes(const httplib::Request& req, httplib::Response& res);
static void handle_get_class_by_id(const httplib::Request& req, httplib::Response& res);
static void handle_create_class(const httplib::Request& req, httplib::Response& res);
static void handle_update_class(const httplib::Request& req, httplib::Response& res);
static void handle_delete_class(const httplib::Request& req, httplib::Response& res);
static void handle_get_students(const httplib::Request& req, httplib::Response& res);
static void handle_get_student_by_id(const httplib::Request& req, httplib::Response& res);
static void handle_create_student(const httplib::Request& req, httplib::Response& res);
static void handle_update_student(const httplib::Request& req, httplib::Response& res);
static void handle_delete_student(const httplib::Request& req, httplib::Response& res);
static void handle_get_subjects(const httplib::Request& req, httplib::Response& res);
static void handle_get_subject_by_id(const httplib::Request& req, httplib::Response& res);
static void handle_create_subject(const httplib::Request& req, httplib::Response& res);
static void handle_update_subject(const httplib::Request& req, httplib::Response& res);
static void handle_delete_subject(const httplib::Request& req, httplib::Response& res);
static void handle_create_question(const httplib::Request& req, httplib::Response& res);
static void handle_update_question(const httplib::Request& req, httplib::Response& res);
static void handle_delete_question(const httplib::Request& req, httplib::Response& res);
static void handle_bulk_delete_questions(const httplib::Request& req, httplib::Response& res);
static void handle_bulk_delete_students(const httplib::Request& req, httplib::Response& res);
static void handle_exam_start(const httplib::Request& req, httplib::Response& res);
static void handle_exam_resume(const httplib::Request& req, httplib::Response& res);
static void handle_exam_answer(const httplib::Request& req, httplib::Response& res);
static void handle_exam_submit(const httplib::Request& req, httplib::Response& res);
static void handle_report_exam(const httplib::Request& req, httplib::Response& res);
static void handle_report_scoreboard(const httplib::Request& req, httplib::Response& res);
static void handle_admin_save(const httplib::Request& req, httplib::Response& res);

static bool g_fullscreenRequired = false;
static void handle_get_system_settings(const httplib::Request& req, httplib::Response& res);
static void handle_post_system_settings(const httplib::Request& req, httplib::Response& res);

static bool runValidationTests() {
    std::cout << "=== RUNNING STORAGE PERSISTENCE VALIDATION TESTS ===\n";
    int passed = 0;
    int total = 0;

    // Test 1: Class with forbidden delimiter '|'
    total++;
    Lop invalidClass;
    invalidClass.MALOP = "MALOP|BAD";
    invalidClass.TENLOP = "Ten Lop Bad";
    std::string errReason;
    if (!StorageValidator::validateClass(invalidClass, errReason)) {
        std::cout << "[PASS] Test 1: Rejected invalid Class MALOP containing '|'. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 1: Failed to reject invalid Class MALOP containing '|'.\n";
    }

    // Test 2: Student with empty MASV
    total++;
    SinhVien invalidStudent;
    invalidStudent.MASV = "";
    invalidStudent.HO = "Nguyen";
    invalidStudent.TEN = "An";
    if (!StorageValidator::validateStudent(invalidStudent, "D22CQCN01", errReason)) {
        std::cout << "[PASS] Test 2: Rejected Student with empty MASV. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 2: Failed to reject Student with empty MASV.\n";
    }

    // Test 3: Question with empty option
    total++;
    CauHoi invalidQuestion;
    invalidQuestion.ID = 1;
    invalidQuestion.NOIDUNG = "Cau hoi test";
    invalidQuestion.A = "Dap an A";
    invalidQuestion.B = ""; // empty option B
    invalidQuestion.C = "Dap an C";
    invalidQuestion.D = "Dap an D";
    invalidQuestion.DAPAN_DUNG = 'A';
    if (!StorageValidator::validateQuestion(invalidQuestion, "INT1339", errReason)) {
        std::cout << "[PASS] Test 3: Rejected Question with empty option B. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 3: Failed to reject Question with empty option B.\n";
    }

    // Test 4: Score out of range (15.0)
    total++;
    DiemThi invalidScore;
    std::strcpy(invalidScore.MAMH, "INT1339");
    invalidScore.DIEM = 15.0f; // invalid score > 10
    Class dummyClass; Subject dummySubject;
    if (!StorageValidator::validateScore("N22DCCN001", invalidScore, dummyClass, dummySubject, errReason)) {
        std::cout << "[PASS] Test 4: Rejected Score out of range (>10.0). Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 4: Failed to reject Score out of range.\n";
    }

    // Test 5: ExamSession with invalid question ID <= 0
    total++;
    ExamSession invalidSession;
    invalidSession.MASV = "N22DCCN001";
    invalidSession.MAMH = "INT1339";
    invalidSession.tongThoiGianPhut = 15;
    invalidSession.questionIds.push_back(-5); // invalid ID
    if (!StorageValidator::validateExamSession(invalidSession, dummyClass, dummySubject, errReason)) {
        std::cout << "[PASS] Test 5: Rejected ExamSession with negative question ID. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 5: Failed to reject ExamSession with negative question ID.\n";
    }

    // Test 6: Control character rejection ('\t' in student name)
    total++;
    SinhVien ctrlStudent;
    ctrlStudent.MASV = "N22DCCN099";
    ctrlStudent.HO = "Nguyen\tVan"; // Tab character
    ctrlStudent.TEN = "An";
    if (!StorageValidator::validateStudent(ctrlStudent, "D22CQCN01", errReason)) {
        std::cout << "[PASS] Test 6: Rejected Student with control character (\\t). Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 6: Failed to reject Student with control character.\n";
    }

    // Test 7: NaN Float Score rejection
    total++;
    DiemThi nanScore;
    std::strcpy(nanScore.MAMH, "INT1339");
    nanScore.DIEM = std::numeric_limits<float>::quiet_NaN();
    if (!StorageValidator::validateScore("N22DCCN001", nanScore, dummyClass, dummySubject, errReason)) {
        std::cout << "[PASS] Test 7: Rejected NaN Float Score. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 7: Failed to reject NaN Float Score.\n";
    }

    // Test 8: Duplicate option rejection (Option A == B after normalization)
    total++;
    CauHoi dupOptionQuestion;
    dupOptionQuestion.ID = 2;
    dupOptionQuestion.NOIDUNG = "Duplicate options test";
    dupOptionQuestion.A = "Gia tri giong nhau";
    dupOptionQuestion.B = "Gia tri giong nhau"; // A == B
    dupOptionQuestion.C = "Dap an C";
    dupOptionQuestion.D = "Dap an D";
    dupOptionQuestion.DAPAN_DUNG = 'A';
    if (!StorageValidator::validateQuestion(dupOptionQuestion, "INT1339", errReason)) {
        std::cout << "[PASS] Test 8: Rejected Question with duplicate options. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 8: Failed to reject Question with duplicate options.\n";
    }

    // Test 9: Multiple space collapsing for TENLOP
    total++;
    std::string inputMultiSpace = "Cong              Nghe Thong         Tin";
    std::string expectedNormSpace = "Cong Nghe Thong Tin";
    std::string actualNormSpace = StringNormalizer::normalizeHumanText(inputMultiSpace);
    if (actualNormSpace == expectedNormSpace) {
        std::cout << "[PASS] Test 9: Successfully collapsed multiple spaces into single space: '" << actualNormSpace << "'\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 9: Space collapsing failed. Expected: '" << expectedNormSpace << "', Got: '" << actualNormSpace << "'\n";
    }

    // Test 10: Tab to space conversion
    total++;
    std::string inputTab = "Lop\tCNTT\t1";
    std::string expectedTab = "Lop CNTT 1";
    std::string actualTab = StringNormalizer::normalizeHumanText(inputTab);
    if (actualTab == expectedTab) {
        std::cout << "[PASS] Test 10: Successfully converted tabs to single spaces: '" << actualTab << "'\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 10: Tab conversion failed. Expected: '" << expectedTab << "', Got: '" << actualTab << "'\n";
    }

    // Test 11: Leading & trailing space trimming
    total++;
    std::string inputTrim = "   Mon Cau Truc Du Lieu   ";
    std::string expectedTrim = "Mon Cau Truc Du Lieu";
    std::string actualTrim = StringNormalizer::normalizeHumanText(inputTrim);
    if (actualTrim == expectedTrim) {
        std::cout << "[PASS] Test 11: Successfully trimmed leading & trailing spaces: '" << actualTrim << "'\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 11: Trimming failed. Expected: '" << expectedTrim << "', Got: '" << actualTrim << "'\n";
    }

    // Test 12: Newline preservation & rejection
    total++;
    std::string inputNewline = "Noi dung\nCau hoi";
    std::string actualNewline = StringNormalizer::normalizeHumanText(inputNewline);
    Lop nlClass; nlClass.MALOP = "MALOP01"; nlClass.TENLOP = actualNewline;
    if (!StorageValidator::validateClass(nlClass, errReason)) {
        std::cout << "[PASS] Test 12: Preserved newline and successfully rejected by Validator. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 12: Failed to reject string with newline.\n";
    }

    // Test 13: Password preservation
    total++;
    SinhVien passSv;
    passSv.MASV = "N22DCCN001";
    passSv.HO = "Nguyen";
    passSv.TEN = "An";
    passSv.passsword = "  P@ss 123  ";
    StringNormalizer::normalizeStudent(passSv);
    if (passSv.passsword == "  P@ss 123  ") {
        std::cout << "[PASS] Test 13: Password preserved 100% untouched ('  P@ss 123  ').\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 13: Password was mutated unexpectedly!\n";
    }

    // Test 14: Identifier trim only (MALOP)
    total++;
    std::string malopPad = "  D22CQCN01  ";
    std::string normMalop = StringNormalizer::trimIdentifier(malopPad);
    if (normMalop == "D22CQCN01") {
        std::cout << "[PASS] Test 14: Identifier trimmed correctly ('D22CQCN01').\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 14: Identifier trim failed.\n";
    }

    // Test 15: Save SystemSettings ON → verify disk matches
    total++;
    {
        bool saved = StorageManager::getInstance().saveSystemSettings(true);
        bool disk = false;
        StorageManager::getInstance().loadSystemSettings(disk);
        if (saved && disk == true) {
            std::cout << "[PASS] Test 15: Save ON persisted correctly (disk=" << disk << ").\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 15: Save ON failed (saved=" << saved << " disk=" << disk << ").\n";
        }
    }

    // Test 16: Save SystemSettings OFF → verify disk matches
    total++;
    {
        bool saved = StorageManager::getInstance().saveSystemSettings(false);
        bool disk = true;
        StorageManager::getInstance().loadSystemSettings(disk);
        if (saved && disk == false) {
            std::cout << "[PASS] Test 16: Save OFF persisted correctly (disk=" << disk << ").\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 16: Save OFF failed (saved=" << saved << " disk=" << disk << ").\n";
        }
    }

    // Test 17: Load after simulated restart
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        bool loaded = false;
        StorageManager::getInstance().loadSystemSettings(loaded);
        if (loaded == true) {
            std::cout << "[PASS] Test 17: Load after restart returns persisted value (true).\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 17: Load after restart returned wrong value (loaded=" << loaded << ").\n";
        }
    }

    // Test 18: Reset storage → SystemSettings.txt = fullscreen=false
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        StorageManager::getInstance().resetToDefault();
        bool loaded = true;
        StorageManager::getInstance().loadSystemSettings(loaded);
        if (loaded == false) {
            std::cout << "[PASS] Test 18: Reset storage sets SystemSettings to false.\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 18: Reset storage did not reset SystemSettings (loaded=" << loaded << ").\n";
        }
    }

    // Test 19: Multiple toggles — final state matches disk
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        StorageManager::getInstance().saveSystemSettings(false);
        StorageManager::getInstance().saveSystemSettings(true);
        StorageManager::getInstance().saveSystemSettings(false);
        bool disk = true;
        StorageManager::getInstance().loadSystemSettings(disk);
        if (disk == false) {
            std::cout << "[PASS] Test 19: Multiple toggles final state correct (disk=false).\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 19: Multiple toggles final state wrong (disk=" << disk << ").\n";
        }
    }

    // Test 20: Verify file format
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        std::string path = PathResolver::getFilePath("SystemSettings.txt");
        std::ifstream f(path);
        std::string content;
        if (f.is_open()) {
            std::getline(f, content);
            f.close();
        }
        if (content == "fullscreen=true") {
            std::cout << "[PASS] Test 20: File format correct ('fullscreen=true').\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 20: File format wrong (content='" << content << "').\n";
        }
    }

    std::cout << "=== VALIDATION TEST SUMMARY: " << passed << "/" << total << " PASSED ===\n";
    return passed == total;
}

int main(int argc, char* argv[]) {
    // Initialize PathResolver with executable path to ensure CWD-independent storage resolution
    PathResolver::init(argc > 0 ? argv[0] : "");

    // Check for CLI flags
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--reset-storage") {
            if (StorageManager::getInstance().resetToDefault()) {
                std::cout << "Storage reset to default empty state.\n";
            } else {
                std::cerr << "Failed to reset storage.\n";
            }
            return 0;
        }
        if (arg == "--test-validation") {
            return runValidationTests() ? 0 : 1;
        }
    }

    std::cout << "[STARTUP LOG] [BEGIN] LoadAllData\n";
    LoadAllData(dsl, dsmh);
    std::cout << "[STARTUP LOG] [END] LoadAllData\n";

    std::cout << "[STARTUP LOG] [BEGIN] loadSystemSettings\n";
    {
        bool loadedFs = false;
        StorageManager::getInstance().loadSystemSettings(loadedFs);
        g_fullscreenRequired = loadedFs;
    }
    std::cout << "[STARTUP LOG] [END] loadSystemSettings\n";

    std::cout << "============================================================\n";
    std::cout << "Loaded System Settings:\n";
    std::cout << "- Fullscreen Supervision: " << (g_fullscreenRequired ? "ON" : "OFF") << "\n";
    std::cout << "============================================================\n";

    std::cout << "[STARTUP LOG] [BEGIN] adjustSessionsForDowntime\n";
    std::time_t startupTime = std::time(nullptr);
    adjustSessionsForDowntime(startupTime);
    std::cout << "[STARTUP LOG] [END] adjustSessionsForDowntime\n";

    httplib::Server svr;

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

    svr.Post("/api/login", handle_login);
    svr.Post("/api/logout", handle_logout);
    svr.Post("/api/admin/save", handle_admin_save);
    svr.Post("/api/admin/rebuild-used", handle_rebuild_used);
    svr.Get("/api/classes", handle_get_classes);
    svr.Get("/api/classes/:id", handle_get_class_by_id);
    svr.Post("/api/classes", handle_create_class);
    svr.Put("/api/classes/:id", handle_update_class);
    svr.Delete("/api/classes/:id", handle_delete_class);
    svr.Get("/api/students", handle_get_students);
    svr.Get("/api/students/:id", handle_get_student_by_id);
    svr.Post("/api/students", handle_create_student);
    svr.Post("/api/students/bulk-delete", handle_bulk_delete_students);
    svr.Put("/api/students/:id", handle_update_student);
    svr.Delete("/api/students/:id", handle_delete_student);
    svr.Get("/api/subjects", handle_get_subjects);
    svr.Get("/api/subjects/:id", handle_get_subject_by_id);
    svr.Post("/api/subjects", handle_create_subject);
    svr.Put("/api/subjects/:id", handle_update_subject);
    svr.Delete("/api/subjects/:id", handle_delete_subject);

    svr.Post("/api/questions", handle_create_question);
    svr.Post("/api/questions/bulk-delete", handle_bulk_delete_questions);
    svr.Put("/api/questions/:id", handle_update_question);
    svr.Delete("/api/questions/:id", handle_delete_question);
    svr.Post("/api/exams/start", handle_exam_start);
    svr.Get("/api/exams/resume", handle_exam_resume);
    svr.Put("/api/exams/answer", handle_exam_answer);
    svr.Post("/api/exams/submit", handle_exam_submit);
    svr.Get("/api/reports/exam", handle_report_exam);
    svr.Get("/api/reports/scoreboard", handle_report_scoreboard);
    svr.Delete("/api/scores", handle_delete_score);
    svr.Get("/api/system/settings", handle_get_system_settings);
    svr.Post("/api/system/settings", handle_post_system_settings);

    std::cout << "[STARTUP LOG] [BEGIN] svr.listen(0.0.0.0, 8080)\n";
    cout << "Server running on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    std::cout << "[STARTUP LOG] [END] svr.listen(0.0.0.0, 8080)\n";

    SaveAllData(dsl, dsmh);
    return 0;
}

// === CONTROLLERS ===

static void handle_login(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
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
        if (login_student(*dsl.getRoot(), username, password)) {
            SinhVien* sv = nullptr;
            dsLop* root = dsl.getRoot();
            for (int i = 0; i < root->n && !sv; i++)
                if (root->dslop[i]) sv = root->dslop[i]->dssinhvien.find(username);
            if (!sv) { error_response(res, "Student not found", 404); return; }

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

static void handle_logout(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
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

static void handle_get_classes(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    dsLop* root = dsl.getRoot();
    json arr = json::array();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            if (!root->dslop[i]) continue;
            arr.push_back({{"malop",root->dslop[i]->MALOP},
                {"tenlop",root->dslop[i]->TENLOP},
                {"siso",root->dslop[i]->dssinhvien.size()}});
        }
    }
    json_response(res, arr);
}

static void handle_get_class_by_id(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string malop = get_path_param(req, "id");
    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }
    json_response(res, {{"malop",lop->MALOP},{"tenlop",lop->TENLOP},
        {"siso",lop->dssinhvien.size()}});
}

static void handle_create_class(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string malop = body.value("malop", "");
    string tenlop = body.value("tenlop", "");

    std::cout << "[TENLOP AUDIT Step 1] HTTP request raw tenlop: '" << tenlop << "', len=" << tenlop.length() << std::endl;

    if (malop.empty() || tenlop.empty()) {
        error_response(res, "malop and tenlop are required", 400);
        return;
    }

    if (malop.length() > 15) {
        error_response(res, "malop must be at most 15 characters", 400);
        return;
    }

    // Check if class already exists
    if (dsl.find(malop)) {
        error_response(res, "Class already exists", 409);
        return;
    }

    Lop* lop = new Lop();
    lop->MALOP = malop;
    lop->TENLOP = tenlop;
    StringNormalizer::normalizeClass(*lop);

    string errReason;
    if (!StorageValidator::validateClass(*lop, errReason)) {
        delete lop;
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    if (dsl.insert(lop)) {
        json_response(res, {{"malop", lop->MALOP}, {"tenlop", lop->TENLOP}, {"siso", 0}}, 201);
    } else {
        error_response(res, "Failed to create class", 500);
    }
}

static void handle_update_class(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string malop = get_path_param(req, "id");

    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string tenlop = body.value("tenlop", "");

    if (tenlop.empty()) {
        error_response(res, "tenlop is required", 400);
        return;
    }

    Lop* lop = dsl.find(malop);
    if (!lop) {
        error_response(res, "Class not found", 404);
        return;
    }

    Lop candidateLop;
    candidateLop.MALOP = malop;
    candidateLop.TENLOP = tenlop;
    StringNormalizer::normalizeClass(candidateLop);

    string errReason;
    if (!StorageValidator::validateClass(candidateLop, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    // Use Class::update() which persists and handles rollback
    if (!dsl.update(malop, candidateLop.TENLOP)) {
        error_response(res, "Failed to update class", 500);
        return;
    }

    Lop* updated = dsl.find(malop);
    if (updated) {
        std::cout << "[TENLOP AUDIT Step 3] Update RAM Object TENLOP: '" << updated->TENLOP << "', len=" << updated->TENLOP.length() << std::endl;
    }
    json_response(res, {{"malop", updated->MALOP}, {"tenlop", updated->TENLOP},
        {"siso", updated->dssinhvien.size()}});
}

static void handle_delete_class(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string malop = get_path_param(req, "id");

    Lop* lop = dsl.find(malop);
    if (!lop) {
        error_response(res, "Class not found", 404);
        return;
    }

    if (lop->dssinhvien.size() > 0) {
        custom_json_response(res, {
            {"success", false},
            {"error", "Unprocessable Entity"},
            {"message", "Lop van con sinh vien, khong the xoa."}
        }, 422);
        return;
    }

    if (dsl.remove(malop)) {
        res.status = 204;
    } else {
        error_response(res, "Failed to delete class", 500);
    }
}

static void handle_get_students(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string malop = req.get_param_value("malop");
    if (malop.empty()) { error_response(res, "malop required", 400); return; }
    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }

    json arr = json::array();
    dsSinhVien* cur = lop->dssinhvien.getRoot();
    while (cur) {
        cur->sinhvien.dsdiemthi.load(cur->sinhvien.MASV);
        int examCount = cur->sinhvien.dsdiemthi.count();
        arr.push_back({
            {"masv", cur->sinhvien.MASV},
            {"ho", cur->sinhvien.HO},
            {"ten", cur->sinhvien.TEN},
            {"phai", cur->sinhvien.PHAI},
            {"malop", malop},
            {"examCount", examCount}
        });
        cur = cur->next;
    }
    json_response(res, arr);
}

static void handle_get_student_by_id(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string masv = get_path_param(req, "id");
    if (masv.empty()) { error_response(res, "masv required", 400); return; }
    dsLop* root = dsl.getRoot();
    for (int i = 0; i < root->n; i++) {
        if (!root->dslop[i]) continue;
        SinhVien* sv = root->dslop[i]->dssinhvien.find(masv);
        if (sv) {
            sv->dsdiemthi.load(sv->MASV);
            int examCount = sv->dsdiemthi.count();
            json scoresArr = json::array();
            dsDiemThi* node = sv->dsdiemthi.getRoot();
            while (node) {
                NodeMH* subNode = dsmh.find(node->diemthi.MAMH);
                string tenmh = subNode ? subNode->data.TENMH : string(node->diemthi.MAMH);
                scoresArr.push_back({
                    {"mamh", string(node->diemthi.MAMH)},
                    {"tenmh", tenmh},
                    {"diem", node->diemthi.DIEM}
                });
                node = node->next;
            }

            json_response(res, {
                {"masv", sv->MASV},
                {"ho", sv->HO},
                {"ten", sv->TEN},
                {"phai", sv->PHAI},
                {"malop", root->dslop[i]->MALOP},
                {"tenlop", root->dslop[i]->TENLOP},
                {"examCount", examCount},
                {"scores", scoresArr}
            });
            return;
        }
    }
    error_response(res, "Student not found", 404);
}

static void handle_create_student(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string malop = body.value("malop", "");
    string masv = body.value("masv", "");
    string ho = body.value("ho", "");
    string ten = body.value("ten", "");
    string phai = body.value("phai", "");
    string password = body.value("password", "");
    if (malop.empty() || masv.empty() || ho.empty() || ten.empty()) {
        error_response(res, "malop, masv, ho, ten are required", 400); return;
    }
    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }
    // Global uniqueness check — search ALL classes, not just current class
    if (findStudentGlobal(masv, nullptr)) { error_response(res, "Mã sinh viên đã tồn tại trong hệ thống.", 409); return; }
    SinhVien sv; sv.MASV = masv; sv.HO = ho; sv.TEN = ten; sv.PHAI = phai; sv.passsword = password;
    StringNormalizer::normalizeStudent(sv);

    string errReason;
    if (!StorageValidator::validateStudent(sv, malop, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    if (lop->dssinhvien.insert(sv)) {
        if (StorageManager::getInstance().saveStudents(dsl)) {
            json_response(res, {{"masv",sv.MASV},{"ho",sv.HO},{"ten",sv.TEN},{"phai",sv.PHAI},{"malop",malop},{"examCount",0}}, 201);
        } else { lop->dssinhvien.remove(masv); error_response(res, "Failed to save student", 500); }
    } else { error_response(res, "Failed to create student", 500); }
}

static void handle_update_student(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string masv = get_path_param(req, "id");
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string ho = body.value("ho", "");
    string ten = body.value("ten", "");
    string phai = body.value("phai", "");
    string password = body.value("password", "");
    if (ho.empty() || ten.empty()) { error_response(res, "ho and ten are required", 400); return; }
    SinhVien* sv = nullptr; Lop* foundLop = nullptr;
    dsLop* root = dsl.getRoot();
    for (int i = 0; i < root->n && !sv; i++) {
        if (!root->dslop[i]) continue;
        sv = root->dslop[i]->dssinhvien.find(masv);
        if (sv) foundLop = root->dslop[i];
    }
    if (!sv) { error_response(res, "Student not found", 404); return; }
    SinhVien newData; newData.MASV = masv; newData.HO = ho; newData.TEN = ten; newData.PHAI = phai; newData.passsword = password.empty() ? sv->passsword : password;
    StringNormalizer::normalizeStudent(newData);

    string errReason;
    if (!StorageValidator::validateStudent(newData, foundLop->MALOP, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }
    if (foundLop->dssinhvien.update(masv, newData)) {
        if (StorageManager::getInstance().saveStudents(dsl)) {
            sv->dsdiemthi.load(sv->MASV);
            int examCount = sv->dsdiemthi.count();
            json_response(res, {{"masv",sv->MASV},{"ho",sv->HO},{"ten",sv->TEN},{"phai",sv->PHAI},{"malop",foundLop->MALOP},{"examCount",examCount}});
        } else { error_response(res, "Failed to save student", 500); }
    } else { error_response(res, "Failed to update student", 500); }
}

static void handle_delete_student(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string masv = get_path_param(req, "id");
    SinhVien* sv = nullptr; Lop* foundLop = nullptr;
    dsLop* root = dsl.getRoot();
    for (int i = 0; i < root->n && !sv; i++) {
        if (!root->dslop[i]) continue;
        sv = root->dslop[i]->dssinhvien.find(masv);
        if (sv) foundLop = root->dslop[i];
    }
    if (!sv) { error_response(res, "Student not found", 404); return; }

    // Check if student has exam scores via domain method
    sv->dsdiemthi.load(masv);
    if (!sv->dsdiemthi.empty()) {
        custom_json_response(res, {
            {"success", false},
            {"error", "Unprocessable Entity"},
            {"message", "Sinh viên đã có kết quả thi, không thể xóa."}
        }, 422);
        return;
    }

    if (foundLop->dssinhvien.remove(masv)) {
        if (StorageManager::getInstance().saveStudents(dsl)) {
            res.status = 204;
        }
        else { error_response(res, "Failed to save after deletion", 500); }
    } else { error_response(res, "Failed to delete student", 500); }
}

static void handle_bulk_delete_students(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    json masvList = body.is_array() ? body : body.value("masv", json::array());
    if (masvList.empty()) {
        error_response(res, "No student IDs provided", 400); return;
    }

    // 1. Transactional Pre-check Phase
    for (auto& item : masvList) {
        string masv = item.is_string() ? item.get<string>() : item.value("masv", "");
        if (masv.empty()) continue;

        SinhVien* sv = nullptr;
        dsLop* root = dsl.getRoot();
        for (int i = 0; i < root->n && !sv; i++) {
            if (!root->dslop[i]) continue;
            sv = root->dslop[i]->dssinhvien.find(masv);
        }
        if (!sv) {
            error_response(res, "Student " + masv + " not found", 404);
            return;
        }
        sv->dsdiemthi.load(masv);
        if (!sv->dsdiemthi.empty()) {
            custom_json_response(res, {
                {"success", false},
                {"message", "Sinh vien " + masv + " da co ket qua thi, khong the xoa."}
            }, 422);
            return;
        }
    }

    // 2. Execution Phase
    int deletedCount = 0;
    for (auto& item : masvList) {
        string masv = item.is_string() ? item.get<string>() : item.value("masv", "");
        if (masv.empty()) continue;

        SinhVien* sv = nullptr;
        Lop* foundLop = nullptr;
        dsLop* root = dsl.getRoot();
        for (int i = 0; i < root->n && !sv; i++) {
            if (!root->dslop[i]) continue;
            sv = root->dslop[i]->dssinhvien.find(masv);
            if (sv) foundLop = root->dslop[i];
        }

        if (sv && foundLop) {
            if (foundLop->dssinhvien.remove(masv)) {
                deletedCount++;
            }
        }
    }

    StorageManager::getInstance().saveStudents(dsl);
    json_response(res, {{"deletedCount", deletedCount}});
}

// Global student lookup — searches ALL classes, NOT just current class
static SinhVien* findStudentGlobal(const string& masv, Lop** outLop) {
    dsLop* root = dsl.getRoot();
    if (!root) return nullptr;
    for (int i = 0; i < root->n; i++) {
        if (!root->dslop[i]) continue;
        SinhVien* sv = root->dslop[i]->dssinhvien.find(masv);
        if (sv) {
            if (outLop) *outLop = root->dslop[i];
            return sv;
        }
    }
    return nullptr;
}

static NodeMH* find_subject_smart(const string& mamh) {
    return dsmh.find(mamh.c_str());
}

static void collectSubjectsForAPI(NodeMH* node, json& arr) {
    if (!node) return;
    collectSubjectsForAPI(node->left, arr);
    arr.push_back({
        {"mamh", string(node->data.MAMH)},
        {"tenmh", node->data.TENMH},
        {"used", node->data.used},
        {"questionCount", node->data.dsCauHoi.size()}
    });
    collectSubjectsForAPI(node->right, arr);
}

static void handle_get_subjects(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    json arr = json::array();
    collectSubjectsForAPI(dsmh.getRoot(), arr);
    json_response(res, arr);
}

static void handle_get_subject_by_id(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string mamh = get_path_param(req, "id");
    NodeMH* node = find_subject_smart(mamh);
    if (!node) { error_response(res, "Subject not found", 404); return; }

    json questions = json::array();
    for (int id = 0; id < 10000; id++) {
        dsCHT* q = node->data.dsCauHoi.find(id);
        if (q) {
            questions.push_back({{"id",q->cauhoi.ID},{"noidung",q->cauhoi.NOIDUNG},
                {"A",q->cauhoi.A},{"B",q->cauhoi.B},{"C",q->cauhoi.C},{"D",q->cauhoi.D},
                {"dapan",string(1,q->cauhoi.DAPAN_DUNG)},{"used",q->cauhoi.used}});
        }
    }
    json_response(res, {{"mamh",node->data.MAMH},{"tenmh",node->data.TENMH},
        {"used",node->data.used},{"questions",questions}});
}

static void handle_create_subject(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string mamh = body.value("mamh", "");
    string tenmh = body.value("tenmh", "");
    if (mamh.empty() || tenmh.empty()) {
        error_response(res, "mamh and tenmh are required", 400); return;
    }
    if (mamh.length() > 15) {
        error_response(res, "mamh must be at most 15 characters", 400); return;
    }
    if (dsmh.find(mamh.c_str())) {
        error_response(res, "Subject already exists", 409); return;
    }
    MonHoc mh;
    strcpy(mh.MAMH, mamh.c_str());
    mh.TENMH = tenmh;
    mh.used = false;
    StringNormalizer::normalizeSubject(mh);

    string errReason;
    if (!StorageValidator::validateSubject(mh, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    if (dsmh.insert(mh)) {
        json_response(res, {{"mamh",mh.MAMH},{"tenmh",mh.TENMH},{"used",false}}, 201);
    } else {
        error_response(res, "Failed to create subject", 500);
    }
}

static void handle_update_subject(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string mamh = get_path_param(req, "id");
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string tenmh = body.value("tenmh", "");
    if (tenmh.empty()) {
        error_response(res, "tenmh is required", 400); return;
    }
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) {
        error_response(res, "Subject not found", 404); return;
    }
    MonHoc candidateMh;
    strcpy(candidateMh.MAMH, mamh.c_str());
    candidateMh.TENMH = tenmh;
    StringNormalizer::normalizeSubject(candidateMh);

    string errReason;
    if (!StorageValidator::validateSubject(candidateMh, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    if (dsmh.update(mamh.c_str(), candidateMh.TENMH)) {
        json_response(res, {{"mamh",node->data.MAMH},{"tenmh",node->data.TENMH},{"used",node->data.used}});
    } else {
        error_response(res, "Failed to update subject", 500);
    }
}

static void handle_delete_subject(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string mamh = get_path_param(req, "id");
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) {
        error_response(res, "Subject not found", 404); return;
    }
    if (dsmh.remove(mamh.c_str())) {
        res.status = 204;
    } else {
        error_response(res, "Failed to delete subject (may have used questions)", 422);
    }
}

static void handle_create_question(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string mamh = body.value("mamh", "");
    string noidung = body.value("noidung", "");
    string a = body.value("a", "");
    string b = body.value("b", "");
    string c = body.value("c", "");
    string d = body.value("d", "");
    string dapan = body.value("dapan", "");
    if (mamh.empty() || noidung.empty() || dapan.empty()) {
        error_response(res, "mamh, noidung, dapan are required", 400); return;
    }
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) { error_response(res, "Subject not found", 404); return; }
    CauHoi ch; ch.ID = 1; // Temporary ID for pre-validation check
    ch.NOIDUNG = noidung; ch.A = a; ch.B = b; ch.C = c; ch.D = d;
    ch.DAPAN_DUNG = dapan.empty() ? '\0' : dapan[0];
    ch.used = false;
    StringNormalizer::normalizeQuestion(ch);

    string errReason;
    if (!StorageValidator::validateQuestion(ch, mamh, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }
    ch.ID = 0; // Reset for auto-increment insertion

    if (node->data.dsCauHoi.insert(ch, true)) {
        if (StorageManager::getInstance().saveQuestions(dsmh)) {
            json_response(res, {{"id",ch.ID},{"mamh",mamh},{"noidung",ch.NOIDUNG},
                {"a",ch.A},{"b",ch.B},{"c",ch.C},{"d",ch.D},{"dapan",dapan},{"used",false}}, 201);
        } else {
            node->data.dsCauHoi.removeNode(ch.ID);
            error_response(res, "Failed to save question", 500);
        }
    } else { error_response(res, "Failed to create question", 500); }
}

static void handle_update_question(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string idStr = get_path_param(req, "id");
    if (idStr.empty()) { error_response(res, "id required", 400); return; }
    int id = std::stoi(idStr);
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string mamh = body.value("mamh", "");
    if (mamh.empty()) { error_response(res, "mamh is required", 400); return; }
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) { error_response(res, "Subject not found", 404); return; }
    dsCHT* found = node->data.dsCauHoi.find(id);
    if (!found) { error_response(res, "Question not found", 404); return; }
    CauHoi newData;
    newData.ID = id;
    newData.NOIDUNG = body.value("noidung", found->cauhoi.NOIDUNG);
    newData.A = body.value("a", found->cauhoi.A);
    newData.B = body.value("b", found->cauhoi.B);
    newData.C = body.value("c", found->cauhoi.C);
    newData.D = body.value("d", found->cauhoi.D);
    string dapan = body.value("dapan", string(1, found->cauhoi.DAPAN_DUNG));
    newData.DAPAN_DUNG = dapan.empty() ? '\0' : dapan[0];
    StringNormalizer::normalizeQuestion(newData);

    string errReason;
    if (!StorageValidator::validateQuestion(newData, mamh, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }
    if (node->data.dsCauHoi.update(id, newData)) {
        if (StorageManager::getInstance().saveQuestions(dsmh)) {
            json_response(res, {{"id",id},{"mamh",mamh},{"noidung",newData.NOIDUNG},
                {"a",newData.A},{"b",newData.B},{"c",newData.C},{"d",newData.D},
                {"dapan",dapan},{"used",found->cauhoi.used}});
        } else { error_response(res, "Failed to save question", 500); }
    } else { error_response(res, "Failed to update question (may be used)", 422); }
}

static void handle_delete_question(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string idStr = get_path_param(req, "id");
    if (idStr.empty()) { error_response(res, "id required", 400); return; }
    int id = std::stoi(idStr);
    string mamh = req.get_param_value("mamh");

    // Search for question by ID across all subjects
    NodeMH* foundNode = nullptr;
    dsCHT* foundQNode = nullptr;

    if (!mamh.empty()) {
        // If mamh is provided, search only that subject (backward compatible)
        foundNode = dsmh.find(mamh.c_str());
        if (foundNode) {
            foundQNode = foundNode->data.dsCauHoi.find(id);
        }
    } else {
        // Search every subject to find the question by ID
        std::function<NodeMH*(NodeMH*)> searchNode = [&](NodeMH* node) -> NodeMH* {
            if (!node) return nullptr;
            NodeMH* left = searchNode(node->left);
            if (left) return left;
            if (node->data.dsCauHoi.find(id)) return node;
            return searchNode(node->right);
        };
        foundNode = searchNode(dsmh.getRoot());
        if (foundNode) {
            foundQNode = foundNode->data.dsCauHoi.find(id);
            mamh = string(foundNode->data.MAMH);
        }
    }

    if (!foundQNode) { error_response(res, "Question not found", 404); return; }
    if (foundQNode->cauhoi.deleted) { error_response(res, "Question already disabled", 422); return; }

    // BUSINESS RULE: Hybrid delete
    string mode;
    if (foundQNode->cauhoi.used) {
        // CASE B: used == true → Soft delete (preserve for historical exams)
        if (!foundNode->data.dsCauHoi.setDeleted(id)) {
            error_response(res, "Failed to soft delete question", 422); return;
        }
        mode = "soft";
    } else {
        // CASE A: used == false → Hard delete (physically remove, no historical value)
        if (!foundNode->data.dsCauHoi.removeNode(id)) {
            error_response(res, "Failed to delete question", 422); return;
        }
        mode = "hard";
    }

    if (StorageManager::getInstance().saveQuestions(dsmh)) {
        json_response(res, {{"id", id}, {"mamh", mamh}, {"mode", mode}});
    } else {
        error_response(res, "Failed to save after deletion", 500);
    }
}

static void handle_bulk_delete_questions(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    json items = body.is_array() ? body : body.value("items", json::array());
    if (items.empty()) {
        error_response(res, "No items provided for bulk delete", 400); return;
    }

    // 1. Transactional Pre-check Phase: Ensure ALL requested questions exist and used == false
    for (auto& item : items) {
        int id = item.value("id", 0);
        string mamh = item.value("mamh", "");
        if (id <= 0 || mamh.empty()) {
            error_response(res, "Invalid item format in bulk delete", 400); return;
        }
        NodeMH* node = dsmh.find(mamh.c_str());
        if (!node) {
            error_response(res, "Subject " + mamh + " not found", 404); return;
        }
        dsCHT* q = node->data.dsCauHoi.find(id);
        if (!q) {
            error_response(res, "Question ID " + to_string(id) + " not found", 404); return;
        }
        if (q->cauhoi.used) {
            custom_json_response(res, {
                {"success", false},
                {"message", "Mot hoac nhieu cau hoi da chon da duoc su dung trong de thi. Thao tac xoa hang loat da bi huy."}
            }, 422);
            return;
        }
    }

    // 2. Execution Phase: All items are unused -> delete and save affected subjects
    set<string> affectedSubjects;
    for (auto& item : items) {
        int id = item.value("id", 0);
        string mamh = item.value("mamh", "");
        NodeMH* node = dsmh.find(mamh.c_str());
        if (node) {
            node->data.dsCauHoi.removeNode(id);
            affectedSubjects.insert(mamh);
        }
    }

    for (const auto& mamh : affectedSubjects) {
        NodeMH* node = dsmh.find(mamh.c_str());
        if (node) {
            // Update used status for subjects if necessary (not needed as removing a question doesn't mark subject used)
        }
    }

    StorageManager::getInstance().saveQuestions(dsmh);

    json_response(res, {{"success", true}, {"deletedCount", (int)items.size()}});
}

static void handle_exam_start(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string masv = body.value("masv", "");
    string mamh = body.value("mamh", "");
    int socau = body.value("socau", 0);
    int thoigianPhut = body.value("thoigian", 15);
    int thoigianSec = body.value("thoigianSec", 0);
    int remainingSec = (thoigianSec > 0) ? thoigianSec : (thoigianPhut * 60);

    if (remainingSec <= 0 || remainingSec > 120 * 60) {
        error_response(res, "Thoi gian lam bai khong hop le", 400); return;
    }

    if (masv.empty() || mamh.empty() || socau <= 0) {
        error_response(res, "masv, mamh, socau required", 400); return;
    }

    // Check active session first
    ExamSession active;
    if (loadExamSession(masv, active) && active.in_progress) {
        int remSec = calculateRemainingSeconds(active);
        if (remSec > 0) {
            if (active.MAMH != mamh) {
                error_response(res, "Ban dang co bai thi chua hoan thanh o mon " + active.MAMH, 409);
                return;
            }
        }
    }

    // Find student
    SinhVien* sv = nullptr;
    dsLop* root = dsl.getRoot();
    for (int i = 0; i < root->n && !sv; i++)
        if (root->dslop[i]) sv = root->dslop[i]->dssinhvien.find(masv);
    if (!sv) { error_response(res, "Student not found", 404); return; }

    // Check already taken
    if (sv->dsdiemthi.find(mamh.c_str())) {
        error_response(res, "Ban da thi mon nay roi", 409); return;
    }

    // Find subject
    NodeMH* node = find_subject_smart(mamh);
    if (!node) { error_response(res, "Subject not found", 404); return; }

    // Collect questions
    DArray<dsCHT*> all;
    for (int id = 0; id < 10000; id++) {
        dsCHT* q = node->data.dsCauHoi.find(id);
        if (q) all.push_back(q);
    }
    if (all.size() < socau) {
        error_response(res, "Not enough questions", 400); return;
    }

    // Fisher-Yates shuffle
    Utility::Random::Seed();
    for (int i = all.size()-1; i > 0; i--) {
        int j = Utility::Random::RandomInt(0, i);
        Utility::Swap(all[i], all[j]);
    }

    // Mark used & build session data
    ExamSession session;
    session.MASV = masv;
    session.MAMH = mamh;
    session.thoiGianBatDau = std::time(nullptr);
    // Fix: correctly compute session duration in minutes.
    // thoigianSec < 60 → special test mode (e.g. 20s), keep 1 min as minimum.
    // thoigianSec >= 60 → standard mode, convert seconds to minutes.
    // thoigianSec == 0  → no seconds provided, use thoigianPhut directly.
    if (thoigianSec > 0 && thoigianSec < 60) {
        session.tongThoiGianPhut = 1; // test mode minimum
    } else if (thoigianSec >= 60) {
        session.tongThoiGianPhut = thoigianSec / 60;
    } else {
        session.tongThoiGianPhut = thoigianPhut;
    }
    session.in_progress = true;

    json qs = json::array();
    for (int i = 0; i < socau; i++) {
        session.questionIds.push_back(all[i]->cauhoi.ID);
        session.answers.push_back('\0');

        qs.push_back({{"id",all[i]->cauhoi.ID},{"noidung",all[i]->cauhoi.NOIDUNG},
            {"A",all[i]->cauhoi.A},{"B",all[i]->cauhoi.B},
            {"C",all[i]->cauhoi.C},{"D",all[i]->cauhoi.D}});
    }

    session.lastServerActivityAt = std::time(nullptr);
    saveExamSession(session);
    StorageManager::getInstance().rebuildUsedFlags(dsmh);
    StorageManager::getInstance().saveSubjects(dsmh);
    StorageManager::getInstance().saveQuestions(dsmh);

    json_response(res, {
        {"questions", qs},
        {"socau", socau},
        {"thoigian", thoigianPhut},
        {"remainingSeconds", remainingSec},
        {"fullscreenRequired", g_fullscreenRequired}
    });
}

static void handle_exam_resume(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string masv = req.get_param_value("masv");
    if (masv.empty()) {
        error_response(res, "masv query param required", 400); return;
    }

    ExamSession session;
    if (!loadExamSession(masv, session) || !session.in_progress) {
        error_response(res, "No active exam session found", 404); return;
    }

    session.lastServerActivityAt = std::time(nullptr);
    saveExamSession(session);

    int remainingSeconds = calculateRemainingSeconds(session);

    NodeMH* node = find_subject_smart(session.MAMH);
    string tenmh = node ? node->data.TENMH : session.MAMH;

    json questions = json::array();
    for (size_t i = 0; i < session.questionIds.size(); i++) {
        int qId = session.questionIds[i];
        char ans = (i < session.answers.size()) ? session.answers[i] : '\0';
        if (ans == '-') ans = '\0';

        if (node) {
            dsCHT* q = node->data.dsCauHoi.find(qId);
            if (q) {
                string selectedAns = (ans != '\0') ? string(1, ans) : "";
                questions.push_back({
                    {"id", q->cauhoi.ID},
                    {"noidung", q->cauhoi.NOIDUNG},
                    {"A", q->cauhoi.A},
                    {"B", q->cauhoi.B},
                    {"C", q->cauhoi.C},
                    {"D", q->cauhoi.D},
                    {"selectedAnswer", selectedAns}
                });
            }
        }
    }

    json_response(res, {
        {"masv", session.MASV},
        {"mamh", session.MAMH},
        {"tenmh", tenmh},
        {"remainingSeconds", remainingSeconds},
        {"thoiGianPhut", session.tongThoiGianPhut},
        {"thoiGianBatDau", session.thoiGianBatDau},
        {"questions", questions},
        {"fullscreenRequired", g_fullscreenRequired}
    });
}

static void handle_exam_answer(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string masv = body.value("masv", "");
    string mamh = body.value("mamh", "");
    int questionId = body.value("questionId", 0);
    string answerStr = body.value("answer", "");

    if (masv.empty() || mamh.empty() || questionId <= 0) {
        error_response(res, "masv, mamh, questionId required", 400); return;
    }

    ExamSession session;
    if (!loadExamSession(masv, session) || !session.in_progress || session.MAMH != mamh) {
        error_response(res, "Active exam session not found for this subject", 404); return;
    }

    int idx = -1;
    for (size_t i = 0; i < session.questionIds.size(); i++) {
        if (session.questionIds[i] == questionId) {
            idx = (int)i;
            break;
        }
    }

    if (idx >= 0) {
        while (session.answers.size() <= (size_t)idx) {
            session.answers.push_back('\0');
        }
        char ch = answerStr.empty() ? '\0' : answerStr[0];
        session.answers[idx] = toupper(ch);
        session.lastServerActivityAt = std::time(nullptr);
        saveExamSession(session);
    }

    json_response(res, {{"success", true}});
}

static void handle_exam_submit(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string masv = body.value("masv", "");
    string mamh = body.value("mamh", "");
    json answers = body.value("answers", json::array());

    if (masv.empty() || mamh.empty()) {
        error_response(res, "masv, mamh required", 400); return;
    }

    SinhVien* sv = nullptr;
    dsLop* root = dsl.getRoot();
    for (int i = 0; i < root->n && !sv; i++)
        if (root->dslop[i]) sv = root->dslop[i]->dssinhvien.find(masv);
    if (!sv) { error_response(res, "Student not found", 404); return; }

    NodeMH* node = find_subject_smart(mamh);
    if (!node) { error_response(res, "Subject not found", 404); return; }

    // Check single attempt constraint
    if (sv->dsdiemthi.find(mamh.c_str())) {
        removeExamSession(masv);
        error_response(res, "Bai thi nay da duoc nop truoc do", 409); return;
    }

    // Check session in progress
    ExamSession session;
    if (!loadExamSession(masv, session) || !session.in_progress || session.MAMH != mamh) {
        error_response(res, "Bai thi nay da duoc nop truoc do hoac phien thi khong ton tai", 409); return;
    }

    // Grade
    int soDung = 0;
    int total = answers.size();
    for (auto& a : answers) {
        int id = a.value("id", 0);
        string ans = a.value("answer", "");
        dsCHT* q = node->data.dsCauHoi.find(id);
        if (q && !ans.empty() && std::toupper(static_cast<unsigned char>(ans[0])) == std::toupper(static_cast<unsigned char>(q->cauhoi.DAPAN_DUNG))) soDung++;
    }

    float diem = total > 0 ? roundScore(((float)soDung / total) * 10.0, 2) : 0.0f;

    DiemThi dt;
    strcpy(dt.MAMH, mamh.c_str());
    dt.DIEM = diem;
    bool saved = sv->dsdiemthi.insert(dt);
    if (saved) {
        StorageManager::getInstance().saveScores(dsl);
    }

    // Append history to exam_history.txt
    StorageManager::getInstance().appendExamHistory(session, diem);

    // Clear active session file upon successful submit
    removeExamSession(masv);

    StorageManager::getInstance().rebuildUsedFlags(dsmh);
    StorageManager::getInstance().saveSubjects(dsmh);
    StorageManager::getInstance().saveQuestions(dsmh);

    json_response(res, {{"soDung",soDung},{"total",total},
        {"diem",diem},{"saved",saved}});
}

static void handle_report_exam(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string malop = req.get_param_value("malop");
    string masv = req.get_param_value("masv");
    string mamh = req.get_param_value("mamh");

    if (malop.empty() || masv.empty() || mamh.empty()) {
        error_response(res, "malop, masv, mamh required", 400); return;
    }

    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }
    SinhVien* sv = lop->dssinhvien.find(masv);
    if (!sv) { error_response(res, "Student not found", 404); return; }
    dsDiemThi* diem = sv->dsdiemthi.find(mamh.c_str());
    if (!diem) { error_response(res, "Chua thi mon nay", 404); return; }
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) { error_response(res, "Subject not found", 404); return; }

    // Search exam_history.txt for the latest matching MASV and MAMH
    string historyPath = PathResolver::getFilePath("exam_history.txt");
    ifstream histFile(historyPath);
    DArray<int> historyQIds;
    DArray<string> historyAnswers;
    bool foundHistory = false;
    
    if (histFile.is_open()) {
        string line;
        bool isHeader = true;
        while (getline(histFile, line)) {
            // Trim leading/trailing whitespace
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) line.pop_back();
            if (line.empty()) continue;
            if (isHeader) { isHeader = false; continue; }
            
            DArray<string> tokens = split(line, '|');
            if (tokens.size() >= 6) {
                string hMasv = trim(tokens[0]);
                string hMamh = trim(tokens[1]);
                if (hMasv == masv && hMamh == mamh) {
                    foundHistory = true;
                    historyQIds.clear();
                    historyAnswers.clear();
                    
                    // Parse question IDs
                    DArray<string> qIdTokens = split(tokens[4], ',');
                    for (int qIdx = 0; qIdx < qIdTokens.size(); qIdx++) {
                        string qidStr = qIdTokens[qIdx];
                        if (!trim(qidStr).empty()) {
                            historyQIds.push_back(stoi(trim(qidStr)));
                        }
                    }
                    
                    // Parse answers chosen
                    DArray<string> ansTokens = split(tokens[5], ',');
                    for (int aIdx = 0; aIdx < ansTokens.size(); aIdx++) {
                        string ansStr = ansTokens[aIdx];
                        string a = trim(ansStr);
                        if (a == "-") a = "";
                        historyAnswers.push_back(a);
                    }
                }
            }
        }
        histFile.close();
    }

    json questions = json::array();
    if (foundHistory) {
        for (size_t i = 0; i < historyQIds.size(); i++) {
            int qid = historyQIds[i];
            dsCHT* q = node->data.dsCauHoi.find(qid);
            if (q) {
                string selAns = i < historyAnswers.size() ? historyAnswers[i] : "";
                questions.push_back({
                    {"id", q->cauhoi.ID},
                    {"noidung", q->cauhoi.NOIDUNG},
                    {"A", q->cauhoi.A},
                    {"B", q->cauhoi.B},
                    {"C", q->cauhoi.C},
                    {"D", q->cauhoi.D},
                    {"dapan", string(1, q->cauhoi.DAPAN_DUNG)},
                    {"selectedAnswer", selAns}
                });
            }
        }
    } else {
        // Fallback for legacy scores without history record
        for (int id = 0; id < 10000; id++) {
            dsCHT* q = node->data.dsCauHoi.find(id);
            if (q) {
                questions.push_back({
                    {"id", q->cauhoi.ID},
                    {"noidung", q->cauhoi.NOIDUNG},
                    {"A", q->cauhoi.A},
                    {"B", q->cauhoi.B},
                    {"C", q->cauhoi.C},
                    {"D", q->cauhoi.D},
                    {"dapan", string(1, q->cauhoi.DAPAN_DUNG)},
                    {"selectedAnswer", ""}
                });
            }
        }
    }

    json_response(res, {
        {"masv",sv->MASV},{"ho",sv->HO},{"ten",sv->TEN},{"phai",sv->PHAI},
        {"mamh",node->data.MAMH},{"tenmh",node->data.TENMH},
        {"diem",diem->diemthi.DIEM},{"questions",questions}
    });
}

static void handle_delete_score(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string masv = req.get_param_value("masv");
    string mamh = req.get_param_value("mamh");
    string malop = req.get_param_value("malop");

    if (masv.empty() || mamh.empty()) {
        error_response(res, "masv and mamh required", 400); return;
    }

    SinhVien* sv = nullptr;
    if (!malop.empty()) {
        Lop* lop = dsl.find(malop);
        if (lop) sv = lop->dssinhvien.find(masv);
    } else {
        dsLop* root = dsl.getRoot();
        if (root) {
            for (int i = 0; i < root->n && !sv; i++) {
                if (root->dslop[i]) {
                    sv = root->dslop[i]->dssinhvien.find(masv);
                }
            }
        }
    }

    if (!sv) { error_response(res, "Student not found", 404); return; }

    bool removed = sv->dsdiemthi.remove(mamh.c_str());
    if (removed) {
        StorageManager::getInstance().saveScores(dsl);
        StorageManager::getInstance().rebuildUsedFlags(dsmh);
        StorageManager::getInstance().saveSubjects(dsmh);
        StorageManager::getInstance().saveQuestions(dsmh);
        json_response(res, {{"message","Score deleted successfully"},{"masv",masv},{"mamh",mamh}});
    } else {
        error_response(res, "Score not found for this subject", 404);
    }
}

static void collectMAMHNames(NodeMH* node, DArray<string>& mamhs) {
    if (!node) return;
    collectMAMHNames(node->left, mamhs);
    mamhs.push_back(node->data.MAMH);
    collectMAMHNames(node->right, mamhs);
}

static void handle_report_scoreboard(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string malop = req.get_param_value("malop");
    if (malop.empty()) { error_response(res, "malop required", 400); return; }
    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }

    // Get all subjects from RAM
    DArray<string> allMAMH;
    collectMAMHNames(dsmh.getRoot(), allMAMH);

    // Read students from RAM
    json arr = json::array();
    dsSinhVien* cur = lop->dssinhvien.getRoot();
    while (cur) {
        SinhVien* sv = &cur->sinhvien;
        json scores = json::object();
        int examCount = 0;
        for (const auto& m : allMAMH) {
            dsDiemThi* d = sv->dsdiemthi.find(m.c_str());
            if (d) {
                scores[m] = json(d->diemthi.DIEM);
                examCount++;
            } else {
                scores[m] = json(nullptr);
            }
        }
        arr.push_back({
            {"masv", sv->MASV},
            {"ho", sv->HO},
            {"ten", sv->TEN},
            {"examCount", examCount},
            {"scores", scores}
        });
        cur = cur->next;
    }
    json_response(res, {{"malop", malop}, {"students", arr}});
}

static void handle_admin_save(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    bool ok = StorageManager::getInstance().saveAllData(dsl, dsmh);
    if (ok) {
        json_response(res, {{"message", "All storage data saved successfully"}});
    } else {
        error_response(res, "Failed to save storage data", 500);
    }
}

static void handle_get_system_settings(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    json_response(res, {{"fullscreenRequired", g_fullscreenRequired}});
}

static void handle_post_system_settings(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    bool required = body.value("fullscreenRequired", false);
    if (!StorageManager::getInstance().saveSystemSettings(required)) {
        error_response(res, "Failed to persist system settings: disk verification mismatch", 500);
        return;
    }
    g_fullscreenRequired = required;
    json_response(res, {{"fullscreenRequired", g_fullscreenRequired}});
}

static void handle_rebuild_used(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    StorageManager::getInstance().rebuildUsedFlags(dsmh);
    StorageManager::getInstance().saveSubjects(dsmh);
    StorageManager::getInstance().saveQuestions(dsmh);
    json_response(res, {{"message", "Used flags rebuilt successfully"}});
}
