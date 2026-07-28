#pragma once
#include <string>
#include <cctype>
#include "../DArray.h"

namespace Utility {
namespace StringUtil {

inline std::string trim(const std::string& s) {
    if (s.empty()) return s;
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

inline DArray<std::string> split(const std::string& s, char delimiter) {
    DArray<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = s.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start));
    return tokens;
}

inline std::string toUpper(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return s;
}

inline std::string toLower(std::string s) {
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

inline std::string replace(std::string s, const std::string& from, const std::string& to) {
    if (from.empty()) return s;
    std::string::size_type start_pos = 0;
    while ((start_pos = s.find(from, start_pos)) != std::string::npos) {
        s.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return s;
}

inline bool contains(const std::string& s, const std::string& substring) {
    if (substring.empty()) return true;
    return s.find(substring) != std::string::npos;
}

inline bool startsWith(const std::string& s, const std::string& prefix) {
    if (s.length() < prefix.length()) return false;
    return s.compare(0, prefix.length(), prefix) == 0;
}

inline bool endsWith(const std::string& s, const std::string& suffix) {
    if (s.length() < suffix.length()) return false;
    return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
}

} // namespace StringUtil
} // namespace Utility
