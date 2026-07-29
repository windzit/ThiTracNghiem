#pragma once
#include <string>
#include <map>

struct GeneratorConfig {
    std::string name;
    int classes = 10;
    int students = 100;
    int subjects = 20;
    int questionsPerSubject = 25;
    int scoresPerStudent = 5;
    int historyPerStudent = 5;
};

class PresetManager {
public:
    static PresetManager& getInstance();
    
    bool loadPreset(const std::string& presetName, GeneratorConfig& outConfig) const;
    void printAvailablePresets() const;

private:
    PresetManager();
    std::map<std::string, GeneratorConfig> m_presets;
};
