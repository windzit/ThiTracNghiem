#include "Diagnostics.h"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <limits>
#include "Class.h"
#include "Subject.h"
#include "Student.h"
#include "Question.h"
#include "Score.h"
#include "Exam.h"
#include "StorageManager.h"
#include "StorageValidator.h"
#include "StringNormalizer.h"
#include "Utils.h"
#include "PathResolver.h"

bool runValidationTests() {
    std::cout << "=== RUNNING STORAGE PERSISTENCE VALIDATION TESTS ===\n";
    int passed = 0;
    int total = 0;

    // Test 1: Class with forbidden delimiter '|'
    total++;
    Lop invalidClass;
    invalidClass.MALOP = "MALOP|BAD";
    invalidClass.TENLOP = "Ten Lop Bad";
    std::string errReason;
    if (!StorageValidator::validateClass(invalidClass, errReason)) {
        std::cout << "[PASS] Test 1: Rejected invalid Class MALOP containing '|'. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 1: Failed to reject invalid Class MALOP containing '|'.\n";
    }

    // Test 2: Student with empty MASV
    total++;
    SinhVien invalidStudent;
    invalidStudent.MASV = "";
    invalidStudent.HO = "Nguyen";
    invalidStudent.TEN = "An";
    if (!StorageValidator::validateStudent(invalidStudent, "D22CQCN01", errReason)) {
        std::cout << "[PASS] Test 2: Rejected Student with empty MASV. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 2: Failed to reject Student with empty MASV.\n";
    }

    // Test 3: Question with empty option
    total++;
    CauHoi invalidQuestion;
    invalidQuestion.ID = 1;
    invalidQuestion.NOIDUNG = "Cau hoi test";
    invalidQuestion.A = "Dap an A";
    invalidQuestion.B = ""; // empty option B
    invalidQuestion.C = "Dap an C";
    invalidQuestion.D = "Dap an D";
    invalidQuestion.DAPAN_DUNG = 'A';
    if (!StorageValidator::validateQuestion(invalidQuestion, "INT1339", errReason)) {
        std::cout << "[PASS] Test 3: Rejected Question with empty option B. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 3: Failed to reject Question with empty option B.\n";
    }

    // Test 4: Score out of range (15.0)
    // Test 4: Score out of range rejection (> 10.0)
    total++;
    DiemThi invalidScore;
    std::strcpy(invalidScore.MAMH, "INT1339");
    invalidScore.DIEM = 15.0f; // invalid score > 10
    if (!StorageValidator::validateScore("N22DCCN001", invalidScore, errReason)) {
        std::cout << "[PASS] Test 4: Rejected Score out of range (>10.0). Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 4: Failed to reject Score out of range.\n";
    }

    // Test 5: ExamSession with invalid question ID <= 0
    total++;
    ExamSession invalidSession;
    invalidSession.MASV = "N22DCCN001";
    invalidSession.MAMH = "INT1339";
    invalidSession.tongThoiGianPhut = 15;
    invalidSession.questionIds.push_back(-5); // invalid ID
    if (!StorageValidator::validateExamSession(invalidSession, errReason)) {
        std::cout << "[PASS] Test 5: Rejected ExamSession with negative question ID. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 5: Failed to reject ExamSession with negative question ID.\n";
    }

    // Test 6: Control character rejection ('\t' in student name)
    total++;
    SinhVien ctrlStudent;
    ctrlStudent.MASV = "N22DCCN099";
    ctrlStudent.HO = "Nguyen\tVan"; // Tab character
    ctrlStudent.TEN = "An";
    if (!StorageValidator::validateStudent(ctrlStudent, "D22CQCN01", errReason)) {
        std::cout << "[PASS] Test 6: Rejected Student with control character (\\t). Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 6: Failed to reject Student with control character.\n";
    }

    // Test 7: NaN Float Score rejection
    total++;
    DiemThi nanScore;
    std::strcpy(nanScore.MAMH, "INT1339");
    nanScore.DIEM = std::numeric_limits<float>::quiet_NaN();
    if (!StorageValidator::validateScore("N22DCCN001", nanScore, errReason)) {
        std::cout << "[PASS] Test 7: Rejected NaN Float Score. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 7: Failed to reject NaN Float Score.\n";
    }

    // Test 8: Duplicate option rejection (Option A == B after normalization)
    total++;
    CauHoi dupOptionQuestion;
    dupOptionQuestion.ID = 2;
    dupOptionQuestion.NOIDUNG = "Duplicate options test";
    dupOptionQuestion.A = "Gia tri giong nhau";
    dupOptionQuestion.B = "Gia tri giong nhau"; // A == B
    dupOptionQuestion.C = "Dap an C";
    dupOptionQuestion.D = "Dap an D";
    dupOptionQuestion.DAPAN_DUNG = 'A';
    if (!StorageValidator::validateQuestion(dupOptionQuestion, "INT1339", errReason)) {
        std::cout << "[PASS] Test 8: Rejected Question with duplicate options. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 8: Failed to reject Question with duplicate options.\n";
    }

    // Test 9: Multiple space collapsing for TENLOP
    total++;
    std::string inputMultiSpace = "Cong              Nghe Thong         Tin";
    std::string expectedNormSpace = "Cong Nghe Thong Tin";
    std::string actualNormSpace = StringNormalizer::normalizeHumanText(inputMultiSpace);
    if (actualNormSpace == expectedNormSpace) {
        std::cout << "[PASS] Test 9: Successfully collapsed multiple spaces into single space: '" << actualNormSpace << "'\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 9: Space collapsing failed. Expected: '" << expectedNormSpace << "', Got: '" << actualNormSpace << "'\n";
    }

    // Test 10: Tab to space conversion
    total++;
    std::string inputTab = "Lop\tCNTT\t1";
    std::string expectedTab = "Lop CNTT 1";
    std::string actualTab = StringNormalizer::normalizeHumanText(inputTab);
    if (actualTab == expectedTab) {
        std::cout << "[PASS] Test 10: Successfully converted tabs to single spaces: '" << actualTab << "'\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 10: Tab conversion failed. Expected: '" << expectedTab << "', Got: '" << actualTab << "'\n";
    }

    // Test 11: Leading & trailing space trimming
    total++;
    std::string inputTrim = "   Mon Cau Truc Du Lieu   ";
    std::string expectedTrim = "Mon Cau Truc Du Lieu";
    std::string actualTrim = StringNormalizer::normalizeHumanText(inputTrim);
    if (actualTrim == expectedTrim) {
        std::cout << "[PASS] Test 11: Successfully trimmed leading & trailing spaces: '" << actualTrim << "'\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 11: Trimming failed. Expected: '" << expectedTrim << "', Got: '" << actualTrim << "'\n";
    }

    // Test 12: Newline preservation & rejection
    total++;
    std::string inputNewline = "Noi dung\nCau hoi";
    std::string actualNewline = StringNormalizer::normalizeHumanText(inputNewline);
    Lop nlClass; nlClass.MALOP = "MALOP01"; nlClass.TENLOP = actualNewline;
    if (!StorageValidator::validateClass(nlClass, errReason)) {
        std::cout << "[PASS] Test 12: Preserved newline and successfully rejected by Validator. Reason: " << errReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 12: Failed to reject string with newline.\n";
    }

    // Test 13: Password preservation
    total++;
    SinhVien passSv;
    passSv.MASV = "N22DCCN001";
    passSv.HO = "Nguyen";
    passSv.TEN = "An";
    passSv.passsword = "  P@ss 123  ";
    StringNormalizer::normalizeStudent(passSv);
    if (passSv.passsword == "  P@ss 123  ") {
        std::cout << "[PASS] Test 13: Password preserved 100% untouched ('  P@ss 123  ').\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 13: Password was mutated unexpectedly!\n";
    }

    // Test 14: Identifier trim only (MALOP)
    total++;
    std::string malopPad = "  D22CQCN01  ";
    std::string normMalop = trim(malopPad);
    if (normMalop == "D22CQCN01") {
        std::cout << "[PASS] Test 14: Identifier trimmed correctly ('D22CQCN01').\n";
        passed++;
    } else {
        std::cerr << "[FAIL] Test 14: Identifier trim failed.\n";
    }

    // Test 15: Save SystemSettings ON → verify disk matches
    total++;
    {
        bool saved = StorageManager::getInstance().saveSystemSettings(true);
        bool disk = false;
        StorageManager::getInstance().loadSystemSettings(disk);
        if (saved && disk == true) {
            std::cout << "[PASS] Test 15: Save ON persisted correctly (disk=" << disk << ").\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 15: Save ON failed (saved=" << saved << " disk=" << disk << ").\n";
        }
    }

    // Test 16: Save SystemSettings OFF → verify disk matches
    total++;
    {
        bool saved = StorageManager::getInstance().saveSystemSettings(false);
        bool disk = true;
        StorageManager::getInstance().loadSystemSettings(disk);
        if (saved && disk == false) {
            std::cout << "[PASS] Test 16: Save OFF persisted correctly (disk=" << disk << ").\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 16: Save OFF failed (saved=" << saved << " disk=" << disk << ").\n";
        }
    }

    // Test 17: Load after simulated restart
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        bool loaded = false;
        StorageManager::getInstance().loadSystemSettings(loaded);
        if (loaded == true) {
            std::cout << "[PASS] Test 17: Load after restart returns persisted value (true).\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 17: Load after restart returned wrong value (loaded=" << loaded << ").\n";
        }
    }

    // Test 18: Reset storage → SystemSettings.txt = fullscreen=false
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        StorageManager::getInstance().resetToDefault();
        bool loaded = true;
        StorageManager::getInstance().loadSystemSettings(loaded);
        if (loaded == false) {
            std::cout << "[PASS] Test 18: Reset storage sets SystemSettings to false.\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 18: Reset storage did not reset SystemSettings (loaded=" << loaded << ").\n";
        }
    }

    // Test 19: Multiple toggles — final state matches disk
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        StorageManager::getInstance().saveSystemSettings(false);
        StorageManager::getInstance().saveSystemSettings(true);
        StorageManager::getInstance().saveSystemSettings(false);
        bool disk = true;
        StorageManager::getInstance().loadSystemSettings(disk);
        if (disk == false) {
            std::cout << "[PASS] Test 19: Multiple toggles final state correct (disk=false).\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 19: Multiple toggles final state wrong (disk=" << disk << ").\n";
        }
    }

    // Test 20: Verify file format
    total++;
    {
        StorageManager::getInstance().saveSystemSettings(true);
        std::string path = PathResolver::getFilePath("SystemSettings.txt");
        std::ifstream f(path);
        std::string content;
        if (f.is_open()) {
            std::getline(f, content);
            f.close();
        }
        if (content == "fullscreen=true") {
            std::cout << "[PASS] Test 20: File format correct ('fullscreen=true').\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 20: File format wrong (content='" << content << "').\n";
        }
    }

    std::cout << "=== VALIDATION TEST SUMMARY: " << passed << "/" << total << " PASSED ===\n";
    return passed == total;
}
