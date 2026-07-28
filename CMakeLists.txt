cmake_minimum_required(VERSION 3.20)

project(ThiTracNghiem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# ============================================================
# Target 1: HTTP Server
# ============================================================
add_executable(server
    "server.cpp"
    "src/Auth.cpp"
    "src/Class.cpp"
    "src/Student.cpp"
    "src/Score.cpp"
    "src/Subject.cpp"
    "src/Question.cpp"
    "src/Report.cpp"
    "src/Storage.cpp"
    "src/Exam.cpp"
    "src/Utils.cpp"
    "src/PathResolver.cpp"
    "src/StorageManager.cpp"
    "src/StorageValidator.cpp"
    "src/StorageDeserializer.cpp"
    "src/StorageVerifier.cpp"
    "src/StorageIntegrityChecker.cpp"
    "src/StringNormalizer.cpp"
    "src/Migrator.cpp"
)

if (WIN32)
    target_link_libraries(server ws2_32)
endif()

# ============================================================
# Target 2: Console Application
# ============================================================
add_executable(console
    "main.cpp"
    "src/Menu.cpp"
    "src/Auth.cpp"
    "src/Class.cpp"
    "src/Student.cpp"
    "src/Score.cpp"
    "src/Subject.cpp"
    "src/Question.cpp"
    "src/Report.cpp"
    "src/Storage.cpp"
    "src/Exam.cpp"
    "src/Utils.cpp"
    "src/PathResolver.cpp"
    "src/StorageManager.cpp"
    "src/StorageValidator.cpp"
    "src/StorageDeserializer.cpp"
    "src/StorageVerifier.cpp"
    "src/StorageIntegrityChecker.cpp"
    "src/StringNormalizer.cpp"
    "src/Migrator.cpp"
)
