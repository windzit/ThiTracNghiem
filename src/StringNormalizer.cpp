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

    // 1. Giữ nguyên dấu xuống dòng để StorageValidator chặn
    bool hasNewline = (input.find('\r') != std::string::npos || input.find('\n') != std::string::npos);
    if (hasNewline) return input;

    // 2. Chuyển Tab thành dấu cách
    std::string s = input;
    for (char& c : s) {
        if (c == '\t') c = ' ';
    }

    // 3. gọt sạch 2 đầu
    s = trim(s);
    if (s.empty()) return "";

    // 4. Thu gọn nhiều dấu cách ở giữa thành 1 dấu cách
    std::string result;
    result.reserve(s.length());
    bool lastWasSpace = false;
    for (char c : s) {
        if (c == ' ') {
            if (!lastWasSpace) {
                result.push_back(' ');
                lastWasSpace = true;
            }
        }
        else {
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
