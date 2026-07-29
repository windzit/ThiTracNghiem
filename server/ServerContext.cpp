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

void set_cors_headers(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "http://localhost:5173");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

std::string get_path_param(const httplib::Request& req, const std::string& name) {
    auto it = req.path_params.find(name);
    if (it != req.path_params.end()) {
        return it->second;
    }
    return "";
}

SinhVien* findStudentGlobal(const std::string& masv, Lop** outLop) {
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

NodeMH* find_subject_smart(const std::string& mamh) {
    return dsmh.find(mamh.c_str());
}
