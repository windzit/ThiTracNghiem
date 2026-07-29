#include "GeneratorConfig.h"
#include "StudentGenerator.h"
#include "Random.h"
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

bool generateHistory(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<StudentInfo>& students, const std::vector<std::string>& subjectCodes) {
    int extraHistoryPerStudent = cfg.historyPerStudent - cfg.scoresPerStudent;
    if (extraHistoryPerStudent <= 0 || students.empty() || subjectCodes.empty()) {
        std::cout << "[HistoryGenerator] Score generator fulfilled history requirements (" << cfg.scoresPerStudent << "/" << cfg.historyPerStudent << ").\n";
        return true;
    }

    int totalExtraHistory = (int)students.size() * extraHistoryPerStudent;
    std::cout << "[HistoryGenerator] Simulating " << extraHistoryPerStudent
              << " additional exam history sessions per student (" << totalExtraHistory << " total)...\n";

    int completedCount = 0;

    for (size_t sIdx = 0; sIdx < students.size(); sIdx++) {
        const std::string& masv = students[sIdx].masv;

        for (int h = 0; h < extraHistoryPerStudent; h++) {
            std::string mamh = subjectCodes[(sIdx + cfg.scoresPerStudent + h) % subjectCodes.size()];
            int socau = std::min(cfg.questionsPerSubject, 10);
            if (socau <= 0) socau = 5;

            // Start -> Answer -> Submit to generate valid history entry
            json startBody = {
                {"masv", masv},
                {"mamh", mamh},
                {"socau", socau},
                {"thoigian", 15}
            };
            auto startRes = cli.Post("/api/exams/start", startBody.dump(), "application/json");
            json answersArr = json::array();
            if (startRes && (startRes->status == 200 || startRes->status == 201)) {
                try {
                    json resJson = json::parse(startRes->body);
                    if (resJson.contains("questions") && resJson["questions"].is_array()) {
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
            }

            json submitBody = {
                {"masv", masv},
                {"mamh", mamh},
                {"answers", answersArr}
            };
            auto subRes = cli.Post("/api/exams/submit", submitBody.dump(), "application/json");
            if (subRes && (subRes->status == 200 || subRes->status == 201)) {
                completedCount++;
            }

            if (completedCount % 500 == 0 || completedCount == totalExtraHistory) {
                std::cout << "  Progress: " << completedCount << "/" << totalExtraHistory << " additional history sessions completed.\n";
            }
        }
    }

    std::cout << "[HistoryGenerator] Done. " << completedCount << " additional exam history sessions generated.\n";
    return true;
}
