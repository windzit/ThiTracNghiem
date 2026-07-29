#pragma once
#include <string>
#include <filesystem>

class PathResolver {
public:
    // Must be called once at program startup with argv[0]
    static void init(const std::string& executablePath);

    // Resolves the absolute path to the root storage directory
    static std::string getStorageDir();

    // Resolves the absolute path to the storage/data directory
    static std::string getDataDir();

    // Resolves the absolute path to the storage/indexes directory
    static std::string getIndexDir();

    // Resolves the absolute path to the storage/backup directory
    static std::string getBackupDir();

    // Resolves the absolute path to a specific data file inside storage/data directory
    static std::string getFilePath(const std::string& filename);

    // Resolves the absolute path to a specific index file inside storage/indexes directory
    static std::string getIndexPath(const std::string& indexFilename);

    // One-time auto-migration of legacy root storage/*.txt files into storage/data/
    static void migrateLegacyStorage();

private:
    // Absolute path to the directory containing the executable
    static std::string s_execDir;
};

