#include "Random.h"
#include <chrono>
#include <iomanip>
#include <sstream>

std::mt19937 Random::s_rng;

void Random::init() {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    s_rng.seed(static_cast<unsigned int>(seed));
}

int Random::getInt(int min, int max) {
    if (min >= max) return min;
    std::uniform_int_distribution<int> dist(min, max);
    return dist(s_rng);
}

float Random::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(s_rng);
}

char Random::getRandomOption() {
    static const char options[] = {'a', 'b', 'c', 'd'};
    return options[getInt(0, 3)];
}

std::string Random::getRandomLastName() {
    static const std::vector<std::string> lastNames = {
        "Nguyen", "Tran", "Le", "Pham", "Hoang", "Huynh", "Vu", "Vo",
        "Dang", "Bui", "Do", "Ho", "Ngo", "Duong", "Ly"
    };
    static const std::vector<std::string> middleNames = {
        "Van", "Thi", "Duc", "Minh", "Quang", "Huu", "Tuan", "Thanh", "Duy", "Ngoc"
    };
    return lastNames[getInt(0, (int)lastNames.size() - 1)] + " " +
           middleNames[getInt(0, (int)middleNames.size() - 1)];
}

std::string Random::getRandomFirstName(bool isFemale) {
    static const std::vector<std::string> maleNames = {
        "An", "Binh", "Cuong", "Dung", "Em", "Giang", "Hoa", "Hung", "Khai", "Linh",
        "Nam", "Phong", "Quan", "Son", "Tuan", "Viet", "Vinh"
    };
    static const std::vector<std::string> femaleNames = {
        "Anh", "Bich", "Cuc", "Diep", "Ha", "Hanh", "Hoa", "Huong", "Lan", "Mai",
        "Nga", "Nhung", "Oanh", "Phuong", "Quyen", "Thao", "Trang", "Yen"
    };
    if (isFemale) {
        return femaleNames[getInt(0, (int)femaleNames.size() - 1)];
    }
    return maleNames[getInt(0, (int)maleNames.size() - 1)];
}

std::string Random::getRandomGender() {
    return (getInt(0, 1) == 0) ? "Nam" : "Nu";
}

std::string Random::getClassCode(int index) {
    static const char* majors[] = {"CN", "VT", "AT", "MR", "KT", "QT"};
    int year = 20 + (index / 60) % 5; // D20 to D24
    int majorIdx = index % 6;
    int classNum = (index / 6) % 10 + 1;
    std::ostringstream ss;
    ss << "D" << year << "CQ" << majors[majorIdx] << std::setw(2) << std::setfill('0') << classNum << "-N";
    return ss.str();
}

std::string Random::getStudentCode(int index) {
    static const char* majors[] = {"CN", "VT", "AT", "MR", "KT"};
    int year = 20 + (index / 2000) % 5;
    int majorIdx = (index / 400) % 5;
    int svNum = (index % 400) + 1;
    std::ostringstream ss;
    ss << "N" << year << "DC" << majors[majorIdx] << std::setw(3) << std::setfill('0') << svNum;
    return ss.str();
}

struct SubjectPair {
    const char* code;
    const char* name;
};

static const SubjectPair g_realPtitSubjects[] = {
    {"INT1339", "Lap trinh C++"},
    {"INT13145", "Cau truc du lieu va giai thuat"},
    {"INT1332", "Co so du lieu"},
    {"INT1415", "Mang may tinh"},
    {"INT1358", "He dieu hanh"},
    {"INT1434", "Lap trinh Web"},
    {"INT1306", "Kien truc may tinh"},
    {"INT1405", "Tri tue nhan tao"},
    {"INT1422", "An toan thong tin"},
    {"INT1340", "Ky thuat phan mem"},
    {"MATH1301", "Dai so tuyen tinh"},
    {"MATH1302", "Giai tich 1"},
    {"PHYS1301", "Vat ly dai cuong 1"},
    {"BAS1105", "Triet hoc Mac - Lenin"},
    {"BAS1224", "Tieng Anh chuyen nganh CNTT"},
    {"INT1360", "Lap trinh Huong doi tuong"},
    {"INT1416", "Truyen thong va mang may tinh"},
    {"INT1428", "Lap trinh thiet bi di dong"},
    {"INT1440", "Kiem thu phan mem"},
    {"INT1450", "Xu ly anh va nhan dang"}
};

static const int g_realPtitCount = sizeof(g_realPtitSubjects) / sizeof(g_realPtitSubjects[0]);

std::string Random::getSubjectCode(int index) {
    if (index < g_realPtitCount) {
        return g_realPtitSubjects[index].code;
    }
    std::ostringstream ss;
    ss << "INT" << (1450 + (index - g_realPtitCount + 1));
    return ss.str();
}

std::string Random::getSubjectName(int index) {
    if (index < g_realPtitCount) {
        return g_realPtitSubjects[index].name;
    }
    return "Mon hoc chuyen nganh " + std::to_string(index + 1);
}
