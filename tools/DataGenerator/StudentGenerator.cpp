#include "GeneratorConfig.h"
#include "StudentGenerator.h"
#include "Random.h"
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

bool generateStudents(httplib::Client& cli, const GeneratorConfig& cfg, const std::vector<std::string>& classCodes, std::vector<StudentInfo>& outStudents) {
    std::cout << "[StudentGenerator] Generating " << cfg.students << " students across " << classCodes.size() << " classes...\n";
    outStudents.clear();
    outStudents.reserve(cfg.students);

    if (classCodes.empty()) {
        std::cerr << "[StudentGenerator] Error: No class codes available.\n";
        return false;
    }

    int successCount = 0;
    for (int i = 0; i < cfg.students; i++) {
        std::string masv = Random::getStudentCode(i);
        std::string malop = classCodes[i % classCodes.size()];
        std::string gender = Random::getRandomGender();
        bool isFemale = (gender == "Nu");
        std::string ho = Random::getRandomLastName();
        std::string ten = Random::getRandomFirstName(isFemale);

        json body = {
            {"malop", malop},
            {"masv", masv},
            {"ho", ho},
            {"ten", ten},
            {"phai", gender},
            {"password", "123"}
        };

        auto res = cli.Post("/api/students", body.dump(), "application/json");
        if (res && (res->status == 201 || res->status == 200)) {
            outStudents.push_back({masv, malop});
            successCount++;
        } else {
            std::cerr << "[StudentGenerator] Error creating student " << masv
                      << " | Status: " << (res ? res->status : 0)
                      << " | Body: " << (res ? res->body : "No response") << "\n";
        }

        if ((i + 1) % 1000 == 0 || i + 1 == cfg.students) {
            std::cout << "  Progress: " << (i + 1) << "/" << cfg.students << " students created.\n";
        }
    }
    std::cout << "[StudentGenerator] Done. " << successCount << " students created.\n";
    return successCount > 0;
}
