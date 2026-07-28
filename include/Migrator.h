#pragma once
#include "Class.h"
#include "Subject.h"
#include "Exam.h"
#include <string>

class Migrator {
public:
    // Main entry point to run data migration
    static bool runMigration(Class& dsl, Subject& dsmh);

    // Creates backup of legacy storage
    static bool backupStorage();

    // Verifies integrity of migrated flat files against RAM data
    static bool verifyMigration(Class& dsl, Subject& dsmh);

    // Restores legacy storage from backup if verification fails
    static bool rollbackMigration();
};
