#include "../include/StorageDeserializer.h"
#include "../include/Utils.h"
#include "../include/StorageValidator.h"
#include <fstream>
#include <iostream>

bool StorageDeserializer::parseClassesFile(const std::string& filePath, DArray<Lop>& result) {
    result.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            Lop lop;
            lop.MALOP = trim(tokens[0]);
            lop.TENLOP = trim(tokens[1]);
            result.push_back(lop);
        }
    }
    file.close();
    return true;
}

bool StorageDeserializer::parseStudentsFile(const std::string& filePath, DArray<SinhVien>& result, DArray<std::string>& classCodes) {
    result.clear();
    classCodes.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 6) {
            std::string malop = trim(tokens[0]);
            SinhVien sv;
            sv.MASV = trim(tokens[1]);
            sv.HO = tokens[2];
            sv.TEN = tokens[3];
            sv.PHAI = tokens[4];
            sv.passsword = tokens[5];

            result.push_back(sv);
            classCodes.push_back(malop);
        }
    }
    file.close();
    return true;
}

bool StorageDeserializer::parseSubjectsFile(const std::string& filePath, DArray<MonHoc>& result) {
    result.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            MonHoc mh;
            std::string mamh = trim(tokens[0]);
            std::strcpy(mh.MAMH, mamh.c_str());
            mh.TENMH = tokens[1];
            mh.used = (tokens.size() >= 3 && (tokens[2] == "1" || tokens[2] == "true"));
            result.push_back(mh);
        }
    }
    file.close();
    return true;
}

bool StorageDeserializer::parseQuestionsFile(const std::string& filePath, DArray<CauHoi>& result, DArray<std::string>& subjectCodes) {
    result.clear();
    subjectCodes.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 8) {
            std::string mamh = trim(tokens[0]);
            CauHoi q;
            q.ID = std::stoi(trim(tokens[1]));
            q.NOIDUNG = tokens[2];
            q.A = tokens[3];
            q.B = tokens[4];
            q.C = tokens[5];
            q.D = tokens[6];
            std::string dapan = trim(tokens[7]);
            q.DAPAN_DUNG = dapan.empty() ? '\0' : dapan[0];
            q.used = (tokens.size() >= 9 && (tokens[8] == "1" || tokens[8] == "true"));
            q.deleted = (tokens.size() >= 10 && (tokens[9] == "1" || tokens[9] == "true"));

            result.push_back(q);
            subjectCodes.push_back(mamh);
        }
    }
    file.close();
    return true;
}

bool StorageDeserializer::parseScoresFile(const std::string& filePath, DArray<DiemThi>& result, DArray<std::string>& masvList) {
    result.clear();
    masvList.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 3) {
            std::string masv = trim(tokens[0]);
            std::string mamh = trim(tokens[1]);
            float diem = std::stof(trim(tokens[2]));

            DiemThi dt;
            std::strcpy(dt.MAMH, mamh.c_str());
            dt.DIEM = diem;

            result.push_back(dt);
            masvList.push_back(masv);
        }
    }
    file.close();
    return true;
}

bool StorageDeserializer::parseExamSessionsFile(const std::string& filePath, DArray<ExamSession>& result) {
    result.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 8) {
            ExamSession s;
            s.MASV = trim(tokens[0]);
            s.MAMH = trim(tokens[1]);
            s.thoiGianBatDau = std::stoll(trim(tokens[2]));
            s.tongThoiGianPhut = std::stoi(trim(tokens[3]));
            s.in_progress = (trim(tokens[4]) == "1" || trim(tokens[4]) == "true");
            s.lastServerActivityAt = std::stoll(trim(tokens[5]));

            DArray<std::string> qIdStrs = split(tokens[6], ',');
            for (int i = 0; i < qIdStrs.size(); i++) {
                if (!trim(qIdStrs[i]).empty()) {
                    s.questionIds.push_back(std::stoi(trim(qIdStrs[i])));
                }
            }

            DArray<std::string> ansStrs = split(tokens[7], ',');
            for (int i = 0; i < ansStrs.size(); i++) {
                std::string a = trim(ansStrs[i]);
                char ansChar = a.empty() ? '-' : a[0];
                s.answers.push_back(ansChar == '-' ? '\0' : ansChar);
            }

            if (s.in_progress) {
                result.push_back(s);
            }
        }
    }
    file.close();
    return true;
}
