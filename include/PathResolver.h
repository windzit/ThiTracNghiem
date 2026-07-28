#pragma once
#include <string>
#include <filesystem>

class PathResolver {
public:
    // Must be called once at program startup with argv[0]
    static void init(const std::string& executablePath);

    // Resolves the absolute path to the storage directory
    static std::string getStorageDir();

    // Resolves the absolute path to a specific file inside storage directory
    static std::string getFilePath(const std::string& filename);

private:
    // Absolute path to the directory containing the executable
    static std::string s_execDir;
};
