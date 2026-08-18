#pragma once

#include <string>
#include <shared_mutex>
#include "httplib.h"
#include "nlohmann/json.hpp"
#include "Class.h"
#include "Subject.h"
#include "Student.h"

using json = nlohmann::json;

// Global shared state
extern Class dsl;
extern Subject dsmh;
extern std::shared_mutex g_dbMutex;
extern bool g_fullscreenRequired;

// Lock macros
#define DB_READ_LOCK  std::shared_lock<std::shared_mutex> _rl(g_dbMutex)
#define DB_WRITE_LOCK std::unique_lock<std::shared_mutex> _wl(g_dbMutex)

// Helper response functions
void json_response(httplib::Response& res, const json& data, int status = 200);
void error_response(httplib::Response& res, const std::string& msg, int status);
void custom_json_response(httplib::Response& res, const json& body, int status = 200);
std::string get_path_param(const httplib::Request& req, const std::string& name);

// Global domain helpers
struct StudentLocation {
    SinhVien* sv = nullptr;
    Lop* lop = nullptr;
};

void registerStudentGlobal(const std::string& masv, SinhVien* sv, Lop* lop);
void unregisterStudentGlobal(const std::string& masv);
void rebuildGlobalStudentMap();

void registerClassGlobal(const std::string& malop, Lop* lop);
void unregisterClassGlobal(const std::string& malop);
void rebuildGlobalClassMap();

SinhVien* findStudentGlobal(const std::string& masv, Lop** outLop = nullptr);
Lop* findClassGlobal(const std::string& malop);
NodeMH* find_subject_smart(const std::string& mamh);
