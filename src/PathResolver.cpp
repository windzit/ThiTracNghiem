#include "../include/PathResolver.h"
#include <iostream>

namespace fs = std::filesystem;

// Static member definition
std::string PathResolver::s_execDir = "";

void PathResolver::init(const std::string& executablePath) {
    try {
        fs::path execPath = fs::absolute(fs::path(executablePath));
        s_execDir = execPath.parent_path().string();
        std::cout << "[PathResolver] Initialized. Executable dir: " << s_execDir << "\n";
    } catch (...) {
        s_execDir = "";
        std::cerr << "[PathResolver] Warning: Could not resolve executable path. Falling back to CWD.\n";
    }
}

std::string PathResolver::getStorageDir() {
    // Priority 1: Traverse up parent directories from executable directory (supports arbitrary build depth like out/build/x64-Debug)
    if (!s_execDir.empty()) {
        fs::path cur = fs::path(s_execDir);
        for (int level = 0; level <= 5; level++) {
            fs::path candidate = cur / "storage";
            if (fs::exists(candidate) && fs::is_directory(candidate)) {
                // Verify that candidate directory actually contains storage data files or is a valid project storage
                if (fs::exists(candidate / "Classes.txt") || fs::exists(candidate / "metadata.txt") || fs::exists(candidate / "subjects.txt")) {
                    return fs::absolute(candidate).string();
                }
            }
            if (!cur.has_parent_path() || cur == cur.parent_path()) break;
            cur = cur.parent_path();
        }

        // Second pass: return any existing storage directory found walking up
        cur = fs::path(s_execDir);
        for (int level = 0; level <= 5; level++) {
            fs::path candidate = cur / "storage";
            if (fs::exists(candidate) && fs::is_directory(candidate)) {
                return fs::absolute(candidate).string();
            }
            if (!cur.has_parent_path() || cur == cur.parent_path()) break;
            cur = cur.parent_path();
        }
    }

    // Priority 2: Fallback — CWD-relative search (legacy, warn loudly)
    std::cerr << "[PathResolver] Warning: executable-relative storage not found. Falling back to CWD search.\n";
    fs::path cwdCandidates[] = {
        "storage",
        "../storage",
        "../../storage",
        "../../../storage"
    };
    for (const auto& p : cwdCandidates) {
        if (fs::exists(p) && fs::is_directory(p)) {
            return fs::absolute(p).string();
        }
    }

    // Last resort: create storage/ next to executable (or CWD if execDir unknown)
    fs::path defaultBase = s_execDir.empty() ? fs::current_path() : fs::path(s_execDir);
    fs::path defaultPath = defaultBase / "storage";
    fs::create_directories(defaultPath);
    std::cerr << "[PathResolver] Created fallback storage dir: " << defaultPath.string() << "\n";
    return fs::absolute(defaultPath).string();
}

std::string PathResolver::getFilePath(const std::string& filename) {
    fs::path dir(getStorageDir());
    return (dir / filename).string();
}
