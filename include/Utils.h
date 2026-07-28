#pragma once
#include "./CommonTypes.h"
#include "DArray.h"
#include <cmath>

// ============================================================
// String utilities
// ============================================================
std::string trim(const std::string& s);
DArray<std::string> split(const std::string& s, char delimiter);

// ============================================================
// Score utilities
// ============================================================
double roundScore(double score, int precision = 1);

// ============================================================
// ID generation (auto-increment per debai.md)
// ============================================================
int generateQuestionID();

// ============================================================
// Validation utilities
// ============================================================
bool MALOP_CHECKER(const std::string& MALOP);
bool TENLOP_CHECKER(const std::string& TENLOP);