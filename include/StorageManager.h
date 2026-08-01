#pragma once
#include "Class.h"
#include "Subject.h"
#include "Exam.h"
#include "PathResolver.h"

// Out of Scope Notice: Hash Table tra cuu sinh vien (MASV -> Student*)
// thuoc ngoai pham vi cua dot nay - se trien khai o giai doan toi uu tra cuu rieng.

class StorageManager {
public:
    static StorageManager& getInstance();

    // System Initialization & Lifecycle
    bool initializeStorage();
    bool loadAllData(Class& dsl, Subject& dsmh);
    bool saveAllData(Class& dsl, Subject& dsmh);

    // Entity Specific Load/Save
    bool loadClasses(Class& dsl);
    bool saveClasses(Class& dsl);
    
    bool loadStudents(Class& dsl);
    bool saveStudents(Class& dsl);

    bool loadSubjects(Subject& dsmh);
    bool saveSubjects(Subject& dsmh);

    bool loadQuestions(Subject& dsmh);
    bool saveQuestions(Subject& dsmh);

    bool loadScores(Class& dsl);
    bool saveScores(Class& dsl);

    bool loadExamSessions(DArray<ExamSession>& sessions);
    bool saveExamSession(const ExamSession& session);
    bool removeExamSession(const std::string& masv);
    bool appendExamHistory(const ExamSession& session, float diem);
    int getActiveSessionCount() const;

    // System Settings (lightweight: single boolean, no Validator/Verifier class needed)
    bool loadSystemSettings(bool& outFullscreenRequired);
    bool saveSystemSettings(bool fullscreenRequired);

    // Save Trigger Policy Interface
    void markDirty();
    bool isDirty() const;
    void incrementOpCount();
    bool checkAndSaveAuto(Class& dsl, Subject& dsmh);

    // Question ID Auto Increment
    int getNextQuestionID();
    bool saveMetadata();

    // Derived Used Flags Rebuild Strategy
    void rebuildUsedFlags(Subject& dsmh);

    // Index Manager Acceleration Coordination
    bool rebuildIndexes();
    bool syncIndexes();

    // Atomic Safe File Writer
    static bool atomicWriteFile(const std::string& targetPath, const std::string& content);

    // Fixed-Length Record Direct seekg() / seekp() Operations
    bool readStudentAt(int64_t offset, SinhVien& outSv, std::string& outMalop);
    bool writeStudentAt(int64_t offset, const SinhVien& sv, const std::string& malop, char status = '0');
    bool appendStudent(const SinhVien& sv, const std::string& malop, int64_t& outOffset);
    bool markStudentStatusAt(int64_t offset, char status);

    bool readQuestionAt(int64_t offset, CauHoi& outQ, std::string& outMamh);
    bool writeQuestionAt(int64_t offset, const CauHoi& q, const std::string& mamh, char status = '0');
    bool appendQuestion(const CauHoi& q, const std::string& mamh, int64_t& outOffset);
    bool markQuestionStatusAt(int64_t offset, char status);

    bool readSubjectAt(int64_t offset, MonHoc& outMh);
    bool writeSubjectAt(int64_t offset, const MonHoc& mh, char status = '0');
    bool appendSubject(const MonHoc& mh, int64_t& outOffset);
    bool markSubjectStatusAt(int64_t offset, char status);

    bool readClassAt(int64_t offset, Lop& outLop);
    bool writeClassAt(int64_t offset, const Lop& lop, char status = '0');
    bool appendClass(const Lop& lop, int64_t& outOffset);
    bool markClassStatusAt(int64_t offset, char status);

    // Storage Compaction Engine
    bool compactStudents();
    bool compactQuestions();
    bool compactSubjects();
    bool compactClasses();
    bool compactAll();

    // Compaction Triggers & Metadata Tracking
    bool checkAndExecuteStartupCompaction();
    void incrementDeletedCount(const std::string& entityType);
    int getDeletedCount(const std::string& entityType) const;
    void resetDeletedCount(const std::string& entityType);


    // Reset storage to empty default state (9 header-only files)
    // Auto-backup current storage/ to storage_backup_reset_<timestamp>/ before overwrite
    bool resetToDefault();


private:
    StorageManager() = default;
    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;

    bool dirty = false;
    int opCount = 0;
    const int SAVE_THRESHOLD = 100;
};