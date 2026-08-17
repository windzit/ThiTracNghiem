#include "../include/Utils.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <locale>
#include <iostream>

// ============================================================
// String utilities
// ============================================================

std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    auto end = s.end();
    while (end != start && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        end--;
    }
    return std::string(start, end);
}

DArray<std::string> split(const std::string& s, char delimiter) {
    DArray<std::string> tokens;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ============================================================
// Score utilities
// ============================================================

double roundScore(double score, int precision) {
    double factor = std::pow(10.0, precision);
    return std::round(score * factor) / factor;
}