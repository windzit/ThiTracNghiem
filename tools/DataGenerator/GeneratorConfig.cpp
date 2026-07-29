#include "GeneratorConfig.h"
#include <iostream>

PresetManager& PresetManager::getInstance() {
    static PresetManager instance;
    return instance;
}

PresetManager::PresetManager() {
    // Preset 1: verify
    GeneratorConfig verifyCfg;
    verifyCfg.name = "verify";
    verifyCfg.classes = 10;
    verifyCfg.students = 100;
    verifyCfg.subjects = 20;
    verifyCfg.questionsPerSubject = 25;
    verifyCfg.scoresPerStudent = 5;
    verifyCfg.historyPerStudent = 5;
    m_presets["verify"] = verifyCfg;

    // Preset 2: small
    GeneratorConfig smallCfg;
    smallCfg.name = "small";
    smallCfg.classes = 100;
    smallCfg.students = 10000;
    smallCfg.subjects = 100;
    smallCfg.questionsPerSubject = 200;
    smallCfg.scoresPerStudent = 20;
    smallCfg.historyPerStudent = 30;
    m_presets["small"] = smallCfg;

    // Preset 3: medium
    GeneratorConfig medCfg;
    medCfg.name = "medium";
    medCfg.classes = 500;
    medCfg.students = 100000;
    medCfg.subjects = 300;
    medCfg.questionsPerSubject = 1000;
    medCfg.scoresPerStudent = 30;
    medCfg.historyPerStudent = 50;
    m_presets["medium"] = medCfg;

    // Preset 4: large
    GeneratorConfig lgCfg;
    lgCfg.name = "large";
    lgCfg.classes = 1000;
    lgCfg.students = 500000;
    lgCfg.subjects = 500;
    lgCfg.questionsPerSubject = 2000;
    lgCfg.scoresPerStudent = 40;
    lgCfg.historyPerStudent = 80;
    m_presets["large"] = lgCfg;

    // Preset 5: extreme
    GeneratorConfig extCfg;
    extCfg.name = "extreme";
    extCfg.classes = 1000;
    extCfg.students = 1000000;
    extCfg.subjects = 500;
    extCfg.questionsPerSubject = 2000;
    extCfg.scoresPerStudent = 40;
    extCfg.historyPerStudent = 100;
    m_presets["extreme"] = extCfg;
}

bool PresetManager::loadPreset(const std::string& presetName, GeneratorConfig& outConfig) const {
    auto it = m_presets.find(presetName);
    if (it != m_presets.end()) {
        outConfig = it->second;
        return true;
    }
    return false;
}

void PresetManager::printAvailablePresets() const {
    std::cout << "Available Presets:\n";
    for (const auto& [name, cfg] : m_presets) {
        std::cout << "  - " << name << ": Classes=" << cfg.classes
                  << ", Students=" << cfg.students
                  << ", Subjects=" << cfg.subjects
                  << ", Questions/Sub=" << cfg.questionsPerSubject
                  << ", Scores/Student=" << cfg.scoresPerStudent
                  << ", History/Student=" << cfg.historyPerStudent << "\n";
    }
}
