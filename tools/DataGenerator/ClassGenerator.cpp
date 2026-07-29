#include "GeneratorConfig.h"
#include "Random.h"
#include "../../include/httplib.h"
#include "../../include/nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

static std::string getPtitClassName(const std::string& malop, int i) {
    std::string majorName = "CNTT";
    if (malop.find("VT") != std::string::npos) majorName = "Vien thong";
    else if (malop.find("AT") != std::string::npos) majorName = "An toan thong tin";
    else if (malop.find("MR") != std::string::npos) majorName = "Mang may tinh";
    else if (malop.find("KT") != std::string::npos) majorName = "Kien thuc nganh";
    else if (malop.find("QT") != std::string::npos) majorName = "Quan tri dich vu";

    std::string year = "2022";
    if (malop.length() >= 3 && std::isdigit(malop[1]) && std::isdigit(malop[2])) {
        year = "20" + malop.substr(1, 2);
    }

    int classNum = (i % 5) + 1;
    return "Lop " + majorName + " " + std::to_string(classNum) + " Khoa " + year;
}

bool generateClasses(httplib::Client& cli, const GeneratorConfig& cfg, std::vector<std::string>& outClassCodes) {
    std::cout << "[ClassGenerator] Generating " << cfg.classes << " PTIT classes...\n";
    outClassCodes.clear();
    outClassCodes.reserve(cfg.classes);

    int successCount = 0;
    for (int i = 0; i < cfg.classes; i++) {
        std::string malop = Random::getClassCode(i);
        std::string tenlop = getPtitClassName(malop, i);

        json body = {
            {"malop", malop},
            {"tenlop", tenlop}
        };

        auto res = cli.Post("/api/classes", body.dump(), "application/json");
        if (res && (res->status == 201 || res->status == 200)) {
            outClassCodes.push_back(malop);
            successCount++;
        } else {
            std::cerr << "[ClassGenerator] Error creating class " << malop << "\n";
        }

        if ((i + 1) % 100 == 0 || i + 1 == cfg.classes) {
            std::cout << "  Progress: " << (i + 1) << "/" << cfg.classes << " classes created.\n";
        }
    }
    std::cout << "[ClassGenerator] Done. " << successCount << " PTIT classes created.\n";
    return successCount > 0;
}
