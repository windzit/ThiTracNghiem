#include "ServerContext.h"

Class dsl;
Subject dsmh;
std::shared_mutex g_dbMutex;
bool g_fullscreenRequired = false;

void json_response(httplib::Response& res, const json& data, int status) {
    json body = {{"success", true}, {"data", data}};
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

void error_response(httplib::Response& res, const std::string& msg, int status) {
    json body = {{"success", false}, {"message", msg}};
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

void custom_json_response(httplib::Response& res, const json& body, int status) {
    res.status = status;
    res.set_content(body.dump(), "application/json; charset=utf-8");
}

std::string get_path_param(const httplib::Request& req, const std::string& name) {
    auto it = req.path_params.find(name);
    if (it != req.path_params.end()) {
        return it->second;
    }
    return "";
}

#include "HashTable.h"

static HashTable<std::string, StudentLocation> g_studentMap;

void registerStudentGlobal(const std::string& masv, SinhVien* sv, Lop* lop) {
    if (!masv.empty() && sv && lop) {
        g_studentMap.insert(masv, StudentLocation{sv, lop});
    }
}

void unregisterStudentGlobal(const std::string& masv) {
    if (!masv.empty()) {
        g_studentMap.remove(masv);
    }
}

void rebuildGlobalStudentMap() {
    g_studentMap.clear();
    dsLop* root = dsl.getRoot();
    if (!root) return;
    for (int i = 0; i < root->n; i++) {
        Lop* lop = root->dslop[i];
        if (!lop) continue;
        dsSinhVien* cur = lop->dssinhvien.getRoot();
        while (cur) {
            g_studentMap.insert(cur->sinhvien.MASV, StudentLocation{&(cur->sinhvien), lop});
            cur = cur->next;
        }
    }
}

SinhVien* findStudentGlobal(const std::string& masv, Lop** outLop) {
    StudentLocation* loc = g_studentMap.find(masv);
    if (loc && loc->sv) {
        if (outLop) *outLop = loc->lop;
        return loc->sv;
    }
    return nullptr;
}

static HashTable<std::string, Lop*> g_classMap;

void registerClassGlobal(const std::string& malop, Lop* lop) {
    if (!malop.empty() && lop) {
        g_classMap.insert(malop, lop);
    }
}

void unregisterClassGlobal(const std::string& malop) {
    if (!malop.empty()) {
        g_classMap.remove(malop);
    }
}

void rebuildGlobalClassMap() {
    g_classMap.clear();
    dsLop* root = dsl.getRoot();
    if (!root) return;
    for (int i = 0; i < root->n; i++) {
        if (root->dslop[i]) {
            g_classMap.insert(root->dslop[i]->MALOP, root->dslop[i]);
        }
    }
}

Lop* findClassGlobal(const std::string& malop) {
    Lop** loc = g_classMap.find(malop);
    return (loc && *loc) ? *loc : nullptr;
}

NodeMH* find_subject_smart(const std::string& mamh) {
    return dsmh.find(mamh.c_str());
}

