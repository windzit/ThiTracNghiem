#include "QuestionHandler.h"
#include "../ServerContext.h"
#include <set>
#include <functional>
#include "StorageManager.h"
#include "IndexManager.h"
#include "StorageValidator.h"
#include "StringNormalizer.h"

using namespace std;

void handle_create_question(const httplib::Request& req, httplib::Response& res) {
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
    CauHoi ch; ch.ID = 1;
    ch.NOIDUNG = noidung; ch.A = a; ch.B = b; ch.C = c; ch.D = d;
    ch.DAPAN_DUNG = dapan.empty() ? '\0' : dapan[0];
    ch.used = false;
    StringNormalizer::normalizeQuestion(ch);

    string errReason;
    if (!StorageValidator::validateQuestion(ch, mamh, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    // Check duplicate question NOIDUNG within the same subject after validation
    dsCHT* checkPtr = node->data.dsCauHoi.getRoot();
    while (checkPtr) {
        if (!checkPtr->cauhoi.deleted && checkPtr->cauhoi.NOIDUNG == ch.NOIDUNG) {
            error_response(res, "Nội dung câu hỏi đã tồn tại trong môn học này.", 409);
            return;
        }
        checkPtr = checkPtr->next;
    }

    ch.ID = 0;

    if (node->data.dsCauHoi.insert(ch, true)) {
        int64_t outOffset = -1;
        StorageManager::getInstance().appendQuestion(ch, mamh, outOffset);
        json_response(res, {{"id",ch.ID},{"mamh",mamh},{"noidung",ch.NOIDUNG},
            {"a",ch.A},{"b",ch.B},{"c",ch.C},{"d",ch.D},{"dapan",dapan},{"used",false},{"deleted",false}}, 201);
    } else { error_response(res, "Failed to create question", 500); }
}

void handle_update_question(const httplib::Request& req, httplib::Response& res) {
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

    // Check duplicate question NOIDUNG within the same subject after validation (excluding self)
    dsCHT* checkPtr = node->data.dsCauHoi.getRoot();
    while (checkPtr) {
        if (checkPtr->cauhoi.ID != id && !checkPtr->cauhoi.deleted && checkPtr->cauhoi.NOIDUNG == newData.NOIDUNG) {
            error_response(res, "Nội dung câu hỏi đã tồn tại trong môn học này.", 409);
            return;
        }
        checkPtr = checkPtr->next;
    }

    if (node->data.dsCauHoi.update(id, newData)) {
        int64_t offset = -1;
        if (IndexManager::getInstance().getQuestionOffset(id, offset)) {
            StorageManager::getInstance().writeQuestionAt(offset, newData, mamh);
        }
        json_response(res, {{"id",id},{"mamh",mamh},{"noidung",newData.NOIDUNG},
            {"a",newData.A},{"b",newData.B},{"c",newData.C},{"d",newData.D},
            {"dapan",dapan},{"used",found->cauhoi.used},{"deleted",found->cauhoi.deleted}});
    } else { error_response(res, "Failed to update question (may be used)", 422); }
}

void handle_delete_question(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string idStr = get_path_param(req, "id");
    if (idStr.empty()) { error_response(res, "id required", 400); return; }
    int id = std::stoi(idStr);
    string mamh = req.get_param_value("mamh");

    NodeMH* foundNode = nullptr;
    dsCHT* foundQNode = nullptr;

    if (!mamh.empty()) {
        foundNode = dsmh.find(mamh.c_str());
        if (foundNode) {
            foundQNode = foundNode->data.dsCauHoi.find(id);
        }
    } else {
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

    string mode;
    char targetStatus = STATUS_DELETED;

    if (foundQNode->cauhoi.used) {
        if (!foundNode->data.dsCauHoi.setDeleted(id)) {
            error_response(res, "Failed to soft delete question", 422); return;
        }
        mode = "soft";
        targetStatus = STATUS_SOFT_DELETED;
    } else {
        if (!foundNode->data.dsCauHoi.removeNode(id)) {
            error_response(res, "Failed to delete question", 422); return;
        }
        mode = "hard";
        targetStatus = STATUS_DELETED;
    }

    int64_t offset = -1;
    bool hasOffset = IndexManager::getInstance().getQuestionOffset(id, offset);
    if (hasOffset) {
        StorageManager::getInstance().markQuestionStatusAt(offset, targetStatus);
        if (targetStatus == STATUS_DELETED) {
            IndexManager::getInstance().removeQuestionOffset(id);
        }
    }

    json_response(res, {{"id", id}, {"mamh", mamh}, {"mode", mode}});
}

void handle_bulk_delete_questions(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    json items = body.is_array() ? body : body.value("items", json::array());
    if (items.empty()) {
        error_response(res, "No items provided for bulk delete", 400); return;
    }

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
    }

    set<string> affectedSubjects;
    int softDeletedCount = 0;
    int hardDeletedCount = 0;

    for (auto& item : items) {
        int id = item.value("id", 0);
        string mamh = item.value("mamh", "");
        NodeMH* node = dsmh.find(mamh.c_str());
        if (node) {
            dsCHT* q = node->data.dsCauHoi.find(id);
            if (!q || q->cauhoi.deleted) continue;

            int64_t offset = -1;
            bool hasOffset = IndexManager::getInstance().getQuestionOffset(id, offset);

            if (q->cauhoi.used) {
                if (node->data.dsCauHoi.setDeleted(id)) {
                    if (hasOffset) {
                        StorageManager::getInstance().markQuestionStatusAt(offset, STATUS_SOFT_DELETED);
                    }
                    softDeletedCount++;
                    affectedSubjects.insert(mamh);
                }
            } else {
                if (node->data.dsCauHoi.removeNode(id)) {
                    if (hasOffset) {
                        StorageManager::getInstance().markQuestionStatusAt(offset, STATUS_DELETED);
                        IndexManager::getInstance().removeQuestionOffset(id);
                    }
                    hardDeletedCount++;
                    affectedSubjects.insert(mamh);
                }
            }
        }
    }

    int totalDeleted = softDeletedCount + hardDeletedCount;
    json_response(res, {
        {"success", true},
        {"deletedCount", totalDeleted},
        {"softDeletedCount", softDeletedCount},
        {"hardDeletedCount", hardDeletedCount}
    });
}

void handle_restore_question(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    string idStr = get_path_param(req, "id");
    if (idStr.empty()) { error_response(res, "id required", 400); return; }
    int id = std::stoi(idStr);
    string mamh = req.get_param_value("mamh");

    NodeMH* foundNode = nullptr;
    dsCHT* foundQNode = nullptr;

    if (!mamh.empty()) {
        foundNode = dsmh.find(mamh.c_str());
        if (foundNode) {
            foundQNode = foundNode->data.dsCauHoi.find(id);
        }
    } else {
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
    if (!foundQNode->cauhoi.deleted) { error_response(res, "Question is already active", 422); return; }

    // Check duplicate question NOIDUNG within the same subject among active questions
    dsCHT* checkPtr = foundNode->data.dsCauHoi.getRoot();
    while (checkPtr) {
        if (checkPtr->cauhoi.ID != id && !checkPtr->cauhoi.deleted && checkPtr->cauhoi.NOIDUNG == foundQNode->cauhoi.NOIDUNG) {
            error_response(res, "Nội dung câu hỏi này trùng với một câu hỏi đang hoạt động trong môn học.", 409);
            return;
        }
        checkPtr = checkPtr->next;
    }

    if (!foundNode->data.dsCauHoi.restoreDeleted(id)) {
        error_response(res, "Failed to restore question", 500); return;
    }

    int64_t offset = -1;
    bool hasOffset = IndexManager::getInstance().getQuestionOffset(id, offset);
    if (hasOffset) {
        StorageManager::getInstance().markQuestionStatusAt(offset, STATUS_ACTIVE);
    }

    json_response(res, {{"id", id}, {"mamh", mamh}, {"success", true}, {"deleted", false}});
}

