#pragma once
#include <string>
#include <cmath>
#include "Class.h"
#include "Subject.h"
#include "Exam.h"

class StorageValidator {
public:
    // Helper string & character checks
    static bool containsForbiddenChars(const std::string& str, std::string& foundCharReason);
    static bool containsControlChars(const std::string& str);
    static bool isEmptyOrWhitespace(const std::string& str);
    static bool isFiniteFloat(float val);


    // Entity validation functions
    static bool validateClass(const Lop& lop, std::string& errReason);
    static bool validateStudent(const SinhVien& sv, const std::string& malop, std::string& errReason);
    static bool validateSubject(const MonHoc& mh, std::string& errReason);
    static bool validateQuestion(const CauHoi& q, const std::string& mamh, std::string& errReason);
    static bool validateScore(const std::string& masv, const DiemThi& score, std::string& errReason);
    static bool validateExamSession(const ExamSession& session, std::string& errReason);

    // Duplicate answer validation (after normalization + lowercase comparison)
    static bool hasDuplicateOptionsAfterNormalization(const CauHoi& q, std::string& errReason);

    // Structured logging helpers
    static void logValidationError(const std::string& entity, const std::string& pk, const std::string& field, const std::string& val, const std::string& reason);
};
