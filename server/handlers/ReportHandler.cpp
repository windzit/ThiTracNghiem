#include "ReportHandler.h"
#include "../ServerContext.h"
#include <fstream>
#include "StorageManager.h"
#include "IndexManager.h"
#include "PathResolver.h"
#include "Utils.h"

using namespace std;

static void collectMAMHNames(NodeMH* node, DArray<string>& mamhs) {
    if (!node) return;
    collectMAMHNames(node->left, mamhs);
    if (node->data.MAMH[0] != '\0') {
        mamhs.push_back(string(node->data.MAMH, strnlen(node->data.MAMH, 15)));
    }
    collectMAMHNames(node->right, mamhs);
}

void handle_report_exam(const httplib::Request& req, httplib::Response& res) {
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

    DArray<int> historyQIds;
    DArray<string> historyAnswers;
    bool foundHistory = false;

    DArray<int64_t> offsets;
    if (IndexManager::getInstance().getHistoryOffsets(masv, offsets)) {
        string historyPath = PathResolver::getFilePath("exam_history.txt");
        ifstream histFile(historyPath, std::ios::in | std::ios::binary);
        if (histFile.is_open()) {
            for (int i = 0; i < offsets.size(); i++) {
                histFile.seekg(offsets[i]);
                string line;
                if (getline(histFile, line)) {
                    while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ')) line.pop_back();
                    DArray<string> tokens = split(line, '|');
                    if (tokens.size() >= 6) {
                        string hMasv = trim(tokens[0]);
                        string hMamh = trim(tokens[1]);
                        if (hMasv == masv && hMamh == mamh) {
                            foundHistory = true;
                            historyQIds.clear();
                            historyAnswers.clear();

                            DArray<string> qIdTokens = split(tokens[4], ',');
                            for (int qIdx = 0; qIdx < qIdTokens.size(); qIdx++) {
                                string qidStr = qIdTokens[qIdx];
                                if (!trim(qidStr).empty()) {
                                    historyQIds.push_back(stoi(trim(qidStr)));
                                }
                            }

                            DArray<string> ansTokens = split(tokens[5], ',');
                            for (int aIdx = 0; aIdx < ansTokens.size(); aIdx++) {
                                string ansStr = ansTokens[aIdx];
                                string a = trim(ansStr);
                                if (a == "-") a = "";
                                historyAnswers.push_back(a);
                            }
                            break;
                        }
                    }
                }
            }
            histFile.close();
        }
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
        dsCHT* curQ = node->data.dsCauHoi.getRoot();
        while (curQ) {
            if (!curQ->cauhoi.deleted) {
                questions.push_back({
                    {"id", curQ->cauhoi.ID},
                    {"noidung", curQ->cauhoi.NOIDUNG},
                    {"A", curQ->cauhoi.A},
                    {"B", curQ->cauhoi.B},
                    {"C", curQ->cauhoi.C},
                    {"D", curQ->cauhoi.D},
                    {"dapan", string(1, curQ->cauhoi.DAPAN_DUNG)},
                    {"selectedAnswer", ""}
                });
            }
            curQ = curQ->next;
        }
    }

    json_response(res, {
        {"masv",sv->MASV},{"ho",sv->HO},{"ten",sv->TEN},{"phai",sv->PHAI},
        {"mamh",node->data.MAMH},{"tenmh",node->data.TENMH},
        {"diem",diem->diemthi.DIEM},{"questions",questions}
    });
}

void handle_report_scoreboard(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string malop = req.get_param_value("malop");
    if (malop.empty()) { error_response(res, "malop required", 400); return; }
    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }

    DArray<string> allMAMH;
    collectMAMHNames(dsmh.getRoot(), allMAMH);

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

void handle_delete_score(const httplib::Request& req, httplib::Response& res) {
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
        json_response(res, {{"message","Score deleted successfully"},{"masv",masv},{"mamh",mamh}});
    } else {
        error_response(res, "Score not found for this subject", 404);
    }
}
