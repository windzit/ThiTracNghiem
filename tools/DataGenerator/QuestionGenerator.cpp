#include "GeneratorConfig.h"
#include "Random.h"
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

bool generateQuestions(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<std::string>& subjectCodes) {
    int totalQuestions = (int)subjectCodes.size() * cfg.questionsPerSubject;
    std::cout << "[QuestionGenerator] Generating " << cfg.questionsPerSubject
              << " questions per subject (" << totalQuestions << " total)...\n";

    int successCount = 0;
    int qCounter = 1;

    for (const auto& mamh : subjectCodes) {
        for (int q = 0; q < cfg.questionsPerSubject; q++) {
            std::string noidung = "Cau hoi " + std::to_string(q + 1) + " cua mon " + mamh;
            std::string a = "Phuong an A - " + std::to_string(qCounter * 4 + 1);
            std::string b = "Phuong an B - " + std::to_string(qCounter * 4 + 2);
            std::string c = "Phuong an C - " + std::to_string(qCounter * 4 + 3);
            std::string d = "Phuong an D - " + std::to_string(qCounter * 4 + 4);
            std::string dapan(1, Random::getRandomOption());

            json body = {
                {"mamh", mamh},
                {"noidung", noidung},
                {"a", a},
                {"b", b},
                {"c", c},
                {"d", d},
                {"dapan", dapan}
            };

            auto res = cli.Post("/api/questions", body.dump(), "application/json");
            if (res && (res->status == 201 || res->status == 200)) {
                successCount++;
            } else {
                std::cerr << "[QuestionGenerator] Error creating question for subject " << mamh << "\n";
            }
            qCounter++;

            if (successCount % 500 == 0 || successCount == totalQuestions) {
                std::cout << "  Progress: " << successCount << "/" << totalQuestions << " questions created.\n";
            }
        }
    }
    std::cout << "[QuestionGenerator] Done. " << successCount << " questions created.\n";
    return successCount > 0;
}
