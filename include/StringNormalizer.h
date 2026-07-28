#pragma once
#include <string>
#include "Class.h"
#include "Subject.h"
#include "Exam.h"

class StringNormalizer {
public:
    // Generic text normalization (Converts tab->space, trims, collapses consecutive spaces)
    // Preserves '\r' and '\n' untouched so validator can detect and reject them.
    static std::string normalizeHumanText(const std::string& input);

    // Generic identifier normalization (Trims leading/trailing whitespace only)
    static std::string trimIdentifier(const std::string& input);

    // Entity normalization helpers
    static void normalizeClass(Lop& lop);
    static void normalizeStudent(SinhVien& sv);
    static void normalizeSubject(MonHoc& mh);
    static void normalizeQuestion(CauHoi& q);
    static void normalizeScore(DiemThi& dt, std::string& masv);
    static void normalizeExamSession(ExamSession& session);
};
