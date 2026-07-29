#include "ExamHandler.h"
#include "../ServerContext.h"
#include <ctime>
#include <cctype>
#include <cstring>
#include "Exam.h"
#include "Utils.h"
#include "StorageManager.h"
#include "DArray.h"
#include "Utility/Swap.h"
#include "Utility/Random.h"

using namespace std;

void handle_exam_start(const httplib::Request& req, httplib::Response& res) {
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

    SinhVien* sv = nullptr;
    dsLop* root = dsl.getRoot();
    for (int i = 0; i < root->n && !sv; i++)
        if (root->dslop[i]) sv = root->dslop[i]->dssinhvien.find(masv);
    if (!sv) { error_response(res, "Student not found", 404); return; }

    if (sv->dsdiemthi.find(mamh.c_str())) {
        error_response(res, "Ban da thi mon nay roi", 409); return;
    }

    NodeMH* node = find_subject_smart(mamh);
    if (!node) { error_response(res, "Subject not found", 404); return; }

    DArray<dsCHT*> all;
    dsCHT* curQ = node->data.dsCauHoi.getRoot();
    while (curQ) {
        if (!curQ->cauhoi.deleted) {
            all.push_back(curQ);
        }
        curQ = curQ->next;
    }
    if (all.size() < socau) {
        error_response(res, "Not enough questions", 400); return;
    }

    Utility::Random::Seed();
    for (int i = all.size()-1; i > 0; i--) {
        int j = Utility::Random::RandomInt(0, i);
        Utility::Swap(all[i], all[j]);
    }

    ExamSession session;
    session.MASV = masv;
    session.MAMH = mamh;
    session.thoiGianBatDau = std::time(nullptr);
    if (thoigianSec > 0 && thoigianSec < 60) {
        session.tongThoiGianPhut = 1;
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

void handle_exam_resume(const httplib::Request& req, httplib::Response& res) {
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

void handle_exam_answer(const httplib::Request& req, httplib::Response& res) {
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

void handle_exam_submit(const httplib::Request& req, httplib::Response& res) {
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

    if (sv->dsdiemthi.find(mamh.c_str())) {
        removeExamSession(masv);
        error_response(res, "Bai thi nay da duoc nop truoc do", 409); return;
    }

    ExamSession session;
    if (!loadExamSession(masv, session) || !session.in_progress || session.MAMH != mamh) {
        error_response(res, "Bai thi nay da duoc nop truoc do hoac phien thi khong ton tai", 409); return;
    }

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

    StorageManager::getInstance().appendExamHistory(session, diem);
    removeExamSession(masv);

    StorageManager::getInstance().rebuildUsedFlags(dsmh);
    StorageManager::getInstance().saveSubjects(dsmh);
    StorageManager::getInstance().saveQuestions(dsmh);

    json_response(res, {{"soDung",soDung},{"total",total},
        {"diem",diem},{"saved",saved}});
}
