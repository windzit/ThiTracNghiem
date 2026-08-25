cmake_minimum_required(VERSION 3.20)

project(ThiTracNghiem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(console)
include_directories(server)
include_directories(server/handlers)

# ============================================================
# Target 1: HTTP Server
# ============================================================
add_executable(server
    "server.cpp"
    "server/ServerContext.cpp"
    "server/Diagnostics.cpp"
    "server/RouteRegistry.cpp"
    "server/ServerBootstrap.cpp"
    "server/handlers/AuthHandler.cpp"
    "server/handlers/ClassHandler.cpp"
    "server/handlers/StudentHandler.cpp"
    "server/handlers/SubjectHandler.cpp"
    "server/handlers/QuestionHandler.cpp"
    "server/handlers/ExamHandler.cpp"
    "server/handlers/ReportHandler.cpp"
    "server/handlers/AdminHandler.cpp"
    "src/Auth.cpp"
    "src/Class.cpp"
    "src/Student.cpp"
    "src/Score.cpp"
    "src/Subject.cpp"
    "src/Question.cpp"
    "src/Storage.cpp"
    "src/Exam.cpp"
    "src/Utils.cpp"
    "src/PathResolver.cpp"
    "src/IndexManager.cpp"
    "src/StorageManager.cpp"
    "src/StorageValidator.cpp"
    "src/StringNormalizer.cpp"
)

if (WIN32)
    target_link_libraries(server ws2_32)
endif()

# ============================================================
# Target 2: Console Application
# ============================================================
add_executable(console
    "console/main.cpp"
    "console/Menu.cpp"
    "console/Report.cpp"
    "console/ConsoleExam.cpp"
    "src/Auth.cpp"
    "src/Class.cpp"
    "src/Student.cpp"
    "src/Score.cpp"
    "src/Subject.cpp"
    "src/Question.cpp"
    "src/Storage.cpp"
    "src/Exam.cpp"
    "src/Utils.cpp"
    "src/PathResolver.cpp"
    "src/IndexManager.cpp"
    "src/StorageManager.cpp"
    "src/StorageValidator.cpp"
    "src/StringNormalizer.cpp"
)

set_target_properties(server console PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


