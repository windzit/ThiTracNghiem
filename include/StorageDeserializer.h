#pragma once
#include <string>
#include "Class.h"
#include "Subject.h"
#include "Exam.h"
#include "DArray.h"

class StorageDeserializer {
public:
    static bool parseClassesFile(const std::string& filePath, DArray<Lop>& result);
    static bool parseStudentsFile(const std::string& filePath, DArray<SinhVien>& result, DArray<std::string>& classCodes);
    static bool parseSubjectsFile(const std::string& filePath, DArray<MonHoc>& result);
    static bool parseQuestionsFile(const std::string& filePath, DArray<CauHoi>& result, DArray<std::string>& subjectCodes);
    static bool parseScoresFile(const std::string& filePath, DArray<DiemThi>& result, DArray<std::string>& masvList);
    static bool parseExamSessionsFile(const std::string& filePath, DArray<ExamSession>& result);
};
