# 09 — API Mapping

## Purpose
This file maps frontend pages to proposed backend API endpoints. Every row is marked **[PROPOSED]** — no HTTP API layer currently exists.

**Confidence**: All entries are Proposed (forward-looking design)

## Proposed REST API Endpoints

### Authentication
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| StudentLogin | POST | `/api/auth/student/login` | Auth::login_student() | Students/* | **[PROPOSED]** |
| TeacherLogin | POST | `/api/auth/teacher/login` | Auth::login_teacher() | — | **[PROPOSED]** |

### Class Management
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| ClassManagement | GET | `/api/classes` | Class::load() | Classes.txt | **[PROPOSED]** |
| ClassManagement | POST | `/api/classes` | Class::insert() | Classes.txt | **[PROPOSED]** |
| ClassDetail | GET | `/api/classes/{malop}` | Class::find() | Classes.txt | **[PROPOSED]** |
| ClassDetail | DELETE | `/api/classes/{malop}` | Class::remove() | Classes.txt | **[PROPOSED]** |

### Student Management
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| ClassDetail | GET | `/api/classes/{malop}/students` | Student::load() | Students/{MALOP}.txt | **[PROPOSED]** |
| ClassDetail | POST | `/api/classes/{malop}/students` | Student::insert() | Students/{MALOP}.txt | **[PROPOSED]** |
| StudentDetail | GET | `/api/students/{masv}` | Student::find() | Students/* | **[PROPOSED]** |
| StudentDetail | DELETE | `/api/students/{masv}` | Student::remove() | Students/{MALOP}.txt | **[PROPOSED]** |

### Subject Management
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| SubjectManagement | GET | `/api/subjects` | Subject::load() | Subjects.txt | **[PROPOSED]** |
| SubjectManagement | POST | `/api/subjects` | Subject::insert() | Subjects.txt | **[PROPOSED]** |
| SubjectDetail | GET | `/api/subjects/{mamh}` | Subject::find() | Subjects.txt | **[PROPOSED]** |
| SubjectDetail | DELETE | `/api/subjects/{mamh}` | Subject::remove() | Subjects.txt | **[PROPOSED]** |
| SubjectDetail | PUT | `/api/subjects/{mamh}` | Subject::update() | Subjects.txt | **[PROPOSED]** |

### Question Management
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| QuestionManagement | GET | `/api/subjects/{mamh}/questions` | Question::load() | Questions/{MAMH}.txt | **[PROPOSED]** |
| QuestionManagement | POST | `/api/subjects/{mamh}/questions` | Question::insert() | Questions/{MAMH}.txt | **[PROPOSED]** |
| QuestionDetail | GET | `/api/questions/{id}` | Question::find() | Questions/{MAMH}.txt | **[PROPOSED]** |
| QuestionManagement | DELETE | `/api/questions/{id}` | Question::remove() | Questions/{MAMH}.txt | **[PROPOSED]** |

### Exam
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| TakeExam | POST | `/api/exam/start` | Exam::startSession() | — | **[PROPOSED]** |
| TakingExam | GET | `/api/exam/session/{sessionId}` | Exam::getSession() | — | **[PROPOSED]** |
| TakingExam | POST | `/api/exam/submit` | Exam::submitExam() | ScoreList/{MASV}.txt | **[PROPOSED]** |
| ExamHistory | GET | `/api/students/{masv}/exams` | Score::find() | ScoreList/{MASV}.txt | **[PROPOSED]** |
| ExamDetail | GET | `/api/students/{masv}/exams/{mamh}` | Report::printDetail() | ScoreList, Questions | **[PROPOSED]** |

### Scores / Gradebook
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| Gradebook | GET | `/api/classes/{malop}/subjects/{mamh}/scores` | Report::printScoreboard() | All storages | **[PROPOSED]** |
| ExamResults | GET | `/api/results` | Score::find() | ScoreList/* | **[PROPOSED]** |

### Reports
| React Page | Method | Endpoint | Business Module | Storage | Status |
|-----------|--------|----------|---------------|---------|--------|
| — (Console) | — | (console print) | Report::PrintClassListReport() | Classes.txt | **[PROPOSED]** |
| — (Console) | — | (console print) | Report::PrintExamDetailReport() | Multiple | **[PROPOSED]** |
| — (Console) | — | (console print) | Report::PrintScoreboardReport() | Multiple | **[PROPOSED]** |

## Data Flow: React Page → API → Business → Storage

```
React Page
  → Service (Axios HTTP)
    → HTTP Request (JSON)
      → server.cpp Route Handler
        → Controller (parse JSON → call Business)
          → Business Module (CRUD operation)
            → Storage (TXT file I/O)
          ← Business Module (return result)
        ← Controller (serialize to JSON)
      ← HTTP Response (JSON)
    ← Service (return data)
  ← React Page (render)
```

**Confidence**: Proposed

## Missing Infrastructure
To implement this API mapping, the following needs to be built:
1. **JSON serialization** in C++ (nlohmann/json exists in include/ but isn't used)
2. **Router handlers** in server.cpp for each endpoint
3. **Controller functions** that parse HTTP requests and call business modules
4. **Service layer** in frontend that makes real HTTP calls instead of returning mock data

**Confidence**: Verified (gap analysis)