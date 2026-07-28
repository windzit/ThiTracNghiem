#include "../include/StorageVerifier.h"
#include "../include/StorageDeserializer.h"
#include "../include/StorageValidator.h"
#include <iostream>
#include <cmath>

bool StorageVerifier::verifyClasses(Class& dsl, const std::string& filePath, std::string& errReason) {
    DArray<Lop> readBack;
    if (!StorageDeserializer::parseClassesFile(filePath, readBack)) {
        errReason = "Could not open or parse written classes file: " + filePath;
        StorageValidator::logVerificationError("Class", "ALL", "FILE", "readable file", "unreadable", errReason);
        return false;
    }

    // Collect RAM objects
    DArray<Lop> ramList;
    dsLop* root = dsl.getRoot();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            if (root->dslop[i]) {
                ramList.push_back(*root->dslop[i]);
            }
        }
    }

    if (ramList.size() != readBack.size()) {
        errReason = "Class record count mismatch.";
        StorageValidator::logVerificationError("Class", "COUNT", "size", std::to_string(ramList.size()), std::to_string(readBack.size()), errReason);
        return false;
    }

    for (int i = 0; i < ramList.size(); i++) {
        const Lop& ram = ramList[i];
        const Lop& disk = readBack[i];
        if (StorageValidator::trim(ram.MALOP) != StorageValidator::trim(disk.MALOP)) {
            errReason = "MALOP mismatch at index " + std::to_string(i);
            StorageValidator::logVerificationError("Class", ram.MALOP, "MALOP", ram.MALOP, disk.MALOP, errReason);
            return false;
        }
        if (StorageValidator::trim(ram.TENLOP) != StorageValidator::trim(disk.TENLOP)) {
            errReason = "TENLOP mismatch for MALOP: " + ram.MALOP;
            StorageValidator::logVerificationError("Class", ram.MALOP, "TENLOP", ram.TENLOP, disk.TENLOP, errReason);
            return false;
        }
    }

    return true;
}

bool StorageVerifier::verifyStudents(Class& dsl, const std::string& filePath, std::string& errReason) {
    DArray<SinhVien> readBack;
    DArray<std::string> diskClassCodes;
    if (!StorageDeserializer::parseStudentsFile(filePath, readBack, diskClassCodes)) {
        errReason = "Could not open or parse written students file: " + filePath;
        StorageValidator::logVerificationError("Student", "ALL", "FILE", "readable file", "unreadable", errReason);
        return false;
    }

    // Collect RAM objects
    DArray<SinhVien> ramList;
    DArray<std::string> ramClassCodes;
    dsLop* root = dsl.getRoot();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            Lop* lop = root->dslop[i];
            if (!lop) continue;
            dsSinhVien* cur = lop->dssinhvien.getRoot();
            while (cur) {
                ramList.push_back(cur->sinhvien);
                ramClassCodes.push_back(lop->MALOP);
                cur = cur->next;
            }
        }
    }

    if (ramList.size() != readBack.size()) {
        errReason = "Student record count mismatch.";
        StorageValidator::logVerificationError("Student", "COUNT", "size", std::to_string(ramList.size()), std::to_string(readBack.size()), errReason);
        return false;
    }

    for (int i = 0; i < ramList.size(); i++) {
        const SinhVien& ram = ramList[i];
        const SinhVien& disk = readBack[i];
        std::string pk = ram.MASV;

        if (StorageValidator::trim(ram.MASV) != StorageValidator::trim(disk.MASV)) {
            errReason = "MASV mismatch at index " + std::to_string(i);
            StorageValidator::logVerificationError("Student", pk, "MASV", ram.MASV, disk.MASV, errReason);
            return false;
        }
        if (ramClassCodes[i] != diskClassCodes[i]) {
            errReason = "MALOP mismatch for student MASV: " + pk;
            StorageValidator::logVerificationError("Student", pk, "MALOP", ramClassCodes[i], diskClassCodes[i], errReason);
            return false;
        }
        if (ram.HO != disk.HO || ram.TEN != disk.TEN) {
            errReason = "Name mismatch for student MASV: " + pk;
            StorageValidator::logVerificationError("Student", pk, "HO/TEN", ram.HO + " " + ram.TEN, disk.HO + " " + disk.TEN, errReason);
            return false;
        }
        if (ram.PHAI != disk.PHAI) {
            errReason = "Gender mismatch for student MASV: " + pk;
            StorageValidator::logVerificationError("Student", pk, "PHAI", ram.PHAI, disk.PHAI, errReason);
            return false;
        }
        if (ram.passsword != disk.passsword) {
            errReason = "Password mismatch for student MASV: " + pk;
            StorageValidator::logVerificationError("Student", pk, "PASSWORD", ram.passsword, disk.passsword, errReason);
            return false;
        }
    }

    return true;
}

static void collectSubjectsFromRAM(NodeMH* node, DArray<MonHoc>& ramList) {
    if (!node) return;
    collectSubjectsFromRAM(node->left, ramList);
    ramList.push_back(node->data);
    collectSubjectsFromRAM(node->right, ramList);
}

bool StorageVerifier::verifySubjects(Subject& dsmh, const std::string& filePath, std::string& errReason) {
    DArray<MonHoc> readBack;
    if (!StorageDeserializer::parseSubjectsFile(filePath, readBack)) {
        errReason = "Could not open or parse written subjects file: " + filePath;
        StorageValidator::logVerificationError("Subject", "ALL", "FILE", "readable file", "unreadable", errReason);
        return false;
    }

    DArray<MonHoc> ramList;
    collectSubjectsFromRAM(dsmh.getRoot(), ramList);

    if (ramList.size() != readBack.size()) {
        errReason = "Subject record count mismatch.";
        StorageValidator::logVerificationError("Subject", "COUNT", "size", std::to_string(ramList.size()), std::to_string(readBack.size()), errReason);
        return false;
    }

    for (int i = 0; i < ramList.size(); i++) {
        const MonHoc& ram = ramList[i];
        const MonHoc& disk = readBack[i];
        std::string pk = ram.MAMH;

        if (StorageValidator::trim(ram.MAMH) != StorageValidator::trim(disk.MAMH)) {
            errReason = "MAMH mismatch at index " + std::to_string(i);
            StorageValidator::logVerificationError("Subject", pk, "MAMH", ram.MAMH, disk.MAMH, errReason);
            return false;
        }
        if (StorageValidator::trim(ram.TENMH) != StorageValidator::trim(disk.TENMH)) {
            errReason = "TENMH mismatch for MAMH: " + pk;
            StorageValidator::logVerificationError("Subject", pk, "TENMH", ram.TENMH, disk.TENMH, errReason);
            return false;
        }
    }

    return true;
}

static void collectQuestionsFromRAM(NodeMH* node, DArray<CauHoi>& ramList, DArray<std::string>& subjectCodes) {
    if (!node) return;
    collectQuestionsFromRAM(node->left, ramList, subjectCodes);

    const MonHoc& mh = node->data;
    for (int id = 0; id <= 100000; id++) {
        dsCHT* qNode = const_cast<MonHoc&>(mh).dsCauHoi.find(id);
        if (qNode) {
            ramList.push_back(qNode->cauhoi);
            subjectCodes.push_back(mh.MAMH);
        }
    }

    collectQuestionsFromRAM(node->right, ramList, subjectCodes);
}

bool StorageVerifier::verifyQuestions(Subject& dsmh, const std::string& filePath, std::string& errReason) {
    DArray<CauHoi> readBack;
    DArray<std::string> diskSubjectCodes;
    if (!StorageDeserializer::parseQuestionsFile(filePath, readBack, diskSubjectCodes)) {
        errReason = "Could not open or parse written questions file: " + filePath;
        StorageValidator::logVerificationError("Question", "ALL", "FILE", "readable file", "unreadable", errReason);
        return false;
    }

    DArray<CauHoi> ramList;
    DArray<std::string> ramSubjectCodes;
    collectQuestionsFromRAM(dsmh.getRoot(), ramList, ramSubjectCodes);

    if (ramList.size() != readBack.size()) {
        errReason = "Question record count mismatch.";
        StorageValidator::logVerificationError("Question", "COUNT", "size", std::to_string(ramList.size()), std::to_string(readBack.size()), errReason);
        return false;
    }

    for (int i = 0; i < ramList.size(); i++) {
        const CauHoi& ram = ramList[i];
        const CauHoi& disk = readBack[i];
        std::string pk = std::to_string(ram.ID);

        if (ram.ID != disk.ID) {
            errReason = "Question ID mismatch at index " + std::to_string(i);
            StorageValidator::logVerificationError("Question", pk, "ID", pk, std::to_string(disk.ID), errReason);
            return false;
        }
        if (ramSubjectCodes[i] != diskSubjectCodes[i]) {
            errReason = "Question parent MAMH mismatch for ID: " + pk;
            StorageValidator::logVerificationError("Question", pk, "MAMH", ramSubjectCodes[i], diskSubjectCodes[i], errReason);
            return false;
        }
        if (ram.NOIDUNG != disk.NOIDUNG) {
            errReason = "Question NOIDUNG mismatch for ID: " + pk;
            StorageValidator::logVerificationError("Question", pk, "NOIDUNG", ram.NOIDUNG, disk.NOIDUNG, errReason);
            return false;
        }
        if (ram.A != disk.A || ram.B != disk.B || ram.C != disk.C || ram.D != disk.D) {
            errReason = "Question options mismatch for ID: " + pk;
            StorageValidator::logVerificationError("Question", pk, "OPTIONS", ram.A + "|" + ram.B, disk.A + "|" + disk.B, errReason);
            return false;
        }
        if (std::toupper(ram.DAPAN_DUNG) != std::toupper(disk.DAPAN_DUNG)) {
            errReason = "Question DAPAN_DUNG mismatch for ID: " + pk;
            StorageValidator::logVerificationError("Question", pk, "DAPAN_DUNG", std::string(1, ram.DAPAN_DUNG), std::string(1, disk.DAPAN_DUNG), errReason);
            return false;
        }
        if (ram.deleted != disk.deleted) {
            errReason = "Question DELETED mismatch for ID: " + pk;
            StorageValidator::logVerificationError("Question", pk, "DELETED",
                std::to_string(ram.deleted), std::to_string(disk.deleted), errReason);
            return false;
        }
    }

    return true;
}

bool StorageVerifier::verifyScores(Class& dsl, const std::string& filePath, std::string& errReason) {
    DArray<DiemThi> readBack;
    DArray<std::string> diskMasvList;
    if (!StorageDeserializer::parseScoresFile(filePath, readBack, diskMasvList)) {
        errReason = "Could not open or parse written scores file: " + filePath;
        StorageValidator::logVerificationError("Score", "ALL", "FILE", "readable file", "unreadable", errReason);
        return false;
    }

    DArray<DiemThi> ramList;
    DArray<std::string> ramMasvList;
    dsLop* root = dsl.getRoot();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            Lop* lop = root->dslop[i];
            if (!lop) continue;
            dsSinhVien* curSV = lop->dssinhvien.getRoot();
            while (curSV) {
                const SinhVien& sv = curSV->sinhvien;
                dsDiemThi* curScore = const_cast<SinhVien&>(sv).dsdiemthi.getRoot();
                while (curScore) {
                    ramList.push_back(curScore->diemthi);
                    ramMasvList.push_back(sv.MASV);
                    curScore = curScore->next;
                }
                curSV = curSV->next;
            }
        }
    }

    if (ramList.size() != readBack.size()) {
        errReason = "Score record count mismatch.";
        StorageValidator::logVerificationError("Score", "COUNT", "size", std::to_string(ramList.size()), std::to_string(readBack.size()), errReason);
        return false;
    }

    for (int i = 0; i < ramList.size(); i++) {
        const DiemThi& ram = ramList[i];
        const DiemThi& disk = readBack[i];
        std::string pk = ramMasvList[i] + ":" + std::string(ram.MAMH);

        if (ramMasvList[i] != diskMasvList[i]) {
            errReason = "Score MASV mismatch at index " + std::to_string(i);
            StorageValidator::logVerificationError("Score", pk, "MASV", ramMasvList[i], diskMasvList[i], errReason);
            return false;
        }
        if (StorageValidator::trim(ram.MAMH) != StorageValidator::trim(disk.MAMH)) {
            errReason = "Score MAMH mismatch for MASV: " + ramMasvList[i];
            StorageValidator::logVerificationError("Score", pk, "MAMH", ram.MAMH, disk.MAMH, errReason);
            return false;
        }
        if (std::abs(ram.DIEM - disk.DIEM) > 0.001f) {
            errReason = "Score DIEM mismatch for " + pk;
            StorageValidator::logVerificationError("Score", pk, "DIEM", std::to_string(ram.DIEM), std::to_string(disk.DIEM), errReason);
            return false;
        }
    }

    return true;
}

bool StorageVerifier::verifyExamSessions(const DArray<ExamSession>& ramSessions, const std::string& filePath, std::string& errReason) {
    DArray<ExamSession> readBack;
    if (!StorageDeserializer::parseExamSessionsFile(filePath, readBack)) {
        errReason = "Could not open or parse written exam sessions file: " + filePath;
        StorageValidator::logVerificationError("ExamSession", "ALL", "FILE", "readable file", "unreadable", errReason);
        return false;
    }

    DArray<ExamSession> activeRam;
    for (int i = 0; i < ramSessions.size(); i++) {
        if (ramSessions[i].in_progress) {
            activeRam.push_back(ramSessions[i]);
        }
    }

    if (activeRam.size() != readBack.size()) {
        errReason = "ExamSession active session record count mismatch.";
        StorageValidator::logVerificationError("ExamSession", "COUNT", "size", std::to_string(activeRam.size()), std::to_string(readBack.size()), errReason);
        return false;
    }

    for (int i = 0; i < activeRam.size(); i++) {
        const ExamSession& ram = activeRam[i];
        const ExamSession& disk = readBack[i];
        std::string pk = ram.MASV;

        if (StorageValidator::trim(ram.MASV) != StorageValidator::trim(disk.MASV)) {
            errReason = "ExamSession MASV mismatch at index " + std::to_string(i);
            StorageValidator::logVerificationError("ExamSession", pk, "MASV", ram.MASV, disk.MASV, errReason);
            return false;
        }
        if (StorageValidator::trim(ram.MAMH) != StorageValidator::trim(disk.MAMH)) {
            errReason = "ExamSession MAMH mismatch for MASV: " + pk;
            StorageValidator::logVerificationError("ExamSession", pk, "MAMH", ram.MAMH, disk.MAMH, errReason);
            return false;
        }
        if (ram.questionIds.size() != disk.questionIds.size()) {
            errReason = "ExamSession questionIds size mismatch for MASV: " + pk;
            StorageValidator::logVerificationError("ExamSession", pk, "questionIds.size", std::to_string(ram.questionIds.size()), std::to_string(disk.questionIds.size()), errReason);
            return false;
        }
    }

    return true;
}

bool StorageVerifier::verifyFileRowCount(const std::string& filePath, int expectedHeaderLines, int expectedRecordCount, std::string& errReason) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        errReason = "Verification failed: Could not open file for reading: " + filePath;
        return false;
    }

    int actualLines = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (!StorageValidator::isEmptyOrWhitespace(line)) {
            actualLines++;
        }
    }
    file.close();

    int expectedTotalLines = expectedHeaderLines + expectedRecordCount;
    if (actualLines != expectedTotalLines) {
        errReason = "Verification row count mismatch for " + filePath + 
                    ". Expected: " + std::to_string(expectedTotalLines) + 
                    ", Got actual: " + std::to_string(actualLines);
        return false;
    }

    return true;
}
