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
        {"questionCount", node->data.dsCauHoi.activeSize()}
    });
    collectSubjectsForAPI(node->right, arr);
}

void handle_get_subjects(const httplib::Request&, httplib::Response& res) {
    DB_READ_LOCK;
    json arr = json::array();
    collectSubjectsForAPI(dsmh.getRoot(), arr);
    json_response(res, arr);
}

void handle_get_subject_by_id(const httplib::Request& req, httplib::Response& res) {
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
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }
    string mamh = body.value("mamh", "");
    string tenmh = body.value("tenmh", "");
    if (mamh.empty() || tenmh.empty()) {
        error_response(res, "mamh and tenmh are required", 400); return;
    }
    if (dsmh.find(mamh.c_str())) {
        error_response(res, "Subject already exists", 409); return;
    }
    MonHoc mh;
    strncpy(mh.MAMH, mamh.c_str(), sizeof(mh.MAMH) - 1);
    mh.MAMH[sizeof(mh.MAMH) - 1] = '\0';
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
    strncpy(candidateMh.MAMH, mamh.c_str(), sizeof(candidateMh.MAMH) - 1);
    candidateMh.MAMH[sizeof(candidateMh.MAMH) - 1] = '\0';
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
    DB_WRITE_LOCK;
    string mamh = get_path_param(req, "id");
    NodeMH* node = dsmh.find(mamh.c_str());
    if (!node) {
        error_response(res, "Subject not found", 404); return;
    }

    if (node->data.used) {
        custom_json_response(res, {
            {"success", false},
            {"error", "Unprocessable Entity"},
            {"message", "Môn học đã có sinh viên thi, không thể xóa."}
        }, 422);
        return;
    }

    int64_t offset = -1;
    bool hasOffset = IndexManager::getInstance().getSubjectOffset(mamh, offset);

    // Save questions list pointers before node removal
    DArray<int> questionIds;
    for (dsCHT* qNode = node->data.dsCauHoi.getRoot(); qNode; qNode = qNode->next) {
        questionIds.push_back(qNode->cauhoi.ID);
    }

    // Perform RAM removal FIRST for transaction atomicity
    if (dsmh.remove(mamh.c_str())) {
        if (hasOffset) {
            StorageManager::getInstance().markSubjectStatusAt(offset, STATUS_DELETED);
            IndexManager::getInstance().removeSubjectOffset(mamh);
        }
        // Mark questions of this subject as STATUS_DELETED in questions.txt and clean up indexes
        for (size_t i = 0; i < questionIds.size(); i++) {
            int qId = questionIds[i];
            int64_t qOffset = -1;
            if (IndexManager::getInstance().getQuestionOffset(qId, qOffset)) {
                StorageManager::getInstance().markQuestionStatusAt(qOffset, STATUS_DELETED);
                IndexManager::getInstance().removeQuestionSubject(qId);
                IndexManager::getInstance().removeQuestionOffset(qId);
            }
        }
        res.status = 204;
    } else {
        error_response(res, "Failed to delete subject (may have used questions)", 422);
    }
}

