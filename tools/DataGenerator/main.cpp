#include "GeneratorConfig.h"
#include "StudentGenerator.h"
#include "Random.h"
#include "../../include/httplib.h"
#include <iostream>
#include <string>
#include <vector>

void printUsage() {
    std::cout << "=== PTIT ThiTracNghiem Standalone Data Generator ===\n"
              << "Usage:\n"
              << "  DataGenerator.exe --preset <name> [--host <host>] [--port <port>]\n\n";
    PresetManager::getInstance().printAvailablePresets();
}

int main(int argc, char* argv[]) {
    Random::init();

    std::string presetName = "verify";
    std::string host = "localhost";
    int port = 8080;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--preset" && i + 1 < argc) {
            presetName = argv[++i];
        } else if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        }
    }

    GeneratorConfig cfg;
    if (!PresetManager::getInstance().loadPreset(presetName, cfg)) {
        std::cerr << "[Error] Unknown preset: '" << presetName << "'\n\n";
        printUsage();
        return 1;
    }

    std::cout << "=========================================================\n"
              << "          PTIT THITRACNGHIEM DATA GENERATOR             \n"
              << "=========================================================\n"
              << "Target API Server : http://" << host << ":" << port << "\n"
              << "Preset Selected   : " << cfg.name << "\n"
              << "  Classes         : " << cfg.classes << "\n"
              << "  Students        : " << cfg.students << "\n"
              << "  Subjects        : " << cfg.subjects << "\n"
              << "  Questions/Sub   : " << cfg.questionsPerSubject << "\n"
              << "  Scores/Student  : " << cfg.scoresPerStudent << "\n"
              << "  History/Student : " << cfg.historyPerStudent << "\n"
              << "=========================================================\n\n";

    httplib::Client cli(host, port);
    cli.set_connection_timeout(5, 0); // 5 seconds
    cli.set_read_timeout(60, 0);       // 60 seconds

    // 1. Verify server connection
    auto pingRes = cli.Get("/");
    if (!pingRes || pingRes->status != 200) {
        std::cerr << "[Error] Cannot connect to API server at http://" << host << ":" << port << "\n"
                  << "Please make sure server.exe is running before executing DataGenerator.\n";
        return 1;
    }
    std::cout << "[Connect] Successfully connected to API server.\n\n";

    // 2. Generate entities via API
    std::vector<std::string> classCodes;
    std::vector<StudentInfo> students;
    std::vector<std::string> subjectCodes;

    if (!generateClasses(cli, cfg, classCodes)) {
        std::cerr << "[Error] Failed to generate classes.\n";
        return 1;
    }

    if (!generateStudents(cli, cfg, classCodes, students)) {
        std::cerr << "[Error] Failed to generate students.\n";
        return 1;
    }

    if (!generateSubjects(cli, cfg, subjectCodes)) {
        std::cerr << "[Error] Failed to generate subjects.\n";
        return 1;
    }

    if (!generateQuestions(cli, cfg, subjectCodes)) {
        std::cerr << "[Error] Failed to generate questions.\n";
        return 1;
    }

    generateExamsAndScores(cli, cfg, students, subjectCodes);
    generateHistory(cli, cfg, students, subjectCodes);

    // 3. Save storage via admin endpoint
    std::cout << "\n[Save] Flushing generated dataset to storage via POST /api/admin/save...\n";
    auto saveRes = cli.Post("/api/admin/save", "{}", "application/json");
    if (saveRes && (saveRes->status == 200 || saveRes->status == 201)) {
        std::cout << "[Save] Storage successfully saved to disk!\n";
    } else {
        std::cerr << "[Warning] POST /api/admin/save did not respond with success status.\n";
    }

    std::cout << "\n=========================================================\n"
              << "   PTIT DATASET GENERATION COMPLETED SUCCESSFULLY!      \n"
              << "=========================================================\n";
    return 0;
}
