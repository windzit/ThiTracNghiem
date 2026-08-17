#include "SubjectHandler.h"
#include "../ServerContext.h"
#include "StorageManager.h"
#include "IndexManager.h"
#include "StorageValidator.h"
#include "StringNormalizer.h"
#include <cstring>

using namespace std;

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

void handle_get_subjects(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    json arr = json::array();
    collectSubjectsForAPI(dsmh.getRoot(), arr);
    json_response(res, arr);
}

void handle_get_subject_by_id(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string mamh = get_path_param(req, "id");
    NodeMH* node = find_subject_smart(mamh);
    if (!node) { error_response(res, "Subject not found", 404); return; }

    json questions = json::array();
    dsCHT* q = node->data.dsCauHoi.getRoot();
    while (q) {
        questions.push_back({{"id",q->cauhoi.ID},{"noidung",q->cauhoi.NOIDUNG},
            {"A",q->cauhoi.A},{"B",q->cauhoi.B},{"C",q->cauhoi.C},{"D",q->cauhoi.D},
            {"dapan",string(1,q->cauhoi.DAPAN_DUNG)},{"used",q->cauhoi.used},{"deleted",q->cauhoi.deleted}});
        q = q->next;
    }
    json_response(res, {{"mamh",node->data.MAMH},{"tenmh",node->data.TENMH},
        {"used",node->data.used},{"questions",questions}});
}

void handle_create_subject(const httplib::Request& req, httplib::Response& res) {
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
        int64_t outOffset = -1;
        StorageManager::getInstance().appendSubject(mh, outOffset);
        json_response(res, {{"mamh",mh.MAMH},{"tenmh",mh.TENMH},{"used",false}}, 201);
    } else {
        error_response(res, "Failed to create subject", 500);
    }
}

void handle_update_subject(const httplib::Request& req, httplib::Response& res) {
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
        int64_t offset = -1;
        if (IndexManager::getInstance().getSubjectOffset(mamh, offset)) {
            StorageManager::getInstance().writeSubjectAt(offset, node->data);
        }
        json_response(res, {{"mamh",node->data.MAMH},{"tenmh",node->data.TENMH},{"used",node->data.used}});
    } else {
        error_response(res, "Failed to update subject", 500);
    }
}

void handle_delete_subject(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string mamh = get_path_param(req, "id");
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) {
        error_response(res, "Subject not found", 404); return;
    }

    int64_t offset = -1;
    bool hasOffset = IndexManager::getInstance().getSubjectOffset(mamh, offset);

    // Mark questions of this subject as STATUS_DELETED in questions.txt and update question.idx
    for (dsCHT* qNode = node->data.dsCauHoi.getRoot(); qNode; qNode = qNode->next) {
        int qId = qNode->cauhoi.ID;
        int64_t qOffset = -1;
        if (IndexManager::getInstance().getQuestionOffset(qId, qOffset)) {
            StorageManager::getInstance().markQuestionStatusAt(qOffset, STATUS_DELETED);
            IndexManager::getInstance().removeQuestionOffset(qId);
        }
    }

    if (dsmh.remove(mamh.c_str())) {
        if (hasOffset) {
            StorageManager::getInstance().markSubjectStatusAt(offset, STATUS_DELETED);
            IndexManager::getInstance().removeSubjectOffset(mamh);
        }
        res.status = 204;
    } else {
        error_response(res, "Failed to delete subject (may have used questions)", 422);
    }
}

