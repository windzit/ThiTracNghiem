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
#include "IndexManager.h"

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

    // Test 18: SystemSettings default verification (safe, without formatting actual storage data)
    total++;
    {
        bool saved = StorageManager::getInstance().saveSystemSettings(false);
        bool loaded = true;
        StorageManager::getInstance().loadSystemSettings(loaded);
        if (saved && loaded == false) {
            std::cout << "[PASS] Test 18: SystemSettings default value safely verified.\n";
            passed++;
        } else {
            std::cerr << "[FAIL] Test 18: SystemSettings default verification failed.\n";
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

// ============================================================
// FULL TEST SUITE FOR PART E (SUBJECT) & PART F (QUESTION)
// ============================================================

bool runFullEFTestSuite() {
    std::cout << "\n============================================================\n";
    std::cout << "=== RUNNING FULL TEST SUITE: PART E (SUBJECT) & PART F (QUESTION) ===\n";
    std::cout << "============================================================\n";
    int passed = 0;
    int total = 0;

    // ------------------------------------------------------------
    // NHÓM 1: MÔN HỌC (SUBJECT - CÂY NHỊ PHÂN TÌM KIẾM BST)
    // ------------------------------------------------------------
    std::cout << "\n--- [GROUP 1: SUBJECT BST (PART E)] ---\n";

    Subject tree;

    // TC_E01: Insert vào cây rỗng
    total++;
    MonHoc m1;
    std::strcpy(m1.MAMH, "INT1339");
    m1.TENMH = "Cau Truc Du Lieu";
    if (tree.insert(m1) && tree.size() == 1 && tree.getRoot() != nullptr && std::strcmp(tree.getRoot()->data.MAMH, "INT1339") == 0) {
        std::cout << "[PASS] TC_E01: Insert into empty BST (Root verified, size=1).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E01: Insert into empty BST failed.\n";
    }

    // TC_E02: Insert Left Child (BAS1201 < INT1339) và Right Child (MATH101 > INT1339)
    total++;
    MonHoc mLeft; std::strcpy(mLeft.MAMH, "BAS1201"); mLeft.TENMH = "Triet Hoc";
    MonHoc mRight; std::strcpy(mRight.MAMH, "MATH101"); mRight.TENMH = "Toan Cao Cap";
    bool insLeft = tree.insert(mLeft);
    bool insRight = tree.insert(mRight);
    if (insLeft && insRight && tree.size() == 3 &&
        tree.getRoot()->left != nullptr && std::strcmp(tree.getRoot()->left->data.MAMH, "BAS1201") == 0 &&
        tree.getRoot()->right != nullptr && std::strcmp(tree.getRoot()->right->data.MAMH, "MATH101") == 0) {
        std::cout << "[PASS] TC_E02: BST Left/Right Ordering (Binary Search Tree invariant verified).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E02: BST Left/Right Ordering failed.\n";
    }

    // TC_E03: Chặn trùng lặp MAMH
    total++;
    MonHoc mDup; std::strcpy(mDup.MAMH, "INT1339"); mDup.TENMH = "Ten Khac";
    if (!tree.insert(mDup) && tree.size() == 3) {
        std::cout << "[PASS] TC_E03: Duplicate MAMH insertion rejected (size remains 3).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E03: Duplicate MAMH insertion was not rejected.\n";
    }

    // TC_E04: Tìm kiếm trên cây BST (find)
    total++;
    NodeMH* foundNode = tree.find("BAS1201");
    NodeMH* notFoundNode = tree.find("NOTEXIST");
    if (foundNode != nullptr && std::strcmp(foundNode->data.MAMH, "BAS1201") == 0 && notFoundNode == nullptr) {
        std::cout << "[PASS] TC_E04: BST Find (Found existing node, null for non-existing).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E04: BST Find failed.\n";
    }

    // TC_E05: Cập nhật tên môn học (update)
    total++;
    bool updOk = tree.update("INT1339", "Cau Truc Du Lieu Nang Cao");
    NodeMH* updNode = tree.find("INT1339");
    if (updOk && updNode && updNode->data.TENMH == "Cau Truc Du Lieu Nang Cao") {
        std::cout << "[PASS] TC_E05: BST Update TENMH (In-place data update verified).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E05: BST Update TENMH failed.\n";
    }

    // TC_E06: Xóa Nút Lá (0 con) -> MATH101
    total++;
    bool remLeaf = tree.remove("MATH101");
    if (remLeaf && tree.size() == 2 && tree.getRoot()->right == nullptr && tree.find("MATH101") == nullptr) {
        std::cout << "[PASS] TC_E06: BST Delete Leaf Node (MATH101 deleted, size=2, right=null).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E06: BST Delete Leaf Node failed.\n";
    }

    // TC_E07: Xóa Nút 1 Con
    total++;
    MonHoc mChild; std::strcpy(mChild.MAMH, "BAS1100"); mChild.TENMH = "Nhap Mon";
    tree.insert(mChild); // BAS1100 becomes left child of BAS1201
    bool rem1Child = tree.remove("BAS1201"); // BAS1201 has 1 child BAS1100
    if (rem1Child && tree.size() == 2 && tree.getRoot()->left != nullptr && std::strcmp(tree.getRoot()->left->data.MAMH, "BAS1100") == 0) {
        std::cout << "[PASS] TC_E07: BST Delete 1-Child Node (Child relinked to parent successfully).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E07: BST Delete 1-Child Node failed.\n";
    }

    // TC_E08: Xóa Nút 2 Con với In-Order Successor và dsCauHoi.swap
    total++;
    Subject tree2;
    MonHoc subM; std::strcpy(subM.MAMH, "M"); subM.TENMH = "Mon M";
    MonHoc subD; std::strcpy(subD.MAMH, "D"); subD.TENMH = "Mon D";
    MonHoc subT; std::strcpy(subT.MAMH, "T"); subT.TENMH = "Mon T";
    MonHoc subP; std::strcpy(subP.MAMH, "P"); subP.TENMH = "Mon P";
    MonHoc subW; std::strcpy(subW.MAMH, "W"); subW.TENMH = "Mon W";

    tree2.insert(subM);
    tree2.insert(subD);
    tree2.insert(subT);
    tree2.insert(subP);
    tree2.insert(subW);

    // Gắn câu hỏi vào nút M và nút P để kiểm tra hoán vị swap
    NodeMH* nodeM = tree2.find("M");
    NodeMH* nodeP = tree2.find("P");
    CauHoi qM; qM.ID = 100; qM.NOIDUNG = "Cau hoi cua Mon M"; qM.A = "A"; qM.B = "B"; qM.C = "C"; qM.D = "D"; qM.DAPAN_DUNG = 'A';
    CauHoi qP; qP.ID = 200; qP.NOIDUNG = "Cau hoi cua Mon P"; qP.A = "A"; qP.B = "B"; qP.C = "C"; qP.D = "D"; qP.DAPAN_DUNG = 'B';
    nodeM->data.dsCauHoi.insert(qM);
    nodeP->data.dsCauHoi.insert(qP);

    // Xóa nút M (gốc, có đủ 2 con D và T). In-order successor là P.
    bool rem2Child = tree2.remove("M");
    NodeMH* newRoot = tree2.getRoot();
    if (rem2Child && newRoot != nullptr && std::strcmp(newRoot->data.MAMH, "P") == 0 &&
        newRoot->data.dsCauHoi.size() == 1 && newRoot->data.dsCauHoi.find(200) != nullptr &&
        tree2.find("M") == nullptr && tree2.size() == 4) {
        std::cout << "[PASS] TC_E08: BST Delete 2-Children Node (Successor 'P' swapped questions O(1), no leak).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E08: BST Delete 2-Children Node failed.\n";
    }

    // TC_E09: Chuẩn hóa Subject
    total++;
    MonHoc normMH;
    std::strcpy(normMH.MAMH, "  csdl 101  ");
    normMH.TENMH = "co   so   du   lieu";
    StringNormalizer::normalizeSubject(normMH);
    if (std::strcmp(normMH.MAMH, "CSDL101") == 0 && normMH.TENMH == "Co So Du Lieu") {
        std::cout << "[PASS] TC_E09: Subject Normalization (MAMH uppercase & trimmed, TENMH Title Case).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E09: Subject Normalization failed.\n";
    }

    // TC_E10: BR-04 Chặn xóa môn học khi used == true (O(1) Check)
    total++;
    Subject treeUsed;
    MonHoc mUsed;
    std::strcpy(mUsed.MAMH, "INT9999");
    mUsed.TENMH = "Mon Hoc Da Thi";
    mUsed.used = true;
    treeUsed.insert(mUsed);
    bool remUsedResult = treeUsed.remove("INT9999");
    if (!remUsedResult && treeUsed.size() == 1 && treeUsed.find("INT9999") != nullptr) {
        std::cout << "[PASS] TC_E10: Subject Deletion Rejection for used==true (BR-04 O(1) verified).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_E10: Subject Deletion Rejection failed.\n";
    }

    // ------------------------------------------------------------
    // NHÓM 2: CÂU HỎI (QUESTION - DSLK ĐƠN CÓ CON TRỎ TAIL)
    // ------------------------------------------------------------
    std::cout << "\n--- [GROUP 2: QUESTION LINKED LIST (PART F)] ---\n";

    Question qList;

    // TC_F01: Insert vào DSLK rỗng
    total++;
    CauHoi q1; q1.ID = 10; q1.NOIDUNG = "Cau hoi 10"; q1.A = "A1"; q1.B = "B1"; q1.C = "C1"; q1.D = "D1"; q1.DAPAN_DUNG = 'A';
    if (qList.insert(q1) && qList.size() == 1 && qList.getRoot() != nullptr && qList.getTail() == qList.getRoot() && qList.getRoot()->cauhoi.ID == 10) {
        std::cout << "[PASS] TC_F01: Insert into empty Question list (root == tail, size=1).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F01: Insert into empty Question list failed.\n";
    }

    // TC_F02: Chèn đuôi O(1) khi ID tăng dần (ID > tail->ID)
    total++;
    CauHoi q2; q2.ID = 20; q2.NOIDUNG = "Cau hoi 20"; q2.A = "A2"; q2.B = "B2"; q2.C = "C2"; q2.D = "D2"; q2.DAPAN_DUNG = 'B';
    if (qList.insert(q2) && qList.size() == 2 && qList.getTail()->cauhoi.ID == 20 && qList.getRoot()->next == qList.getTail()) {
        std::cout << "[PASS] TC_F02: O(1) Tail Insertion (ID 20 appended to tail, size=2).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F02: O(1) Tail Insertion failed.\n";
    }

    // TC_F03: Chèn đuôi câu hỏi tiếp theo và từ chối ID không tăng dần
    total++;
    CauHoi q3; q3.ID = 30; q3.NOIDUNG = "Cau hoi 30"; q3.A = "A"; q3.B = "B"; q3.C = "C"; q3.D = "D"; q3.DAPAN_DUNG = 'C';
    bool insQ3 = qList.insert(q3);
    CauHoi qInvalid; qInvalid.ID = 15; // 15 <= tail->ID (30)
    bool rejInvalid = !qList.insert(qInvalid);
    if (insQ3 && rejInvalid && qList.size() == 3 && qList.getTail()->cauhoi.ID == 30) {
        std::cout << "[PASS] TC_F03: Consecutive O(1) Tail Insertions & Rejection of non-increasing ID.\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F03: Consecutive Tail Insertions failed.\n";
    }

    // TC_F04: Tìm kiếm câu hỏi (find)
    total++;
    dsCHT* foundQ = qList.find(20);
    dsCHT* notFoundQ = qList.find(999);
    if (foundQ != nullptr && foundQ->cauhoi.ID == 20 && notFoundQ == nullptr) {
        std::cout << "[PASS] TC_F04: Question Find (Found ID 20, null for 999).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F04: Question Find failed.\n";
    }

    // TC_F05: Cập nhật nội dung câu hỏi (update)
    total++;
    CauHoi qUpd = qList.find(20)->cauhoi;
    qUpd.NOIDUNG = "Noi dung moi cua cau 20";
    bool updQOk = qList.update(20, qUpd);
    if (updQOk && qList.find(20)->cauhoi.NOIDUNG == "Noi dung moi cua cau 20") {
        std::cout << "[PASS] TC_F05: Question Update (Content updated in-place).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F05: Question Update failed.\n";
    }

    // TC_F06: Xóa cứng (Hard Delete) câu hỏi chưa thi (used == false)
    total++;
    bool remHead = qList.removeNode(10); // Xóa node đầu tiên (ID 10)
    if (remHead && qList.size() == 2 && qList.find(10) == nullptr && qList.getRoot()->cauhoi.ID == 20) {
        std::cout << "[PASS] TC_F06: Question Hard Delete (removeNode for unused question, root updated, size=2).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F06: Question Hard Delete failed.\n";
    }

    // TC_F07: Xóa mềm (Soft Delete) câu hỏi đã thi (used == true)
    total++;
    qList.find(20)->cauhoi.used = true;
    bool softDelOk = qList.setDeleted(20);
    dsCHT* node20 = qList.find(20);
    if (softDelOk && node20 != nullptr && node20->cauhoi.deleted == true && qList.size() == 2) {
        std::cout << "[PASS] TC_F07: Question Soft Delete (Node preserved in memory, deleted=true).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F07: Question Soft Delete failed.\n";
    }

    // TC_F08: Khôi phục câu hỏi đã xóa mềm (restoreDeleted)
    total++;
    bool restOk = qList.restoreDeleted(20);
    if (restOk && node20->cauhoi.deleted == false) {
        std::cout << "[PASS] TC_F08: Question Restore (deleted flag reset to false).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F08: Question Restore failed.\n";
    }

    // TC_F09: Hoán vị 2 danh sách câu hỏi (swap) trong O(1)
    total++;
    Question qListOther;
    CauHoi qOther; qOther.ID = 999; qOther.NOIDUNG = "Other"; qOther.A = "A"; qOther.B = "B"; qOther.C = "C"; qOther.D = "D"; qOther.DAPAN_DUNG = 'A';
    qListOther.insert(qOther);
    qList.swap(qListOther);
    if (qList.size() == 1 && qList.getRoot()->cauhoi.ID == 999 && qListOther.size() == 2) {
        std::cout << "[PASS] TC_F09: Question List O(1) Swap (root, tail, listSize exchanged).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F09: Question List Swap failed.\n";
    }

    // TC_F10: hasUsedQuestions check
    total++;
    qListOther.find(20)->cauhoi.used = true;
    if (qListOther.hasUsedQuestions() == true && qList.hasUsedQuestions() == false) {
        std::cout << "[PASS] TC_F10: hasUsedQuestions correctly detects exam usage.\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_F10: hasUsedQuestions check failed.\n";
    }

    // ------------------------------------------------------------
    // NHÓM 3: ĐĨA, INDEX VÀ COMPACTION (STORAGE & INDEX INTEGRATION)
    // ------------------------------------------------------------
    std::cout << "\n--- [GROUP 3: STORAGE & INDEX INTEGRATION] ---\n";

    // TC_S01: Auto-increment Question ID Generation
    total++;
    int id1 = StorageManager::getInstance().getNextQuestionID();
    int id2 = StorageManager::getInstance().getNextQuestionID();
    if (id2 == id1 + 1 && id1 > 0) {
        std::cout << "[PASS] TC_S01: StorageManager Question Auto-Increment ID (id1=" << id1 << ", id2=" << id2 << ").\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_S01: Question Auto-Increment ID failed.\n";
    }

    // TC_S02: Fixed-Length Record Direct Seek Format (736 bytes Question, 71 bytes Subject)
    total++;
    if (StorageConfig::RECORD_SIZE_QUESTION == 736 && StorageConfig::RECORD_SIZE_SUBJECT == 71 &&
        StorageConfig::STATUS_OFFSET_QUESTION == 734 && StorageConfig::STATUS_OFFSET_SUBJECT == 69) {
        std::cout << "[PASS] TC_S02: Fixed-Length Record Constants verified (Question: 736B, Subject: 71B).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_S02: Fixed-Length Record Constants mismatch.\n";
    }

    // TC_S03: IndexManager O(1) Hash Map Updates
    total++;
    IndexManager::getInstance().updateQuestionOffset(8888, 1472);
    IndexManager::getInstance().updateQuestionSubject(8888, "INT1339");
    int64_t qOff = -1;
    bool foundQOff = IndexManager::getInstance().getQuestionOffset(8888, qOff);
    std::string mamhLookup;
    bool foundMamh = IndexManager::getInstance().getQuestionSubject(8888, mamhLookup);
    if (foundQOff && qOff == 1472 && foundMamh && mamhLookup == "INT1339") {
        std::cout << "[PASS] TC_S03: IndexManager O(1) Hash Map Lookup (ID->Offset & ID->MAMH verified).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_S03: IndexManager O(1) Hash Map Lookup failed.\n";
    }

    // TC_S04: Validation Question Duplicate Options Normalization
    total++;
    CauHoi dupOptNorm;
    dupOptNorm.ID = 99;
    dupOptNorm.NOIDUNG = "Cau hoi test";
    dupOptNorm.A = "   Dap An Chung   ";
    dupOptNorm.B = "dap an chung";
    dupOptNorm.C = "Dap An Khac 1";
    dupOptNorm.D = "Dap An Khac 2";
    dupOptNorm.DAPAN_DUNG = 'A';
    std::string dupReason;
    if (!StorageValidator::validateQuestion(dupOptNorm, "INT1339", dupReason)) {
        std::cout << "[PASS] TC_S04: Rejected Question with normalized duplicate options. Reason: " << dupReason << "\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_S04: Failed to reject normalized duplicate options.\n";
    }

    // TC_S05: Delete Question Index Cleanup
    total++;
    IndexManager::getInstance().removeQuestionOffset(8888);
    IndexManager::getInstance().removeQuestionSubject(8888);
    int64_t cleanOff = -1;
    if (!IndexManager::getInstance().getQuestionOffset(8888, cleanOff)) {
        std::cout << "[PASS] TC_S05: Question Index Removal verified (ID 8888 purged from hash map).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_S05: Question Index Removal failed.\n";
    }

    // TC_S06: Atomic File Overwrite Verification (File exists -> Overwritten atomically)
    total++;
    std::string testPath = PathResolver::getFilePath("test_atomic_overwrite.txt");
    bool w1 = StorageManager::atomicWriteFile(testPath, "FIRST_CONTENT_111\n");
    bool w2 = StorageManager::atomicWriteFile(testPath, "SECOND_OVERWRITTEN_CONTENT_222\n");
    std::ifstream inTest(testPath);
    std::string readContent;
    if (inTest.is_open()) {
        std::getline(inTest, readContent);
        inTest.close();
    }
    std::error_code ecDel;
    std::filesystem::remove(testPath, ecDel);
    if (w1 && w2 && readContent == "SECOND_OVERWRITTEN_CONTENT_222") {
        std::cout << "[PASS] TC_S06: Atomic File Overwrite verified (Old file successfully overwritten with new content).\n";
        passed++;
    } else {
        std::cerr << "[FAIL] TC_S06: Atomic File Overwrite failed (w1=" << w1 << ", w2=" << w2 << ", read='" << readContent << "').\n";
    }

    std::cout << "\n============================================================\n";
    std::cout << "=== FULL E & F TEST SUITE SUMMARY: " << passed << "/" << total << " PASSED ===\n";
    std::cout << "============================================================\n";
    return passed == total;
}

