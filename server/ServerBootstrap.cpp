#include "ServerBootstrap.h"
#include "ServerContext.h"
#include "Diagnostics.h"
#include "RouteRegistry.h"
#include <iostream>
#include <string>
#include <ctime>
#include "PathResolver.h"
#include "Storage.h"
#include "StorageManager.h"
#include "Exam.h"

int ServerBootstrap::run(int argc, char* argv[]) {
    // Initialize PathResolver with executable path to ensure CWD-independent storage resolution
    PathResolver::init(argc > 0 ? argv[0] : "");

    // Check for CLI flags
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--reset-storage") {
            if (StorageManager::getInstance().resetToDefault()) {
                std::cout << "Storage reset to default empty state.\n";
            } else {
                std::cerr << "Failed to reset storage.\n";
            }
            return 0;
        }
        if (arg == "--test-validation") {
            return runValidationTests() ? 0 : 1;
        }
    }

    std::cout << "[STARTUP LOG] [BEGIN] LoadAllData\n";
    LoadAllData(dsl, dsmh);
    std::cout << "[STARTUP LOG] [END] LoadAllData\n";

    rebuildGlobalStudentMap();
    rebuildGlobalClassMap();

    std::cout << "[STARTUP LOG] [BEGIN] loadSystemSettings\n";
    {
        bool loadedFs = false;
        StorageManager::getInstance().loadSystemSettings(loadedFs);
        g_fullscreenRequired = loadedFs;
    }
    std::cout << "[STARTUP LOG] [END] loadSystemSettings\n";

    std::cout << "============================================================\n";
    std::cout << "Loaded System Settings:\n";
    std::cout << "- Fullscreen Supervision: " << (g_fullscreenRequired ? "ON" : "OFF") << "\n";
    std::cout << "============================================================\n";

    std::cout << "[STARTUP LOG] [BEGIN] adjustSessionsForDowntime\n";
    std::time_t startupTime = std::time(nullptr);
    adjustSessionsForDowntime(startupTime);
    std::cout << "[STARTUP LOG] [END] adjustSessionsForDowntime\n";

    httplib::Server svr;
    registerRoutes(svr);

    std::cout << "[STARTUP LOG] [BEGIN] svr.listen(0.0.0.0, 8080)\n";
    std::cout << "Server running on http://localhost:8080\n";
    bool listenSuccess = svr.listen("0.0.0.0", 8080);
    if (!listenSuccess) {
        std::cerr << "[SERVER ERROR] Cannot bind to port 8080! Port is already in use by another server instance or application.\n";
    }
    std::cout << "[STARTUP LOG] [END] svr.listen(0.0.0.0, 8080)\n";

    return 0;
}
