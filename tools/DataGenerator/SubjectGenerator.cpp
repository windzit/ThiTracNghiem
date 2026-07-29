#include "GeneratorConfig.h"
#include "Random.h"
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

bool generateSubjects(httplib::Client& cli, const GeneratorConfig& cfg, std::vector<std::string>& outSubjectCodes) {
    std::cout << "[SubjectGenerator] Generating " << cfg.subjects << " subjects...\n";
    outSubjectCodes.clear();
    outSubjectCodes.reserve(cfg.subjects);

    int successCount = 0;
    for (int i = 0; i < cfg.subjects; i++) {
        std::string mamh = Random::getSubjectCode(i);
        std::string tenmh = Random::getSubjectName(i);

        json body = {
            {"mamh", mamh},
            {"tenmh", tenmh}
        };

        auto res = cli.Post("/api/subjects", body.dump(), "application/json");
        if (res && (res->status == 201 || res->status == 200)) {
            outSubjectCodes.push_back(mamh);
            successCount++;
        } else {
            std::cerr << "[SubjectGenerator] Error creating subject " << mamh << "\n";
        }

        if ((i + 1) % 50 == 0 || i + 1 == cfg.subjects) {
            std::cout << "  Progress: " << (i + 1) << "/" << cfg.subjects << " subjects created.\n";
        }
    }
    std::cout << "[SubjectGenerator] Done. " << successCount << " subjects created.\n";
    return successCount > 0;
}
