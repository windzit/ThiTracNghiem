#pragma once
#include "./CommonTypes.h"
#include "DArray.h"


std::string trim(const std::string& s);
DArray<std::string> split(const std::string& s, char delimiter);


double roundScore(double score, int precision = 1);


