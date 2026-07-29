#pragma once
#include <string>
#include <vector>
#include "GeneratorConfig.h"
#include "../../include/httplib.h"

struct StudentInfo {
    std::string masv;
    std::string malop;
};

bool generateClasses(httplib::Client& cli, const GeneratorConfig& cfg, std::vector<std::string>& outClassCodes);
bool generateStudents(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<std::string>& classCodes, std::vector<StudentInfo>& outStudents);
bool generateSubjects(httplib::Client& cli, const GeneratorConfig& cfg, std::vector<std::string>& outSubjectCodes);
bool generateQuestions(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<std::string>& subjectCodes);
bool generateExamsAndScores(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<StudentInfo>& students, const std::vector<std::string>& subjectCodes);
bool generateHistory(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<StudentInfo>& students, const std::vector<std::string>& subjectCodes);
