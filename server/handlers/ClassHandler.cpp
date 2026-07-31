#include "ClassHandler.h"
#include "../ServerContext.h"
#include "StorageManager.h"
#include "IndexManager.h"
#include "StorageValidator.h"
#include "StringNormalizer.h"
#include <iostream>

using namespace std;

void handle_get_classes(const httplib::Request&, httplib::Response& res) {
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

void handle_get_class_by_id(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    string malop = get_path_param(req, "id");
    Lop* lop = dsl.find(malop);
    if (!lop) { error_response(res, "Class not found", 404); return; }
    json_response(res, {{"malop",lop->MALOP},{"tenlop",lop->TENLOP},
        {"siso",lop->dssinhvien.size()}});
}

void handle_create_class(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    string malop = body.value("malop", "");
    string tenlop = body.value("tenlop", "");

    if (malop.empty() || tenlop.empty()) {
        error_response(res, "malop and tenlop are required", 400);
        return;
    }

    if (malop.length() > 15) {
        error_response(res, "malop must be at most 15 characters", 400);
        return;
    }

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
        int64_t outOffset = -1;
        StorageManager::getInstance().appendClass(*lop, outOffset);
        json_response(res, {{"malop", lop->MALOP}, {"tenlop", lop->TENLOP}, {"siso", 0}}, 201);
    } else {
        error_response(res, "Failed to create class", 500);
    }
}

void handle_update_class(const httplib::Request& req, httplib::Response& res) {
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

    if (!dsl.update(malop, candidateLop.TENLOP)) {
        error_response(res, "Failed to update class", 500);
        return;
    }

    Lop* updated = dsl.find(malop);
    if (updated) {
        int64_t offset = -1;
        if (IndexManager::getInstance().getClassOffset(malop, offset)) {
            StorageManager::getInstance().writeClassAt(offset, *updated);
        }
    }
    json_response(res, {{"malop", updated->MALOP}, {"tenlop", updated->TENLOP},
        {"siso", updated->dssinhvien.size()}});
}

void handle_delete_class(const httplib::Request& req, httplib::Response& res) {
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

    int64_t offset = -1;
    bool hasOffset = IndexManager::getInstance().getClassOffset(malop, offset);

    if (dsl.remove(malop)) {
        if (hasOffset) {
            StorageManager::getInstance().markClassStatusAt(offset, STATUS_DELETED);
            IndexManager::getInstance().removeClassOffset(malop);
        }
        res.status = 204;
    } else {
        error_response(res, "Failed to delete class", 500);
    }
}

