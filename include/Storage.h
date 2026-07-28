#pragma once

#include "Class.h"
#include "Subject.h"

// Load all data from disk at startup with benchmarks & diagnostics.
bool LoadAllData(Class& dsl, Subject& dsmh);

// Save all data to disk at shutdown.
bool SaveAllData(Class& dsl, Subject& dsmh);

// Print startup loading report, consistency audit, and dev mode notice.
void PrintStartupReport(Class& dsl, Subject& dsmh, long long totalLoadingTimeMs, long long classLoadMs, long long subjectLoadMs);
