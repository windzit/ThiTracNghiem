cmake_minimum_required(VERSION 3.15)
project(DataGenerator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DATAGEN_SOURCES
    main.cpp
    Random.cpp
    GeneratorConfig.cpp
    ClassGenerator.cpp
    StudentGenerator.cpp
    SubjectGenerator.cpp
    QuestionGenerator.cpp
    ScoreGenerator.cpp
    HistoryGenerator.cpp
)

add_executable(DataGenerator ${DATAGEN_SOURCES})

set_target_properties(DataGenerator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

target_include_directories(DataGenerator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

if (MSVC)
    target_compile_options(DataGenerator PRIVATE /W4 /EHsc)
    target_link_libraries(DataGenerator PRIVATE ws2_32 crypt32)
else()
    target_compile_options(DataGenerator PRIVATE -Wall -Wextra)
    target_link_libraries(DataGenerator PRIVATE pthread)
endif()
