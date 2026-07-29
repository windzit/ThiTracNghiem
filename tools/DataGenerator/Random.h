#pragma once
#include <string>
#include <vector>
#include <random>

class Random {
public:
    static void init();

    static int getInt(int min, int max);
    static float getFloat(float min, float max);
    static char getRandomOption(); // 'a', 'b', 'c', 'd'

    static std::string getRandomLastName();
    static std::string getRandomFirstName(bool isFemale);
    static std::string getRandomGender(); // "Nam" or "Nu"
    static std::string getClassCode(int index);
    static std::string getStudentCode(int index);
    static std::string getSubjectCode(int index);
    static std::string getSubjectName(int index);

private:
    static std::mt19937 s_rng;
};
