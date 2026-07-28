# 02 — System Architecture

## Actual Architecture (Verified)

```
┌─────────────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                            │
│  ┌──────────────────────┐    ┌──────────────────────────────┐   │
│  │ Console UI (C++)     │    │ React Frontend (TypeScript)  │   │
│  │ Menu.cpp, Validation │    │ 23 Routes, shadcn/ui         │   │
│  │ .cpp, Report.cpp     │    │ Real Axios Services          │   │
│  └──────────┬───────────┘    └──────────────┬───────────────┘   │
│             │                               │                   │
│             │           ╔═══════════════════╝                   │
│             │           ║ HTTP REST API (cpp-httplib)           │
│             ▼           ▼                                       │
├─────────────────────────────────────────────────────────────────┤
│                    BUSINESS LOGIC LAYER                          │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐       │
│  │  Auth    │  Class   │ Student  │ Subject  │  Score   │       │
│  │ (CRUD)   │ (CRUD)   │ (CRUD)   │ (BST)    │ (CRUD)   │       │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌─────────────┐     │
│  │ Question │  │  Exam    │  │  Report  │  │  Session    │     │
│  │ (CRUD)   │  │ (Resume) │  │ (Full)   │  │(Heartbeat/  │     │
│  └──────────┘  └──────────┘  └──────────┘  │ TTL=30s)    │     │
│                                            └─────────────┘     │
├─────────────────────────────────────────────────────────────────┤
│                    DATA STRUCTURE LAYER                          │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐       │
│  │  BST     │  Array   │  Linked  │  Linked  │  Linked  │       │
│  │ (MonHoc) │(Lop*[]   │   List   │   List   │   List   │       │
│  │          │ MAXLOP)  │(SinhVien)│ (CauHoi) │ (DiemThi)│       │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘       │
├─────────────────────────────────────────────────────────────────┤
│                    STORAGE LAYER                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  TXT Files: Classes.txt, Subjects.txt, Questions/{MAMH}  │   │
│  │  .txt, Students/{MALOP}.txt, ScoreList/{MASV}.txt,      │   │
│  │  ExamSessions/{MASV}.txt                                 │   │
│  │  Pipe-delimited (`|`) format                             │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

**Confidence**: Verified

## Component Summary

| Layer | Status | Implementation Details |
|-------|--------|------------------------|
| React Frontend | ✅ | 23 routes, Axios services, `SessionHeartbeatProvider`, `GlobalConnectionOverlay` |
| HTTP REST API | ✅ | `server.cpp` with 17+ endpoints (JSON format, CORS headers) |
| Session Registry | ✅ | `SessionRegistry` in C++ with 30s TTL and `POST /api/session/heartbeat` |
| Business Services | ✅ | Class, Student, Subject (BST), Question, Score, Exam, Session, Report |
| Storage (TXT) | ✅ | Per-module file I/O + auto-persistence and downtime compensation |

**Confidence**: Verified