#include "StudentHandler.h"
#include "../ServerContext.h"
#include "StorageManager.h"
#include "IndexManager.h"
#include "StorageValidator.h"
#include "StringNormalizer.h"

using namespace std;

void handle_get_students(const httplib::Request& req, httplib::Response& res) {
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

void handle_get_student_by_id(const httplib::Request& req, httplib::Response& res) {
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

void handle_create_student(const httplib::Request& req, httplib::Response& res) {
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
    if (findStudentGlobal(masv, nullptr)) { error_response(res, "Mã sinh viên đã tồn tại trong hệ thống.", 409); return; }
    SinhVien sv; sv.MASV = masv; sv.HO = ho; sv.TEN = ten; sv.PHAI = phai; sv.passsword = password;
    StringNormalizer::normalizeStudent(sv);

    string errReason;
    if (!StorageValidator::validateStudent(sv, malop, errReason)) {
        error_response(res, "Validation failed: " + errReason, 400);
        return;
    }

    if (lop->dssinhvien.insert(sv)) {
        int64_t outOffset = -1;
        StorageManager::getInstance().appendStudent(sv, malop, outOffset);
        json_response(res, {{"masv",sv.MASV},{"ho",sv.HO},{"ten",sv.TEN},{"phai",sv.PHAI},{"malop",malop},{"examCount",0}}, 201);
    } else { error_response(res, "Failed to create student", 500); }
}

void handle_update_student(const httplib::Request& req, httplib::Response& res) {
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
        int64_t offset = -1;
        if (IndexManager::getInstance().getStudentOffset(masv, offset)) {
            StorageManager::getInstance().writeStudentAt(offset, newData, foundLop->MALOP);
        }
        sv->dsdiemthi.load(sv->MASV);
        int examCount = sv->dsdiemthi.count();
        json_response(res, {{"masv",sv->MASV},{"ho",sv->HO},{"ten",sv->TEN},{"phai",sv->PHAI},{"malop",foundLop->MALOP},{"examCount",examCount}});
    } else { error_response(res, "Failed to update student", 500); }
}

void handle_delete_student(const httplib::Request& req, httplib::Response& res) {
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

    sv->dsdiemthi.load(masv);
    if (!sv->dsdiemthi.empty()) {
        custom_json_response(res, {
            {"success", false},
            {"error", "Unprocessable Entity"},
            {"message", "Sinh viên đã có kết quả thi, không thể xóa."}
        }, 422);
        return;
    }

    int64_t offset = -1;
    bool hasOffset = IndexManager::getInstance().getStudentOffset(masv, offset);

    if (foundLop->dssinhvien.remove(masv)) {
        if (hasOffset) {
            StorageManager::getInstance().markStudentStatusAt(offset, STATUS_DELETED);
            IndexManager::getInstance().removeStudentOffset(masv);
        }
        res.status = 204;
    } else { error_response(res, "Failed to delete student", 500); }
}

void handle_bulk_delete_students(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    json masvList = body.is_array() ? body : body.value("masv", json::array());
    if (masvList.empty()) {
        error_response(res, "No student IDs provided", 400); return;
    }

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
            int64_t offset = -1;
            bool hasOffset = IndexManager::getInstance().getStudentOffset(masv, offset);

            if (foundLop->dssinhvien.remove(masv)) {
                if (hasOffset) {
                    StorageManager::getInstance().markStudentStatusAt(offset, STATUS_DELETED);
                    IndexManager::getInstance().removeStudentOffset(masv);
                }
                deletedCount++;
            }
        }
    }

    json_response(res, {{"deletedCount", deletedCount}});
}

