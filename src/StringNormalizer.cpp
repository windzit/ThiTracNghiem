#include "../include/StringNormalizer.h"
#include "../include/Utils.h"
#include <cctype>

std::string StringNormalizer::normalizeIdentifier(const std::string& input) {
    std::string result;
    result.reserve(input.length());
    for (char c : input) {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
        }
    }
    return result;
}

std::string StringNormalizer::toTitleCase(const std::string& input) {
    std::string s = normalizeHumanText(input);
    if (s.empty()) return "";

    std::string result;
    result.reserve(s.length());
    bool isWordStart = true;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c == ' ') {
            result.push_back(' ');
            isWordStart = true;
        } else {
            if (isWordStart) {
                result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
                isWordStart = false;
            } else {
                result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            }
        }
    }
    return result;
}

std::string StringNormalizer::normalizeHumanText(const std::string& input) {
    if (input.empty()) return "";

    // 1. Preserve newlines untouched so StorageValidator will reject them
    bool hasNewline = (input.find('\r') != std::string::npos || input.find('\n') != std::string::npos);
    if (hasNewline) {
        return input;
    }

    // 2. Convert tabs to spaces
    std::string s = input;
    for (char& c : s) {
        if (c == '\t') c = ' ';
    }

    // 3. Trim leading & trailing whitespace
    size_t first = s.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \t\n\r");
    s = s.substr(first, (last - first + 1));

    // 4. Collapse multiple consecutive spaces into a single space
    std::string result;
    result.reserve(s.length());
    bool lastWasSpace = false;
    for (char c : s) {
        if (c == ' ') {
            if (!lastWasSpace) {
                result.push_back(' ');
                lastWasSpace = true;
            }
        } else {
            result.push_back(c);
            lastWasSpace = false;
        }
    }

    return result;
}

void StringNormalizer::normalizeClass(Lop& lop) {
    lop.MALOP = normalizeIdentifier(lop.MALOP);
    lop.TENLOP = toTitleCase(lop.TENLOP);
}

void StringNormalizer::normalizeStudent(SinhVien& sv) {
    sv.MASV = normalizeIdentifier(sv.MASV);
    sv.HO = toTitleCase(sv.HO);
    sv.TEN = toTitleCase(sv.TEN);
    sv.PHAI = normalizeHumanText(sv.PHAI);
    // PASSWORD is left 100% untouched
}

void StringNormalizer::normalizeSubject(MonHoc& mh) {
    std::string mamh = normalizeIdentifier(mh.MAMH);
    std::strncpy(mh.MAMH, mamh.c_str(), sizeof(mh.MAMH) - 1);
    mh.MAMH[sizeof(mh.MAMH) - 1] = '\0';
    mh.TENMH = toTitleCase(mh.TENMH);
}

void StringNormalizer::normalizeQuestion(CauHoi& q) {
    q.NOIDUNG = normalizeHumanText(q.NOIDUNG);
    q.A = normalizeHumanText(q.A);
    q.B = normalizeHumanText(q.B);
    q.C = normalizeHumanText(q.C);
    q.D = normalizeHumanText(q.D);
}

void StringNormalizer::normalizeScore(DiemThi& dt, std::string& masv) {
    masv = trim(masv);
    std::string mamh = trim(dt.MAMH);
    std::strncpy(dt.MAMH, mamh.c_str(), sizeof(dt.MAMH) - 1);
    dt.MAMH[sizeof(dt.MAMH) - 1] = '\0';
}

void StringNormalizer::normalizeExamSession(ExamSession& session) {
    session.MASV = trim(session.MASV);
    session.MAMH = trim(session.MAMH);
}
