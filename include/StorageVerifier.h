#pragma once
#include <string>
#include "Class.h"
#include "Subject.h"
#include "Exam.h"
#include "DArray.h"

class StorageVerifier {
public:
    static bool verifyClasses(Class& dsl, const std::string& filePath, std::string& errReason);
    static bool verifyStudents(Class& dsl, const std::string& filePath, std::string& errReason);
    static bool verifySubjects(Subject& dsmh, const std::string& filePath, std::string& errReason);
    static bool verifyQuestions(Subject& dsmh, const std::string& filePath, std::string& errReason);
    static bool verifyScores(Class& dsl, const std::string& filePath, std::string& errReason);
    static bool verifyExamSessions(const DArray<ExamSession>& ramSessions, const std::string& filePath, std::string& errReason);
    static bool verifyFileRowCount(const std::string& filePath, int expectedHeaderLines, int expectedRecordCount, std::string& errReason);
};
