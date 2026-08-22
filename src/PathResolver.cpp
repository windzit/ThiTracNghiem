#include "../include/PathResolver.h"
#include <iostream>

namespace fs = std::filesystem;

// Static members
std::string PathResolver::s_execDir = "";
std::string PathResolver::s_storageDir = "";
std::string PathResolver::s_dataDir = "";
std::string PathResolver::s_indexDir = "";

// ============================================================
// CHECK STORAGE
// ============================================================

static bool isValidStorage(const fs::path& storage)
{
    if (!fs::exists(storage) || !fs::is_directory(storage))
        return false;

    fs::path dataDir = storage / "data";
    return (fs::exists(dataDir) && fs::is_directory(dataDir));
}

// ============================================================
// RESOLVE INTERNAL
// ============================================================

static std::string resolveStorageLocation(const std::string& execDir)
{
    // PRIORITY 1: Tìm storage từ thư mục chứa executable đi ngược lên
    if (!execDir.empty())
    {
        fs::path current = execDir;
        while (true)
        {
            fs::path storage = current / "storage";
            std::string pathStr = storage.string();

            bool isBuildFolder =
                pathStr.find("out\\build") != std::string::npos ||
                pathStr.find("out/build") != std::string::npos;

            if (!isBuildFolder && isValidStorage(storage))
            {
                return fs::absolute(storage).string();
            }

            if (!current.has_parent_path() || current == current.parent_path())
            {
                break;
            }
            current = current.parent_path();
        }
    }

    // PRIORITY 2: Tìm storage từ Current Working Directory
    fs::path cwd = fs::current_path();
    fs::path candidates[] =
    {
        cwd / "storage",
        cwd / "../storage",
        cwd / "../../storage",
        cwd / "../../../storage"
    };

    for (const auto& storage : candidates)
    {
        if (isValidStorage(storage))
        {
            return fs::absolute(storage).string();
        }
    }

    // PRIORITY 3: Không tìm thấy -> Tạo storage mới
    fs::path base = execDir.empty() ? fs::current_path() : fs::path(execDir);
    fs::path storage = base / "storage";
    fs::create_directories(storage / "data");
    fs::create_directories(storage / "indexes");

    std::cerr << "[PathResolver] Created storage at: " << storage << '\n';
    return fs::absolute(storage).string();
}

// ============================================================
// INIT
// ============================================================

void PathResolver::init(const std::string& executablePath)
{
    try
    {
        fs::path execPath = fs::absolute(executablePath);
        s_execDir = execPath.parent_path().string();
        std::cout << "[PathResolver] Executable dir: " << s_execDir << '\n';
    }
    catch (...)
    {
        s_execDir = "";
        std::cerr << "[PathResolver] Cannot resolve executable path.\n";
    }

    // Pre-cache all directories once at startup -> O(1) subsequent calls
    s_storageDir = resolveStorageLocation(s_execDir);
    s_dataDir = (fs::path(s_storageDir) / "data").string();
    s_indexDir = (fs::path(s_storageDir) / "indexes").string();

    fs::create_directories(s_dataDir);
    fs::create_directories(s_indexDir);
}

// ============================================================
// DIRECTORY GETTERS (O(1) Cached)
// ============================================================

std::string PathResolver::getStorageDir()
{
    if (s_storageDir.empty())
    {
        s_storageDir = resolveStorageLocation(s_execDir);
    }
    return s_storageDir;
}

std::string PathResolver::getDataDir()
{
    if (s_dataDir.empty())
    {
        s_dataDir = (fs::path(getStorageDir()) / "data").string();
        fs::create_directories(s_dataDir);
    }
    return s_dataDir;
}

std::string PathResolver::getIndexDir()
{
    if (s_indexDir.empty())
    {
        s_indexDir = (fs::path(getStorageDir()) / "indexes").string();
        fs::create_directories(s_indexDir);
    }
    return s_indexDir;
}

// ============================================================
// FILE PATH GETTERS (O(1) In-Memory String Assembly)
// ============================================================

std::string PathResolver::getFilePath(const std::string& filename)
{
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".idx")
    {
        return getIndexPath(filename);
    }
    return (fs::path(getDataDir()) / filename).string();
}

std::string PathResolver::getIndexPath(const std::string& filename)
{
    return (fs::path(getIndexDir()) / filename).string();
}