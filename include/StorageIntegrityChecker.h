#pragma once
#include <string>
#include "Class.h"
#include "Subject.h"
#include "Exam.h"
#include "DArray.h"

class StorageIntegrityChecker {
public:
    // Runs foreign key orphan checks on RAM structures after startup data load
    static bool auditStorageIntegrity(Class& dsl, Subject& dsmh, const DArray<ExamSession>& sessions);
};
