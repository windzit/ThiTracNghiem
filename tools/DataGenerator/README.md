# Standalone PTIT Data Generator

**Component:** `DataGenerator.exe`  
**Location:** `tools/DataGenerator/`  
**Project:** ThiTracNghiem  

---

## 1. Overview & Architecture

The **PTIT Data Generator** is a completely independent C++ command-line tool designed to generate valid test datasets for the **ThiTracNghiem** examination system.

### Key Architectural Principles
- **REST API Only:** Interacts exclusively over HTTP endpoints (`http://localhost:8080`). It does **NOT** write directly to `.txt` or `.idx` storage files, ensuring all backend validation, storage integrity, metadata tracking, and index offset alignment remain 100% synchronized and correct.
- **Zero Benchmark/Profiler Logic:** Strictly generates data without performance measurement or timing code.
- **Zero Codebase Coupling:** Completely standalone binary built under `tools/DataGenerator/`.

---

## 2. Directory & File Structure

```
tools/DataGenerator/
├── CMakeLists.txt         # Build configuration
├── main.cpp               # CLI argument parser & execution controller
├── GeneratorConfig.h      # Configuration struct & preset declarations
├── GeneratorConfig.cpp    # Preset definitions (verify, small, medium, large, extreme)
├── Random.h               # Vietnamese name and code randomizer header
├── Random.cpp             # Randomizer implementation
├── ClassGenerator.cpp     # POST /api/classes
├── StudentGenerator.h     # Student entity structs & generator function signatures
├── StudentGenerator.cpp   # POST /api/students
├── SubjectGenerator.cpp   # POST /api/subjects
├── QuestionGenerator.cpp  # POST /api/questions
├── ScoreGenerator.cpp     # Simulates exam sessions via POST /api/exams/start, PUT /api/exams/answer, POST /api/exams/submit
├── HistoryGenerator.cpp   # Simulates exam history sessions via API endpoints
└── README.md              # Documentation (this file)
```

---

## 3. Presets Specification

| Preset | Classes | Students | Subjects | Questions / Subject | Scores / Student | Exam History / Student |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **`verify`** | 10 | 100 | 20 | 25 | 5 | 5 |
| **`small`** | 100 | 10,000 | 100 | 200 | 20 | 30 |
| **`medium`** | 500 | 100,000 | 300 | 1,000 | 30 | 50 |
| **`large`** | 1,000 | 500,000 | 500 | 2,000 | 40 | 80 |
| **`extreme`** | 1,000 | 1,000,000 | 500 | 2,000 | 40 | 100 |

---

## 4. Building the Generator

### Build via CMake
```bash
# Build all targets (server, console, DataGenerator)
cmake --build build --config Debug
```
The compiled binary will be produced at `build/Debug/DataGenerator.exe`.

---

## 5. Running the Generator

### Direct Execution
Make sure `server.exe` is running on `http://localhost:8080`:

```cmd
# Run verify preset
build\Debug\DataGenerator.exe --preset verify

# Run small preset
build\Debug\DataGenerator.exe --preset small

# Run with custom host/port
build\Debug\DataGenerator.exe --preset verify --host localhost --port 8080
```

---

## 6. One-Click Batch Automation

Use the automated batch scripts in `scripts/`:

```cmd
# Seed verify dataset
scripts\seed_ptit_data_verify.bat

# Seed small dataset
scripts\seed_ptit_data_small.bat

# Seed medium dataset
scripts\seed_ptit_data_medium.bat

# Seed large dataset
scripts\seed_ptit_data_large.bat

# Seed extreme dataset
scripts\seed_ptit_data_extreme.bat
```

Each batch script automatically:
1. Stops any running `server.exe` instance.
2. Resets storage to a clean default state via `server.exe --reset-storage`.
3. Starts `server.exe` in the background.
4. Waits until `http://localhost:8080/` becomes responsive.
5. Executes `DataGenerator.exe --preset <preset>`.
6. Flushes data to disk via `POST /api/admin/save`.
7. Stops `server.exe` cleanly.

---

## 7. How to Add a New Preset

To add a new preset (e.g. `demo`):

1. Open `tools/DataGenerator/GeneratorConfig.cpp`.
2. Add your new preset in the `PresetManager` constructor:
   ```cpp
   GeneratorConfig demoCfg;
   demoCfg.name = "demo";
   demoCfg.classes = 5;
   demoCfg.students = 50;
   demoCfg.subjects = 10;
   demoCfg.questionsPerSubject = 10;
   demoCfg.scoresPerStudent = 2;
   demoCfg.historyPerStudent = 2;
   m_presets["demo"] = demoCfg;
   ```
3. Rebuild `DataGenerator.exe`.
