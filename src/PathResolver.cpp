#include "../include/PathResolver.h"
#include <iostream>

namespace fs = std::filesystem;

// Static member
std::string PathResolver::s_execDir = "";


// ============================================================
// INIT
// ============================================================

void PathResolver::init(const std::string& executablePath)
{
    try
    {
        fs::path execPath = fs::absolute(executablePath);

        // Lấy thư mục chứa file executable
        s_execDir = execPath.parent_path().string();

        std::cout
            << "[PathResolver] Executable dir: "
            << s_execDir << '\n';
    }
    catch (...)
    {
        s_execDir = "";

        std::cerr
            << "[PathResolver] Cannot resolve executable path.\n";
    }
}


// ============================================================
// CHECK STORAGE
// ============================================================

bool isValidStorage(const fs::path& storage)
{
    // storage phải tồn tại
    if (!fs::exists(storage))
        return false;

    // storage phải là thư mục
    if (!fs::is_directory(storage))
        return false;

    // storage phải có data/
    fs::path dataDir = storage / "data";

    if (!fs::exists(dataDir))
        return false;

    if (!fs::is_directory(dataDir))
        return false;

    return true;
}


// ============================================================
// GET STORAGE DIRECTORY
// ============================================================

std::string PathResolver::getStorageDir()
{
    // --------------------------------------------------------
    // PRIORITY 1
    // Tìm storage từ thư mục chứa executable
    // --------------------------------------------------------

    if (!s_execDir.empty())
    {
        fs::path current = s_execDir;

        while (true)
        {
            fs::path storage = current / "storage";

            // Không lấy storage nằm bên trong out/build
            std::string path = storage.string();

            bool isBuildFolder =
                path.find("out\\build") != std::string::npos ||
                path.find("out/build") != std::string::npos;

            if (!isBuildFolder && isValidStorage(storage))
            {
                return fs::absolute(storage).string();
            }

            // Nếu đã tới thư mục gốc thì dừng
            if (!current.has_parent_path() ||
                current == current.parent_path())
            {
                break;
            }

            // Đi lên một thư mục
            current = current.parent_path();
        }
    }


    // --------------------------------------------------------
    // PRIORITY 2
    // Tìm storage từ Current Working Directory
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // PRIORITY 3
    // Không tìm thấy → tạo storage mới
    // --------------------------------------------------------

    fs::path base =
        s_execDir.empty()
        ? fs::current_path()
        : fs::path(s_execDir);

    fs::path storage = base / "storage";

    fs::create_directories(storage / "data");
    fs::create_directories(storage / "indexes");
    fs::create_directories(storage / "backup");

    std::cerr
        << "[PathResolver] Created storage at: "
        << storage << '\n';

    return fs::absolute(storage).string();
}


// ============================================================
// DATA DIRECTORY
// ============================================================

std::string PathResolver::getDataDir()
{
    fs::path dataDir =
        fs::path(getStorageDir()) / "data";

    fs::create_directories(dataDir);

    return dataDir.string();
}


// ============================================================
// INDEX DIRECTORY
// ============================================================

std::string PathResolver::getIndexDir()
{
    fs::path indexDir =
        fs::path(getStorageDir()) / "indexes";

    fs::create_directories(indexDir);

    return indexDir.string();
}


// ============================================================
// BACKUP DIRECTORY
// ============================================================

std::string PathResolver::getBackupDir()
{
    fs::path backupDir =
        fs::path(getStorageDir()) / "backup";

    fs::create_directories(backupDir);

    return backupDir.string();
}


// ============================================================
// GET FILE PATH
// ============================================================

std::string PathResolver::getFilePath(
    const std::string& filename)
{
    // File .idx → indexes/
    if (filename.size() >= 4 &&
        filename.substr(filename.size() - 4) == ".idx")
    {
        return getIndexPath(filename);
    }

    // File bình thường → data/
    return (fs::path(getDataDir()) / filename).string();
}


// ============================================================
// GET INDEX FILE PATH
// ============================================================

std::string PathResolver::getIndexPath(
    const std::string& filename)
{
    return (fs::path(getIndexDir()) / filename).string();
}