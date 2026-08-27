#include "AdminHandler.h"
#include "../ServerContext.h"
#include "StorageManager.h"

using namespace std;

void handle_get_system_settings(const httplib::Request& req, httplib::Response& res) {
    DB_READ_LOCK;
    json_response(res, {{"fullscreenRequired", g_fullscreenRequired}});
}

void handle_post_system_settings(const httplib::Request& req, httplib::Response& res) {
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
