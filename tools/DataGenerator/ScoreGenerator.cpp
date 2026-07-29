#include "GeneratorConfig.h"
#include "Random.h"
#include "StudentGenerator.h"
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

bool generateExamsAndScores(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<StudentInfo>& students, const std::vector<std::string>& subjectCodes) {
    if (students.empty() || subjectCodes.empty()) {
        std::cout << "[ScoreGenerator] Skipping score generation (no students or subjects).\n";
        return true;
    }

    int totalExamsToRun = (int)students.size() * cfg.scoresPerStudent;
    std::cout << "[ScoreGenerator] Simulating " << cfg.scoresPerStudent
              << " exam sessions per student (" << totalExamsToRun << " total exams)...\n";

    int completedCount = 0;

    for (size_t sIdx = 0; sIdx < students.size(); sIdx++) {
        const std::string& masv = students[sIdx].masv;

        for (int e = 0; e < cfg.scoresPerStudent; e++) {
            std::string mamh = subjectCodes[(sIdx + e) % subjectCodes.size()];
            int socau = std::min(cfg.questionsPerSubject, 10);
            if (socau <= 0) socau = 5;

            // 1. Start exam
            json startBody = {
                {"masv", masv},
                {"mamh", mamh},
                {"socau", socau},
                {"thoigian", 15}
            };

            auto startRes = cli.Post("/api/exams/start", startBody.dump(), "application/json");
            if (!startRes || (startRes->status != 200 && startRes->status != 201)) {
                continue;
            }

            json answersArr = json::array();
            try {
                json resJson = json::parse(startRes->body);
                if (resJson.contains("questions") && resJson["questions"].is_array()) {
                    // 2. Answer questions
                    for (const auto& qObj : resJson["questions"]) {
                        int qId = qObj.value("id", 0);
                        std::string ans(1, Random::getRandomOption());

                        json ansBody = {
                            {"masv", masv},
                            {"mamh", mamh},
                            {"questionId", qId},
                            {"answer", ans}
                        };
                        cli.Put("/api/exams/answer", ansBody.dump(), "application/json");

                        answersArr.push_back({
                            {"id", qId},
                            {"answer", ans}
                        });
                    }
                }
            } catch (...) {}

            // 3. Submit exam with answers array
            json submitBody = {
                {"masv", masv},
                {"mamh", mamh},
                {"answers", answersArr}
            };
            auto subRes = cli.Post("/api/exams/submit", submitBody.dump(), "application/json");
            if (subRes && (subRes->status == 200 || subRes->status == 201)) {
                completedCount++;
            }

            if (completedCount % 500 == 0 || completedCount == totalExamsToRun) {
                std::cout << "  Progress: " << completedCount << "/" << totalExamsToRun << " exams completed.\n";
            }
        }
    }

    std::cout << "[ScoreGenerator] Done. " << completedCount << " exams completed and scored via API.\n";
    return true;
}
