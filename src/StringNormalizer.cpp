#include "../include/StringNormalizer.h"
#include <cctype>

std::string StringNormalizer::trimIdentifier(const std::string& input) {
    size_t first = input.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = input.find_last_not_of(" \t\n\r");
    return input.substr(first, (last - first + 1));
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
    lop.MALOP = trimIdentifier(lop.MALOP);
    lop.TENLOP = normalizeHumanText(lop.TENLOP);
}

void StringNormalizer::normalizeStudent(SinhVien& sv) {
    sv.MASV = trimIdentifier(sv.MASV);
    sv.HO = normalizeHumanText(sv.HO);
    sv.TEN = normalizeHumanText(sv.TEN);
    sv.PHAI = normalizeHumanText(sv.PHAI);
    // PASSWORD is left 100% untouched
}

void StringNormalizer::normalizeSubject(MonHoc& mh) {
    std::string mamh = trimIdentifier(mh.MAMH);
    std::strncpy(mh.MAMH, mamh.c_str(), sizeof(mh.MAMH) - 1);
    mh.MAMH[sizeof(mh.MAMH) - 1] = '\0';
    mh.TENMH = normalizeHumanText(mh.TENMH);
}

void StringNormalizer::normalizeQuestion(CauHoi& q) {
    q.NOIDUNG = normalizeHumanText(q.NOIDUNG);
    q.A = normalizeHumanText(q.A);
    q.B = normalizeHumanText(q.B);
    q.C = normalizeHumanText(q.C);
    q.D = normalizeHumanText(q.D);
}

void StringNormalizer::normalizeScore(DiemThi& dt, std::string& masv) {
    masv = trimIdentifier(masv);
    std::string mamh = trimIdentifier(dt.MAMH);
    std::strncpy(dt.MAMH, mamh.c_str(), sizeof(dt.MAMH) - 1);
    dt.MAMH[sizeof(dt.MAMH) - 1] = '\0';
}

void StringNormalizer::normalizeExamSession(ExamSession& session) {
    session.MASV = trimIdentifier(session.MASV);
    session.MAMH = trimIdentifier(session.MAMH);
}
