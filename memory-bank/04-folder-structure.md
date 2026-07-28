# 04 — Folder Structure

## Root Directory

```
ThiTracNghiem/
├── .gitignore                         # Root gitignore rules
├── CMakeLists.txt                     # CMake build system (targets: server, console)
├── CMakeSettings.json                 # VS CMake settings
├── CppProperties.json                 # VS C++ properties
├── main.cpp                           # Console app entry point
├── server.cpp                         # HTTP API server (httplib.h)
├── TASKS.md                           # Master task progress tracker
│
├── docs/                              # System documentation
│   ├── ponytail.md                    # Repository cleanup roadmap
│   ├── debai.md                       # Problem statement & assignment spec
│   └── archive/                       # Historical specs & legacy documentation
│       ├── Chitiet_1.md
│       ├── Chitiet_2.md
│       ├── TongQuan.md
│       └── UI_goiy.md
│
├── include/                           # C++ header files (26 files)
│   ├── Auth.h, Class.h, Exam.h, Menu.h, Migrator.h, PathResolver.h
│   ├── Question.h, Report.h, Score.h, Storage*.h, StringNormalizer.h
│   ├── Student.h, Subject.h, Utils.h, Validation.h
│   ├── DArray.h, LinkedList.h, Queue.h, Stack.h
│   ├── Utility/ (Random.h, StringUtil.h, Swap.h)
│   ├── httplib.h, nlohmann/
│
├── src/                               # C++ source files (20 files)
│   ├── Auth.cpp, Class.cpp, Exam.cpp, Menu.cpp, Migrator.cpp
│   ├── PathResolver.cpp, Question.cpp, Report.cpp, Score.cpp
│   ├── Storage*.cpp, StringNormalizer.cpp, Student.cpp, Subject.cpp
│   ├── Utils.cpp, Validation.cpp
│
├── storage/                           # Persistence files (.txt)
│   ├── classes.txt, students.txt, subjects.txt, questions.txt
│   ├── scores.txt, exam_sessions.txt, exam_history.txt
│   ├── metadata.txt, SystemSettings.txt, transaction.log
│
├── scripts/                           # Utility scripts (.bat)
│   ├── reset_storage.bat
│   └── seed_ptit_data.bat
│
├── frontend/                          # React + Vite + Tailwind CSS project
│   ├── index.html                     # HTML entry
│   ├── package.json                   # NPM dependencies
│   ├── vite.config.ts                 # Vite proxy & build config
│   ├── tailwind.config.ts             # Tailwind design system
│   ├── tsconfig.json / .app.json / .node.json
│   ├── components.json               # shadcn/ui config
│   ├── eslint.config.js              # ESLint configuration
│   ├── public/                        # Static assets (images, logos)
│   └── src/                           # Frontend source
│       ├── App.tsx                   # Main routing (20 pages)
│       ├── main.tsx                  # React entry
│       ├── index.css                 # CSS entry & custom styling
│       ├── components/               # UI, shared & layout components
│       ├── pages/                    # 20 page components
│       ├── services/                 # API service layer (http calls)
│       ├── mappers/                  # Backend ↔ Frontend data mappers
│       ├── config/                   # API config (api.ts)
│       ├── context/                  # Context providers (Toast, Connection)
│       ├── types/                    # Unified TypeScript definitions (index.ts, backend.ts)
│       ├── data/                     # Mock fallback & constant data
│       └── utils/                    # Utility functions (ApiErrorHandler, formatSubject)
│
└── memory-bank/                      # System memory bank (17 docs)
```

**Confidence**: Verified