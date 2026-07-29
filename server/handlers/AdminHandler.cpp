#include "AdminHandler.h"
#include "../ServerContext.h"
#include "StorageManager.h"

using namespace std;

void handle_admin_save(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    bool ok = StorageManager::getInstance().saveAllData(dsl, dsmh);
    if (ok) {
        json_response(res, {{"message", "All storage data saved successfully"}});
    } else {
        error_response(res, "Failed to save storage data", 500);
    }
}

void handle_get_system_settings(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_READ_LOCK;
    json_response(res, {{"fullscreenRequired", g_fullscreenRequired}});
}

void handle_post_system_settings(const httplib::Request& req, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    json body;
    try { body = json::parse(req.body); }
    catch (...) { error_response(res, "Invalid JSON", 400); return; }

    bool required = body.value("fullscreenRequired", false);
    if (!StorageManager::getInstance().saveSystemSettings(required)) {
        error_response(res, "Failed to persist system settings: disk verification mismatch", 500);
        return;
    }
    g_fullscreenRequired = required;
    json_response(res, {{"fullscreenRequired", g_fullscreenRequired}});
}

void handle_rebuild_used(const httplib::Request&, httplib::Response& res) {
    set_cors_headers(res);
    DB_WRITE_LOCK;
    StorageManager::getInstance().rebuildUsedFlags(dsmh);
    StorageManager::getInstance().saveSubjects(dsmh);
    StorageManager::getInstance().saveQuestions(dsmh);
    json_response(res, {{"message", "Used flags rebuilt successfully"}});
}
