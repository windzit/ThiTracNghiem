# 05 — Data Flow

## Existing Flows

### 1. Data Load Flow (Application Startup)
```
[File: storage/Classes.txt] → Class::load()
  → [File: storage/Students/{MALOP}.txt] → Student::load()
    → [File: storage/ScoreList/{MASV}.txt] → Score::load()

[File: storage/Subjects.txt] → Subject::load()
  → [File: storage/Questions/{MAMH}.txt] → Question::load()
```

**Status**: ✅ Implemented in each module's `load()` method  
**Orchestration**: ❌ `Storage::LoadAllData()` is a stub — no central load function wired up  
**Confidence**: Verified

### 2. Data Save Flow (Application Shutdown / Manual Save)
```
Class::save() → [File: storage/Classes.txt]
  → Student::save(MALOP) → [File: storage/Students/{MALOP}.txt]
    → Score::save(MASV) → [File: storage/ScoreList/{MASV}.txt]

Subject::save() → [File: storage/Subjects.txt]
  → Question::save(MAMH) → [File: storage/Questions/{MAMH}.txt]
```

**Status**: ✅ Implemented in each module's `save()` method  
**Orchestration**: ❌ `Storage::SaveAllData()` is a stub  
**Confidence**: Verified

### 3. CRUD Flows (Console)

**Class CRUD** (GV only):
```
Menu → Class::insert(Lop*) → dsLop array (in-memory)
Menu → Class::find(MALOP) → Lop* (in-memory)
Menu → Class::remove(MALOP) → dsLop array (in-memory)
Menu → Class::save() → FILE
```

**Student CRUD** (GV only):
```
Menu → Class::find(MALOP) → Student::insert(SinhVien) → linked list (in-memory)
Menu → Student::find(MASV) → SinhVien* (in-memory)
Menu → Student::remove(MASV) → linked list (in-memory)
Menu → Student::save(MALOP) → FILE
```

**Subject CRUD** (GV only):
```
Menu → Subject::insert(MonHoc) → BST (in-memory, auto-balanced)
Menu → Subject::find(MAMH) → NodeMH* (BST search)
Menu → Subject::remove(MAMH) → BST (in-memory)
Menu → Subject::save() → FILE
```

**Question CRUD** (GV only):
```
Menu → Subject::find(MAMH) → Question::insert(CauHoi) → linked list (in-memory)
Menu → Question::find(ID) → dsCHT* (linked list search)
Menu → Question::remove(ID) → linked list (with used-flag check)
Menu → Question::save(MAMH) → FILE
```

**Score CRUD** (System only):
```
Exam → Score::insert(DiemThi) → linked list (after exam submission)
Report → Score::find(MAMH) → dsDiemThi* (linked list search)
```

**Confidence**: Verified

## Missing Flows

### 4. Exam Flow (g/) — NOT IMPLEMENTED
```
Student → Select Subject → Select Question Count → Set Timer
  → Random Question Selection → Display Questions (timed)
  → Submit → Auto-Grade → Save Score
```

**Status**: ❌ Entirely missing (no Exam.cpp)  
**Dependencies**: Utils (random, timer), Exam.cpp, Question (used-flag set), Score (insert)  
**Confidence**: Verified

### 5. Report Flows (h/, i/) — PARTIALLY IMPLEMENTED
```
h/ View Exam Detail:
  GV/SV → Select Student (MASV) → Select Subject (MAMH)
    → Load Exam Detail → Display Questions + Answers + Score

i/ View Scoreboard:
  GV → Select Class (MALOP) → Select Subject (MAMH)
    → Iterate Students → Lookup Score per Student → Display Table
```

**Status**: ⚠ `Report.cpp` has `PrintClassListReport` implemented, but `PrintExamDetailReport` and `PrintScoreboardReport` are stubs  
**Confidence**: Verified

### 6. Frontend → Backend Flow — NOT IMPLEMENTED
```
React Page → Axios HTTP → server.cpp REST API → Business Logic → TXT
```

**Status**: ❌ Entirely missing. Frontend uses mock data only.  
**Confidence**: Verified

## Data Flow Diagram

```
[Startup]
  │
  ├── Storage::LoadAllData() [STUB — not wired]
  │     ├── Class::load() → Classes.txt
  │     │     └── Student::load() → Students/{MALOP}.txt
  │     │           └── Score::load() → ScoreList/{MASV}.txt
  │     └── Subject::load() → Subjects.txt
  │           └── Question::load() → Questions/{MAMH}.txt
  │
  ▼
[Menu — STUB]
  │
  ├── [GV] → CRUD Operations (in-memory) → Save → Files
  │     ├── Class CRUD
  │     ├── Student CRUD
  │     ├── Subject CRUD
  │     ├── Question CRUD
  │     ├── Report (class list)
  │     └── Report (scoreboard) [STUB]
  │
  └── [SV] → Exam [NOT IMPLEMENTED] → Score → Files
        ├── Take Exam (g/) [NOT IMPLEMENTED]
        └── View Results (h/) [NOT IMPLEMENTED]

[Shutdown]
  │
  └── Storage::SaveAllData() [STUB — not wired]
        ├── Class::save()
        ├── Subject::save()
        └── ...
```

**Confidence**: Verified