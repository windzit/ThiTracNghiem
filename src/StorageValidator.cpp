#include "../include/StorageValidator.h"
#include "../include/StringNormalizer.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <cmath>

bool StorageValidator::containsControlChars(const std::string& str) {
    for (unsigned char c : str) {
        if (c < 32 && c != ' ') return true; // ASCII control characters < 32
    }
    return false;
}

bool StorageValidator::containsForbiddenChars(const std::string& str, std::string& foundCharReason) {
    if (str.find('|') != std::string::npos) {
        foundCharReason = "Contains pipe delimiter '|'";
        return true;
    }
    if (str.find('\r') != std::string::npos) {
        foundCharReason = "Contains carriage return '\\r'";
        return true;
    }
    if (str.find('\n') != std::string::npos) {
        foundCharReason = "Contains newline '\\n'";
        return true;
    }
    if (str.find('\t') != std::string::npos) {
        foundCharReason = "Contains tab character '\\t'";
        return true;
    }
    if (containsControlChars(str)) {
        foundCharReason = "Contains ASCII control character (<32)";
        return true;
    }
    return false;
}

std::string StorageValidator::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool StorageValidator::isEmptyOrWhitespace(const std::string& str) {
    return trim(str).empty();
}

bool StorageValidator::isFiniteFloat(float val) {
    return std::isfinite(val) && !std::isnan(val);
}

void StorageValidator::logValidationError(const std::string& entity, const std::string& pk, const std::string& field, const std::string& val, const std::string& reason) {
    std::cerr << "\n[VALIDATION ERROR]\n"
              << "Entity:       " << entity << "\n"
              << "Primary Key:  " << pk << "\n"
              << "Field:        " << field << "\n"
              << "Value:        " << val << "\n"
              << "Reason:       " << reason << "\n"
              << std::endl;
}

void StorageValidator::logVerificationError(const std::string& entity, const std::string& pk, const std::string& field, const std::string& expected, const std::string& actual, const std::string& reason) {
    std::cerr << "\n[VERIFICATION ERROR]\n"
              << "Entity:       " << entity << "\n"
              << "Primary Key:  " << pk << "\n"
              << "Field:        " << field << "\n"
              << "Expected:     " << expected << "\n"
              << "Actual:       " << actual << "\n"
              << "Reason:       " << reason << "\n"
              << std::endl;
}

bool StorageValidator::validateClass(const Lop& lop, std::string& errReason) {
    std::string malop = trim(lop.MALOP);
    std::string tenlop = trim(lop.TENLOP);
    std::string charReason;

    if (malop.empty()) {
        errReason = "MALOP is empty or whitespace.";
        logValidationError("Class", lop.MALOP, "MALOP", lop.MALOP, errReason);
        return false;
    }
    if (malop.length() > 15) {
        errReason = "MALOP exceeds maximum length 15: '" + malop + "'";
        logValidationError("Class", malop, "MALOP", malop, errReason);
        return false;
    }
    if (containsForbiddenChars(lop.MALOP, charReason)) {
        errReason = "MALOP " + charReason + ": '" + lop.MALOP + "'";
        logValidationError("Class", lop.MALOP, "MALOP", lop.MALOP, errReason);
        return false;
    }
    if (tenlop.empty()) {
        errReason = "TENLOP is empty or whitespace for class: " + malop;
        logValidationError("Class", malop, "TENLOP", lop.TENLOP, errReason);
        return false;
    }
    if (containsForbiddenChars(lop.TENLOP, charReason)) {
        errReason = "TENLOP " + charReason + " for class: " + malop;
        logValidationError("Class", malop, "TENLOP", lop.TENLOP, errReason);
        return false;
    }
    return true;
}

bool StorageValidator::validateStudent(const SinhVien& sv, const std::string& malop, std::string& errReason) {
    std::string masv = trim(sv.MASV);
    std::string ho = trim(sv.HO);
    std::string ten = trim(sv.TEN);
    std::string pass = trim(sv.passsword);
    std::string charReason;

    if (masv.empty()) {
        errReason = "MASV is empty or whitespace.";
        logValidationError("Student", sv.MASV, "MASV", sv.MASV, errReason);
        return false;
    }
    if (masv.length() > 15) {
        errReason = "MASV exceeds maximum length 15: '" + masv + "'";
        logValidationError("Student", masv, "MASV", masv, errReason);
        return false;
    }
    if (containsForbiddenChars(sv.MASV, charReason)) {
        errReason = "MASV " + charReason + ": '" + sv.MASV + "'";
        logValidationError("Student", sv.MASV, "MASV", sv.MASV, errReason);
        return false;
    }
    if (ho.empty() || ten.empty()) {
        errReason = "Student HO or TEN is empty for MASV: " + masv;
        logValidationError("Student", masv, "HO/TEN", sv.HO + " " + sv.TEN, errReason);
        return false;
    }
    if (containsForbiddenChars(sv.HO, charReason)) {
        errReason = "Student HO " + charReason + " for MASV: " + masv;
        logValidationError("Student", masv, "HO", sv.HO, errReason);
        return false;
    }
    if (containsForbiddenChars(sv.TEN, charReason)) {
        errReason = "Student TEN " + charReason + " for MASV: " + masv;
        logValidationError("Student", masv, "TEN", sv.TEN, errReason);
        return false;
    }
    if (containsForbiddenChars(sv.PHAI, charReason)) {
        errReason = "Student PHAI " + charReason + " for MASV: " + masv;
        logValidationError("Student", masv, "PHAI", sv.PHAI, errReason);
        return false;
    }
    if (containsForbiddenChars(sv.passsword, charReason)) {
        errReason = "Student PASSWORD " + charReason + " for MASV: " + masv;
        logValidationError("Student", masv, "PASSWORD", sv.passsword, errReason);
        return false;
    }
    if (trim(malop).empty() || containsForbiddenChars(malop, charReason)) {
        errReason = "Student class code MALOP is empty or invalid for MASV: " + masv;
        logValidationError("Student", masv, "MALOP", malop, errReason);
        return false;
    }
    return true;
}

bool StorageValidator::validateSubject(const MonHoc& mh, std::string& errReason) {
    std::string mamh = trim(mh.MAMH);
    std::string tenmh = trim(mh.TENMH);
    std::string charReason;

    if (mamh.empty()) {
        errReason = "MAMH is empty or whitespace.";
        logValidationError("Subject", mh.MAMH, "MAMH", mh.MAMH, errReason);
        return false;
    }
    if (mamh.length() > 15) {
        errReason = "MAMH exceeds maximum length 15: '" + mamh + "'";
        logValidationError("Subject", mamh, "MAMH", mamh, errReason);
        return false;
    }
    if (containsForbiddenChars(mh.MAMH, charReason)) {
        errReason = "MAMH " + charReason + ": '" + std::string(mh.MAMH) + "'";
        logValidationError("Subject", mh.MAMH, "MAMH", mh.MAMH, errReason);
        return false;
    }
    if (tenmh.empty()) {
        errReason = "TENMH is empty or whitespace for MAMH: " + mamh;
        logValidationError("Subject", mamh, "TENMH", mh.TENMH, errReason);
        return false;
    }
    if (containsForbiddenChars(mh.TENMH, charReason)) {
        errReason = "TENMH " + charReason + " for MAMH: " + mamh;
        logValidationError("Subject", mamh, "TENMH", mh.TENMH, errReason);
        return false;
    }
    return true;
}

bool StorageValidator::validateQuestion(const CauHoi& q, const std::string& mamh, std::string& errReason) {
    std::string charReason;
    std::string pk = std::to_string(q.ID);

    if (q.ID <= 0) {
        errReason = "Question ID must be positive: " + pk;
        logValidationError("Question", pk, "ID", pk, errReason);
        return false;
    }
    if (isEmptyOrWhitespace(mamh) || containsForbiddenChars(mamh, charReason)) {
        errReason = "Question parent MAMH is invalid: '" + mamh + "'";
        logValidationError("Question", pk, "MAMH", mamh, errReason);
        return false;
    }
    if (isEmptyOrWhitespace(q.NOIDUNG)) {
        errReason = "Question NOIDUNG is empty for ID: " + pk;
        logValidationError("Question", pk, "NOIDUNG", q.NOIDUNG, errReason);
        return false;
    }
    if (containsForbiddenChars(q.NOIDUNG, charReason)) {
        errReason = "Question NOIDUNG " + charReason + " for ID: " + pk;
        logValidationError("Question", pk, "NOIDUNG", q.NOIDUNG, errReason);
        return false;
    }
    if (isEmptyOrWhitespace(q.A) || isEmptyOrWhitespace(q.B) ||
        isEmptyOrWhitespace(q.C) || isEmptyOrWhitespace(q.D)) {
        errReason = "One or more question options (A,B,C,D) are empty for ID: " + pk;
        logValidationError("Question", pk, "A/B/C/D", q.A + " | " + q.B, errReason);
        return false;
    }
    if (containsForbiddenChars(q.A, charReason) || containsForbiddenChars(q.B, charReason) ||
        containsForbiddenChars(q.C, charReason) || containsForbiddenChars(q.D, charReason)) {
        errReason = "One or more question options contain " + charReason + " for ID: " + pk;
        logValidationError("Question", pk, "A/B/C/D", q.A + " | " + q.B, errReason);
        return false;
    }

    char ans = std::toupper(q.DAPAN_DUNG);
    if (ans != 'A' && ans != 'B' && ans != 'C' && ans != 'D') {
        errReason = "Question correct answer DAPAN_DUNG must be A, B, C, or D. Got: '" + std::string(1, q.DAPAN_DUNG) + "'";
        logValidationError("Question", pk, "DAPAN_DUNG", std::string(1, q.DAPAN_DUNG), errReason);
        return false;
    }

    // Duplicate option check AFTER normalization = REJECTION
    std::string dupReason;
    if (hasDuplicateOptionsAfterNormalization(q, dupReason)) {
        errReason = dupReason + " for ID: " + pk;
        logValidationError("Question", pk, "A/B/C/D", q.A + " | " + q.B, errReason);
        return false;
    }

    return true;
}

bool StorageValidator::hasDuplicateOptionsAfterNormalization(const CauHoi& q, std::string& errReason) {
    // Normalize for comparison: apply StringNormalizer::normalizeHumanText then lowercase
    std::string opts[4] = {
        StringNormalizer::normalizeHumanText(q.A),
        StringNormalizer::normalizeHumanText(q.B),
        StringNormalizer::normalizeHumanText(q.C),
        StringNormalizer::normalizeHumanText(q.D)
    };
    for (int i = 0; i < 4; i++) {
        for (char& c : opts[i]) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (opts[i] == opts[j]) {
                errReason = "Duplicate answer options after normalization: option "
                          + std::string(1, 'A' + i) + " and " + std::string(1, 'A' + j);
                return true;
            }
        }
    }
    return false;
}

bool StorageValidator::validateScore(const std::string& masv, const DiemThi& score, const Class& dsl, const Subject& dsmh, std::string& errReason) {
    std::string s_masv = trim(masv);
    std::string s_mamh = trim(score.MAMH);
    std::string charReason;

    if (s_masv.empty() || containsForbiddenChars(masv, charReason)) {
        errReason = "Score MASV is empty or " + charReason + ": '" + masv + "'";
        logValidationError("Score", masv, "MASV", masv, errReason);
        return false;
    }
    if (s_mamh.empty() || containsForbiddenChars(score.MAMH, charReason)) {
        errReason = "Score MAMH is empty or " + charReason + ": '" + std::string(score.MAMH) + "'";
        logValidationError("Score", masv + ":" + std::string(score.MAMH), "MAMH", score.MAMH, errReason);
        return false;
    }
    if (!isFiniteFloat(score.DIEM)) {
        errReason = "Score DIEM is NaN or Infinite.";
        logValidationError("Score", s_masv + ":" + s_mamh, "DIEM", std::to_string(score.DIEM), errReason);
        return false;
    }
    if (score.DIEM < 0.0f || score.DIEM > 10.0f) {
        errReason = "Score DIEM must be between 0.0 and 10.0. Got: " + std::to_string(score.DIEM);
        logValidationError("Score", s_masv + ":" + s_mamh, "DIEM", std::to_string(score.DIEM), errReason);
        return false;
    }

    return true;
}

bool StorageValidator::validateExamSession(const ExamSession& session, const Class& dsl, const Subject& dsmh, std::string& errReason) {
    std::string masv = trim(session.MASV);
    std::string mamh = trim(session.MAMH);
    std::string charReason;

    if (masv.empty() || containsForbiddenChars(session.MASV, charReason)) {
        errReason = "ExamSession MASV is empty or " + charReason + ": '" + session.MASV + "'";
        logValidationError("ExamSession", session.MASV, "MASV", session.MASV, errReason);
        return false;
    }
    if (mamh.empty() || containsForbiddenChars(session.MAMH, charReason)) {
        errReason = "ExamSession MAMH is empty or " + charReason + ": '" + session.MAMH + "'";
        logValidationError("ExamSession", session.MASV, "MAMH", session.MAMH, errReason);
        return false;
    }
    if (session.questionIds.empty()) {
        errReason = "ExamSession questionIds list is empty for MASV: " + masv;
        logValidationError("ExamSession", session.MASV, "questionIds", "empty", errReason);
        return false;
    }
    for (int i = 0; i < session.questionIds.size(); i++) {
        if (session.questionIds[i] <= 0) {
            errReason = "ExamSession contains invalid non-positive question ID at index " + std::to_string(i);
            logValidationError("ExamSession", session.MASV, "questionIds", std::to_string(session.questionIds[i]), errReason);
            return false;
        }
    }
    if (session.tongThoiGianPhut <= 0) {
        errReason = "ExamSession total duration in minutes must be positive. Got: " + std::to_string(session.tongThoiGianPhut);
        logValidationError("ExamSession", session.MASV, "tongThoiGianPhut", std::to_string(session.tongThoiGianPhut), errReason);
        return false;
    }
    return true;
}
