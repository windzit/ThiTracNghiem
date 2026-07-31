#pragma once
#include <string>
#include <cstring>    // strcmp
#include <fstream>    // ifstream, ofstream
#include <sstream>    // stringstream
#include <filesystem>
#include <iostream>
#include <cstddef>

constexpr int MAXLOP = 10000;

namespace StorageConfig {
    // Fixed-Length Record Sizes (including '|' delimiters and '\n')
    constexpr size_t RECORD_SIZE_STUDENT  = 134;
    constexpr size_t RECORD_SIZE_QUESTION = 736;
    constexpr size_t RECORD_SIZE_SUBJECT  = 71;
    constexpr size_t RECORD_SIZE_CLASS    = 69;
    constexpr size_t RECORD_SIZE_SCORE    = 35;

    // Relative byte offsets for status flag within each record (0-indexed)
    constexpr size_t STATUS_OFFSET_STUDENT  = 132; // 134 - 2
    constexpr size_t STATUS_OFFSET_QUESTION = 734; // 736 - 2
    constexpr size_t STATUS_OFFSET_SUBJECT  = 69;  // 71 - 2
    constexpr size_t STATUS_OFFSET_CLASS    = 67;  // 69 - 2

    // Record Status Flags
    constexpr char STATUS_ACTIVE       = '0'; // Active record
    constexpr char STATUS_DELETED      = '1'; // Logically deleted record (eligible for compaction)
    constexpr char STATUS_SOFT_DELETED = '2'; // Soft-deleted question (used in exam history)

    // Storage Compaction Thresholds & Ratios
    constexpr int STUDENT_COMPACT_COUNT  = 50;
    constexpr int QUESTION_COMPACT_COUNT = 50;
    constexpr int SUBJECT_COMPACT_COUNT  = 20;
    constexpr int CLASS_COMPACT_COUNT    = 10;

    constexpr double STUDENT_COMPACT_RATIO  = 0.25; // 25% deleted records trigger compaction
    constexpr double QUESTION_COMPACT_RATIO = 0.25;
    constexpr double SUBJECT_COMPACT_RATIO  = 0.25;
    constexpr double CLASS_COMPACT_RATIO    = 0.25;

    // Formatting Delimiters & Padding
    constexpr char FIELD_DELIMITER      = '|';
    constexpr char RECORD_NEWLINE       = '\n';
    constexpr char RECORD_PADDING_CHAR  = ' ';
}

// Aliased global constants for direct usage
constexpr size_t RECORD_SIZE_STUDENT  = StorageConfig::RECORD_SIZE_STUDENT;
constexpr size_t RECORD_SIZE_QUESTION = StorageConfig::RECORD_SIZE_QUESTION;
constexpr size_t RECORD_SIZE_SUBJECT  = StorageConfig::RECORD_SIZE_SUBJECT;
constexpr size_t RECORD_SIZE_CLASS    = StorageConfig::RECORD_SIZE_CLASS;
constexpr size_t RECORD_SIZE_SCORE    = StorageConfig::RECORD_SIZE_SCORE;

constexpr char STATUS_ACTIVE       = StorageConfig::STATUS_ACTIVE;
constexpr char STATUS_DELETED      = StorageConfig::STATUS_DELETED;
constexpr char STATUS_SOFT_DELETED = StorageConfig::STATUS_SOFT_DELETED;