# Ponytail Cleanup Roadmap

> **PTIT Computer-Based Testing System**
> Generated: 2026-07-28 · Audit-only — no code was modified

---

## Executive Summary

### Repository Health

| Metric | Status |
|---|---|
| Backend stability | ✅ Stable — all business logic verified |
| Frontend completeness | ✅ Feature-complete, 20 pages |
| Build system | ✅ CMake dual-target (server + console) |
| Technical debt | ⚠️ Moderate — accumulated temp files, mock data, type duplication |
| Repository hygiene | ❌ No root `.gitignore`, committed binaries + logs + backups |

### Main Risks

1. **No root `.gitignore`** — binaries (7 MB `server.exe`, 138 KB `store.exe`), logs, and backups are tracked.
2. **Five `storage_backup_reset_*` directories** committed to the repo.
3. **Type definitions duplicated** across `data/mock*.ts` and `types/` — pages import interfaces from mock files instead of a canonical `types/` module.
4. **`frontend/dist/` committed** — 14.9 MB of build artifacts tracked in version control.
5. **Mock data files** still imported by production pages (fallback types & data coexist).

### Technical Debt Summary

| Category | Items Found | Estimated Effort |
|---|---|---|
| Temporary files & logs | 12 files + 5 backup dirs | 30 min |
| Documentation clutter | ~10 obsolete md files | 30 min |
| Frontend type duplication | 10 mock data files, types in 2 locations | 2–3 hours |
| Frontend empty directories | 3 dirs (`features/`, `styles/`, `layouts/` root) | 5 min |
| Frontend unused mock data | 3 files (`mockProfile`, `mockResults`, `mockExams`) | 1 hour |
| Backend — dead test files | `test.cpp`, `store.cpp`, `data.txt` | 15 min |
| Repository config | `.gitignore`, `.vs/`, `out/` cleanup | 30 min |

### Expected Impact

- **~22 MB** reclaimable from committed binaries, dist, and backups.
- Cleaner git history going forward.
- Reduced confusion for new contributors.
- No runtime behavior change — zero risk to stability.

---

## Protected Files

> [!CAUTION]
> These files are **production-critical**. Modify only with explicit approval and full verification.

### Backend Core (`include/` + `src/`)

| File | Role | Why Protected |
|---|---|---|
| `server.cpp` | HTTP API server (1815 lines) | All REST endpoints, CORS, locking, data serialization |
| `main.cpp` | Console entry point | Loads/saves all data, shows menu |
| `CMakeLists.txt` | Build configuration | Dual-target build (server + console) |
| `include/*.h` | All 26 header files | Data structures, business entities, storage layer |
| `src/*.cpp` | All 20 implementation files | Business logic, storage, validation, migration |
| `include/httplib.h` | HTTP server library (702 KB) | Third-party single-header, do not touch |
| `include/nlohmann/` | JSON library | Third-party, do not touch |
| `include/Utility/` | `Random.h`, `StringUtil.h`, `Swap.h` | Core utility headers |

### Storage (`storage/`)

| File | Role |
|---|---|
| `storage/*.txt` | Live production data (classes, students, questions, subjects, scores, etc.) |
| `storage/metadata.txt` | Storage version/metadata |
| `storage/transaction.log` | Transaction integrity log |
| `storage/SystemSettings.txt` | Runtime settings |

### Frontend Core (`frontend/src/`)

| Path | Role |
|---|---|
| `frontend/src/pages/*.tsx` | All 20 page components |
| `frontend/src/services/*.ts` | All 10 API service modules |
| `frontend/src/components/` | UI + shared + layout components |
| `frontend/src/config/api.ts` | API configuration |
| `frontend/src/context/` | Toast + Connection context providers |
| `frontend/src/mappers/` | Backend↔Frontend data mappers |
| `frontend/src/types/backend.ts` | Backend API contract types |
| `frontend/src/App.tsx` | Router + layout composition |

### Scripts (`scripts/`)

| File | Role |
|---|---|
| `scripts/reset_storage.bat` | Storage reset utility |
| `scripts/seed_ptit_data.bat` | Test data seeding |

---

## Phase 0 — Repository Audit (Inventory Only)

> No modifications. This section documents what exists.

### Root Directory Inventory

| File | Size | Verdict | Reason |
|---|---|---|---|
| `audit_log.txt` | 0 bytes | **DELETE** | Empty log from past audit run |
| `audit_err.txt` | 0 bytes | **DELETE** | Empty error log from past audit run |
| `startup_log.txt` | 0 bytes | **DELETE** | Empty startup log |
| `startup_err.txt` | 0 bytes | **DELETE** | Empty startup error log |
| `seed_server.log` | 5.5 KB | **DELETE** | Seeding script output log |
| `server_startup.log` | 0 bytes | **DELETE** | Empty server startup log |
| `server_startup_err.log` | 0 bytes | **DELETE** | Empty server startup error log |
| `server_tc03.log` | 0 bytes | **DELETE** | Empty test case log |
| `server_tc03_restart.log` | 0 bytes | **DELETE** | Empty test case restart log |
| `server.exe` | 7.0 MB | **DELETE** | Compiled binary — should be built, not tracked |
| `store.exe` | 138 KB | **DELETE** | Compiled binary from `store.cpp` experiment |
| `store.cpp` | 577 bytes | **DELETE** | Standalone file I/O experiment, not part of build |
| `test.cpp` | 10.5 KB | **DELETE** | Old manual test harness, not in CMake build |
| `data.txt` | 32 bytes | **DELETE** | Test data for `store.cpp` experiment |
| `Role.md` | 116 bytes | **DELETE** | Team role assignment notes (informal, outdated) |
| `current-task.md` | 201 bytes | **DELETE** | AI agent task tracking file |
| `next-task.md` | 241 bytes | **DELETE** | AI agent next-task file |
| `progress.md` | 797 bytes | **DELETE** | AI agent progress report |
| `TASKS.md` | 35.6 KB | **ARCHIVE** | Comprehensive task tracker — historical value, but clutters root |
| `CMakeSettings.json` | 405 bytes | **IGNORE** | VS CMake settings, useful for local dev |
| `CppProperties.json` | 382 bytes | **IGNORE** | VS C++ IntelliSense config, useful for local dev |
| `CMakeLists.txt` | 1.6 KB | **KEEP** | Build system — critical |
| `server.cpp` | 71 KB | **KEEP** | Production server — critical |
| `main.cpp` | 611 bytes | **KEEP** | Console entry point — critical |

### Backup Directories

| Directory | Size | Verdict | Reason |
|---|---|---|---|
| `storage_backup_reset_20260728_142314/` | 307 bytes | **DELETE** | Reset backup (mostly empty files) |
| `storage_backup_reset_20260728_144255/` | 9.4 KB | **DELETE** | Reset backup |
| `storage_backup_reset_20260728_144426/` | 307 bytes | **DELETE** | Reset backup (mostly empty files) |
| `storage_backup_reset_20260728_145328/` | 9.3 KB | **DELETE** | Reset backup |
| `storage_backup_reset_20260728_145431/` | 307 bytes | **DELETE** | Reset backup (mostly empty files) |

### Build/Output Directories

| Directory | Verdict | Reason |
|---|---|---|
| `build/` | **IGNORE** | CMake build output — should be in `.gitignore` |
| `out/` | **IGNORE** | VS build output — should be in `.gitignore` |
| `.vs/` | **IGNORE** | VS internal — should be in `.gitignore` |
| `frontend/dist/` | **DELETE from git** | Vite build output (14.9 MB) — should be in `.gitignore` |
| `frontend/node_modules/` | **IGNORE** | Already in `frontend/.gitignore` |

### Documentation Directory (`document/`)

| File | Size | Verdict | Reason |
|---|---|---|---|
| `Chitiet_1.md` | 20.8 KB | **ARCHIVE** | Detailed spec part 1 — historical reference |
| `Chitiet_2.md` | 21.5 KB | **ARCHIVE** | Detailed spec part 2 — historical reference |
| `TongQuan.md` | 38 KB | **ARCHIVE** | Overview spec — historical reference |
| `UI_goiy.md` | 38 KB | **ARCHIVE** | UI suggestion spec — historical reference |
| `debai.md` | 2.1 KB | **KEEP** | Project assignment/problem statement |

### Memory Bank (`memory-bank/`)

| File | Size | Verdict | Reason |
|---|---|---|---|
| `00-project-dna.md` | 6.6 KB | **KEEP** | Core project identity |
| `01-overview.md` | 1.6 KB | **KEEP** | System overview |
| `02-system-architecture.md` | 5.4 KB | **KEEP** | Architecture doc |
| `03-tech-stack.md` | 2.1 KB | **KEEP** | Technology documentation |
| `04-folder-structure.md` | 5.9 KB | **KEEP** (update after cleanup) | Folder structure reference |
| `05-data-flow.md` | 4.8 KB | **KEEP** | Data flow documentation |
| `06-dependencies.md` | 4.3 KB | **KEEP** | Dependency tracking |
| `07-storage.md` | 5.8 KB | **KEEP** | Storage format documentation |
| `08-frontend-architecture.md` | 4.9 KB | **KEEP** | Frontend architecture |
| `09-api-mapping.md` | 5.7 KB | **KEEP** | API endpoint reference |
| `10-business-modules.md` | 5.5 KB | **KEEP** | Business logic docs |
| `11-coding-convention.md` | 3.1 KB | **KEEP** | Conventions |
| `12-backend-architecture.md` | 6.4 KB | **KEEP** | Backend architecture |
| `13-workflow.md` | 6.2 KB | **KEEP** | Build/run workflow |
| `14-decisions.md` | 2.7 KB | **KEEP** | Decision log |
| `15-progress` (no ext) | 4.8 KB | **DELETE** | Duplicate of `15-progress.md` (no file extension) |
| `15-progress.md` | 3.7 KB | **KEEP** | Progress tracking |
| `16-current-task.md` | 1.3 KB | **DELETE** | Duplicates root `current-task.md` (both deletable) |
| `17-next-task.md` | 1.8 KB | **DELETE** | Duplicates root `next-task.md` (both deletable) |
| `18-issues.md` | 4.0 KB | **KEEP** | Known issues tracker |

---

## Phase 1 — Temporary Files Cleanup

> Delete all logs, backups, compiled binaries, and experiment files that do not belong in version control.

### Files to Delete

| # | File/Directory | Type | Size |
|---|---|---|---|
| 1 | `audit_log.txt` | Empty log | 0 B |
| 2 | `audit_err.txt` | Empty log | 0 B |
| 3 | `startup_log.txt` | Empty log | 0 B |
| 4 | `startup_err.txt` | Empty log | 0 B |
| 5 | `seed_server.log` | Script output | 5.5 KB |
| 6 | `server_startup.log` | Empty log | 0 B |
| 7 | `server_startup_err.log` | Empty log | 0 B |
| 8 | `server_tc03.log` | Empty log | 0 B |
| 9 | `server_tc03_restart.log` | Empty log | 0 B |
| 10 | `server.exe` | Compiled binary | 7.0 MB |
| 11 | `store.exe` | Compiled binary | 138 KB |
| 12 | `store.cpp` | Experiment file | 577 B |
| 13 | `data.txt` | Experiment data | 32 B |
| 14 | `test.cpp` | Old test harness | 10.5 KB |
| 15 | `storage_backup_reset_20260728_142314/` | Backup dir | 307 B |
| 16 | `storage_backup_reset_20260728_144255/` | Backup dir | 9.4 KB |
| 17 | `storage_backup_reset_20260728_144426/` | Backup dir | 307 B |
| 18 | `storage_backup_reset_20260728_145328/` | Backup dir | 9.3 KB |
| 19 | `storage_backup_reset_20260728_145431/` | Backup dir | 307 B |

**Expected Risk**: 🟢 Low — none of these are referenced by the build system or runtime.

**Rollback**: `git checkout` to restore deleted files from last commit.

**Acceptance Criteria**:
- `cmake --build` still produces `server.exe` and `console.exe` successfully.
- `server.exe` starts and responds on `:8080`.
- `frontend` builds with `npm run build`.
- No runtime behavior change.

---

## Phase 2 — Documentation Cleanup

### Obsolete Root Markdown Files

| File | Action | Reason |
|---|---|---|
| `Role.md` | **DELETE** | Informal team notes, not documentation |
| `current-task.md` | **DELETE** | AI agent tracking artifact |
| `next-task.md` | **DELETE** | AI agent tracking artifact |
| `progress.md` | **DELETE** | AI agent tracking artifact |
| `TASKS.md` | **ARCHIVE** → `docs/archive/TASKS.md` | Historical task tracker (35.6 KB), valuable reference but clutters root |

### Duplicate Documentation

| File | Action | Reason |
|---|---|---|
| `memory-bank/15-progress` (no extension) | **DELETE** | Duplicate of `15-progress.md` |
| `memory-bank/16-current-task.md` | **DELETE** | Duplicate of root `current-task.md` (both being deleted) |
| `memory-bank/17-next-task.md` | **DELETE** | Duplicate of root `next-task.md` (both being deleted) |

### Document Directory

| File | Action | Reason |
|---|---|---|
| `document/Chitiet_1.md` | **ARCHIVE** → `docs/archive/` | Historical spec, not active documentation |
| `document/Chitiet_2.md` | **ARCHIVE** → `docs/archive/` | Historical spec |
| `document/TongQuan.md` | **ARCHIVE** → `docs/archive/` | Historical overview |
| `document/UI_goiy.md` | **ARCHIVE** → `docs/archive/` | Historical UI suggestions |
| `document/debai.md` | **KEEP** | Project assignment — still relevant |

> [!NOTE]
> After archiving, the `document/` directory may contain only `debai.md`. Consider moving it to `docs/` to consolidate.

---

## Phase 3 — Frontend Cleanup

### Unused Mock Data Files

Pages import **type definitions** from `data/mock*.ts` files, but some mock files are only referenced internally (not imported by any page or component):

| File | Size | Imported By | Action |
|---|---|---|---|
| `data/mockProfile.ts` | 2.5 KB | **Nobody** (0 imports outside self) | **DELETE** |
| `data/mockResults.ts` | 4.5 KB | **Nobody** (only self-referential exports) | **DELETE** |
| `data/mockExams.ts` | 2.9 KB | **Nobody** (only self-referential, SubjectDetail imports `mockCharts` not `mockExams`) | **DELETE** |

### Mock Data Files That Export Types Still In Use

> [!IMPORTANT]
> These files are imported for their **type definitions** by pages and mappers. They cannot be deleted without first moving types to `types/index.ts` or `types/backend.ts`.

| File | Imported By (types only) |
|---|---|
| `data/mockClasses.ts` | `ClassDetail`, `ClassManagement`, `Gradebook`, `StudentDetail`, `TeacherDashboard`, `ClassMapper` |
| `data/mockStudents.ts` | `StudentDetail`, `StudentMapper` |
| `data/mockSubjects.ts` | `SubjectDetail`, `SubjectManagement`, `TeacherDashboard`, `TakeExam`, `Gradebook`, `QuestionManagement`, `SubjectMapper`, `formatSubject.ts` |
| `data/mockQuestions.ts` | `QuestionManagement`, `QuestionDetail`, `QuestionMapper` |
| `data/mockNotifications.ts` | `NotificationDropdown`, `NotificationItem`, `NotificationList` |
| `data/mockCharts.ts` | `SubjectDetail` |
| `data/difficulty.ts` | `QuestionManagement`, `QuestionMapper`, `DifficultyBadge` — **KEEP** (enum/constant, not mock data) |

**Recommended Approach** (conservative):
1. Extract type interfaces from `mock*.ts` → `types/index.ts` (which already has similar but slightly different definitions).
2. Update imports page by page.
3. Once no page imports from `data/mock*.ts`, delete the data files.
4. **Do NOT do this all at once.** One mock file at a time.

### Empty Directories

| Directory | Action | Reason |
|---|---|---|
| `frontend/src/features/` | **DELETE** | Empty, never populated |
| `frontend/src/styles/` | **DELETE** | Empty, CSS lives in `index.css` |
| `frontend/src/layouts/` | **DELETE** | Empty, layouts are in `components/layouts/` |
| `frontend/src/hooks/` | **DELETE** | Empty, no custom hooks exist |

### Frontend Dist (Build Artifacts)

| Path | Size | Action |
|---|---|---|
| `frontend/dist/` | 14.9 MB | **DELETE** from git, add to `.gitignore` |

### Potential CSS Cleanup

- `frontend/src/index.css` (3.1 KB) is the only CSS file. No unused CSS files exist.
- Tailwind is configured (`tailwind.config.ts`) and used for utility classes.
- No separate CSS files to clean up.

### Duplicate Type Definitions

> [!WARNING]
> This is the biggest frontend debt item. Types exist in **two places**:
>
> - `types/index.ts` + `types/backend.ts` — canonical type module (not imported by pages!)
> - `data/mock*.ts` — mock data files export interfaces that pages actually import
>
> Pages use `import type { ClassItem } from "@/data/mockClasses"` instead of `from "@/types"`.

**Risk**: 🟡 Medium — requires careful import rewriting.

**Rollback**: Git revert per file.

---

## Phase 4 — Backend Cleanup

> [!CAUTION]
> Backend cleanup must be **conservative**. No algorithm changes, no storage format changes, no API changes.

### `test.cpp` (Root)

- **Status**: Not in CMake build. Standalone old test harness (472 lines).
- **Action**: DELETE (Phase 1).
- **Reason**: Tests `Score`, `Student`, `Class`, `Subject` save/load manually. Not automated, not runnable without modifications.

### `store.cpp` + `data.txt` (Root)

- **Status**: Standalone file I/O experiment, not in CMake build.
- **Action**: DELETE (Phase 1).

### `server.cpp` — Conservative Review

> Per policy: only dead helper functions, duplicated utilities, and repeated response code.

| Area | Observation | Action |
|---|---|---|
| `json_response()` | Used extensively, clean | **KEEP** |
| `error_response()` | Used extensively, clean | **KEEP** |
| `custom_json_response()` | Check if still used or redundant with `json_response` | **AUDIT** in execution |
| `set_cors_headers()` | Manually sets CORS — known issue KI-1 (duplicate with `set_default_headers`) | **FLAG** for Phase 8 (production fix, not cleanup) |
| Unused includes | `#include "DArray.h"`, `#include "Utility/Swap.h"` — audit whether actually used in server.cpp | **AUDIT** in execution |

> [!IMPORTANT]
> No endpoint logic may change. Only provably dead helper functions and provably unused includes may be removed.

### `src/` and `include/` — Conservative Review

No dead files identified. All 20 `.cpp` files and 26 `.h` files in `src/`/`include/` are referenced by `CMakeLists.txt` for one or both targets. Deeper dead-code analysis (unused functions within files) should be done file-by-file during execution with grep verification.

---

## Phase 5 — Code Duplication

### High Priority

| Duplication | Files Affected | Severity |
|---|---|---|
| **Type definitions in mock files vs `types/`** | 10 `data/mock*.ts` files vs `types/index.ts` + `types/backend.ts` | 🔴 High |
| **`ClassItem` defined in 3 places** | `types/index.ts`, `data/mockClasses.ts`, (imported by 5+ pages from mock) | 🔴 High |
| **`Subject` defined in 3 places** | `types/index.ts` (`SubjectItem`), `types/backend.ts` (`BackendSubject`), `data/mockSubjects.ts` (`Subject`) | 🔴 High |

### Medium Priority

| Duplication | Files Affected | Severity |
|---|---|---|
| **Public assets duplicated in `dist/`** | 10 PNG files exist in both `frontend/public/` and `frontend/dist/` | 🟡 Medium (goes away when `dist/` is gitignored) |
| **Task/progress files duplicated** | `current-task.md` (root) ↔ `memory-bank/16-current-task.md`; `next-task.md` (root) ↔ `memory-bank/17-next-task.md` | 🟡 Medium |
| **`15-progress` (no ext) ↔ `15-progress.md`** | `memory-bank/` | 🟡 Medium |

### Low Priority

| Duplication | Files Affected | Severity |
|---|---|---|
| **Mapper + service pattern** | 4 mapper files + 10 service files — each mapper converts Backend→Frontend types. Could be simplified once types are unified, but works correctly. | 🟢 Low |
| **`CMakeSettings.json` + `CppProperties.json`** | VS IDE config — duplication of include paths with `CMakeLists.txt`. Harmless. | 🟢 Low |

### Consolidation Recommendations

1. **Types**: Establish `types/index.ts` as the single source of truth. Move all interfaces there. Update all imports. Delete mock type exports.
2. **Mock data**: After types are extracted, the remaining mock data (`mockNotifications`, `mockCharts`) can stay if they serve as fallback/demo data, or be deleted if no longer imported.
3. **Documentation**: Consolidate `memory-bank/` task tracking into a single status document.

---

## Phase 6 — Repository Organization

### `.gitignore` (Root) — MUST CREATE

The repository has **no root `.gitignore`**. This is the highest-priority hygiene fix.

```gitignore
# Compiled binaries
*.exe
*.obj
*.o

# Build outputs
build/
out/
.vs/

# IDE
*.suo
*.ntvs*
*.njsproj
*.sln
*.sw?
.idea/

# Logs
*.log
audit_log.txt
audit_err.txt
startup_log.txt
startup_err.txt

# Backups
storage_backup_*/

# Frontend build
frontend/dist/
frontend/node_modules/
```

> [!IMPORTANT]
> After creating `.gitignore`, run `git rm -r --cached` on already-tracked files that should now be ignored (`build/`, `out/`, `.vs/`, `*.exe`, `frontend/dist/`).

### Folder Cleanup

| Action | Detail |
|---|---|
| Create `docs/` | Already being created (this file). Move `document/debai.md` → `docs/debai.md`. |
| Create `docs/archive/` | Move historical docs (`TASKS.md`, `document/Chitiet_*.md`, `TongQuan.md`, `UI_goiy.md`). |
| Delete `document/` | After moving contents to `docs/`. |
| Remove empty `frontend/src/` dirs | `features/`, `styles/`, `layouts/`, `hooks/`. |

### File Naming

No file renaming needed — naming is consistent within each layer:
- Backend: `PascalCase.h`/`.cpp` ✅
- Frontend pages: `PascalCase.tsx` ✅
- Frontend services: `camelCaseService.ts` ✅
- Frontend components: `PascalCase.tsx` ✅

### Documentation Organization (Final State)

```
docs/
├── ponytail.md          ← This roadmap
├── debai.md             ← Project assignment
└── archive/
    ├── TASKS.md
    ├── Chitiet_1.md
    ├── Chitiet_2.md
    ├── TongQuan.md
    └── UI_goiy.md

memory-bank/             ← Keep as active documentation (00–14, 15, 18)
```

---

## Phase 7 — Verification

> After **every cleanup phase**, run this verification checklist.

### Build Verification

- [ ] `cmake -B build -S .` succeeds
- [ ] `cmake --build build --target server` succeeds
- [ ] `cmake --build build --target console` succeeds
- [ ] `server.exe` starts, listens on `:8080`, responds to `GET /`

### Frontend Verification

- [ ] `cd frontend && npm run build` succeeds with 0 errors
- [ ] `npm run dev` starts dev server
- [ ] All 20 pages load without console errors
- [ ] Teacher login → dashboard → CRUD flows work
- [ ] Student login → dashboard → exam flows work

### Storage Compatibility

- [ ] `storage/*.txt` files are not modified
- [ ] Server loads existing storage data correctly
- [ ] CRUD operations create/update/delete data correctly

### No Regression

- [ ] All REST API endpoints return correct responses
- [ ] Validation behavior unchanged (e.g., duplicate student ID rejected)
- [ ] Exam logic unchanged (Fisher-Yates, used marking, scoring)
- [ ] Question ID generation unchanged (auto-increment)

---

## Risk Matrix

| Risk Level | Description | Examples |
|---|---|---|
| 🟢 **Low** | Deleting files not referenced by build or runtime | Logs, empty files, `store.cpp`, `data.txt`, `test.cpp`, empty dirs |
| 🟡 **Medium** | Deleting/moving documentation, creating `.gitignore`, removing `dist/` from git | `TASKS.md` archive, root markdown cleanup, `.gitignore` creation |
| 🟠 **High** | Refactoring frontend type imports from `mock*.ts` → `types/` | Type consolidation (Phase 3/5) — must be done page by page with testing |
| 🔴 **Critical** | Any change to `server.cpp` endpoint logic, storage format, or business rules | **NOT ALLOWED** by policy — listed here for completeness |

---

## Execution Order

> Each task is designed to be executed independently with its own verification step.
> Execute in order. Stop and verify after each task.

### Phase 1 — Temporary Files (Risk: 🟢 Low)

| # | Task | Difficulty | Risk | Benefit | Files Affected | Rollback |
|---|---|---|---|---|---|---|
| 1.1 | Delete all empty log files (9 files) | Trivial | 🟢 Low | Clean root directory | `audit_log.txt`, `audit_err.txt`, `startup_log.txt`, `startup_err.txt`, `server_startup.log`, `server_startup_err.log`, `server_tc03.log`, `server_tc03_restart.log`, `seed_server.log` | `git checkout` |
| 1.2 | Delete experiment files | Trivial | 🟢 Low | Remove 3 dead files | `store.cpp`, `store.exe`, `data.txt` | `git checkout` |
| 1.3 | Delete old test file | Trivial | 🟢 Low | Remove dead test code | `test.cpp` | `git checkout` |
| 1.4 | Delete `server.exe` binary | Trivial | 🟢 Low | Remove 7 MB binary | `server.exe` | Rebuild |
| 1.5 | Delete 5 `storage_backup_reset_*` directories | Trivial | 🟢 Low | Remove backup clutter | 5 directories (50 files total) | `git checkout` |

### Phase 2 — Documentation (Risk: 🟡 Medium)

| # | Task | Difficulty | Risk | Benefit | Files Affected | Rollback |
|---|---|---|---|---|---|---|
| 2.1 | Delete root tracking markdown files | Trivial | 🟢 Low | Clean root | `Role.md`, `current-task.md`, `next-task.md`, `progress.md` | `git checkout` |
| 2.2 | Delete duplicate memory-bank files | Easy | 🟢 Low | Remove duplicates | `memory-bank/15-progress`, `memory-bank/16-current-task.md`, `memory-bank/17-next-task.md` | `git checkout` |
| 2.3 | Create `docs/archive/` and move historical docs | Easy | 🟡 Medium | Organize documentation | `TASKS.md` → `docs/archive/`, `document/*.md` → `docs/archive/` | `git mv` reverse |

### Phase 3 — Frontend Cleanup (Risk: 🟡–🟠)

| # | Task | Difficulty | Risk | Benefit | Files Affected | Rollback |
|---|---|---|---|---|---|---|
| 3.1 | Delete empty directories | Trivial | 🟢 Low | Clean structure | `features/`, `styles/`, `layouts/`, `hooks/` | Recreate if needed |
| 3.2 | Delete unused mock data files | Easy | 🟢 Low | Remove dead code | `data/mockProfile.ts`, `data/mockResults.ts`, `data/mockExams.ts` | `git checkout` |
| 3.3 | Consolidate type definitions (mock → types/) | Hard | 🟠 High | Single source of truth for types | 10+ mock files, `types/index.ts`, 16+ page/component files | `git revert` per commit |

### Phase 4 — Backend Cleanup (Risk: 🟢–🟡)

| # | Task | Difficulty | Risk | Benefit | Files Affected | Rollback |
|---|---|---|---|---|---|---|
| 4.1 | Audit `server.cpp` unused includes | Easy | 🟡 Medium | Cleaner includes | `server.cpp` | `git checkout` |
| 4.2 | Audit `server.cpp` for dead helper functions | Medium | 🟡 Medium | Remove dead code | `server.cpp` | `git checkout` |

### Phase 5 — Repository Organization (Risk: 🟡 Medium)

| # | Task | Difficulty | Risk | Benefit | Files Affected | Rollback |
|---|---|---|---|---|---|---|
| 5.1 | Create root `.gitignore` | Easy | 🟢 Low | Prevent future clutter | New file | Delete file |
| 5.2 | Remove tracked-but-ignored files from git | Medium | 🟡 Medium | Clean git index | `build/`, `out/`, `.vs/`, `frontend/dist/`, `*.exe` | `git checkout` |
| 5.3 | Update `memory-bank/04-folder-structure.md` | Easy | 🟢 Low | Accurate documentation | 1 file | `git checkout` |

### Phase 6 — Final Verification (Risk: N/A)

| # | Task | Difficulty | Risk | Benefit | Files Affected | Rollback |
|---|---|---|---|---|---|---|
| 6.1 | Run full verification checklist (Phase 7) | Medium | N/A | Confirm zero regression | None | N/A |

---

> [!NOTE]
> **This document is ONLY a roadmap.** No code was generated. No refactoring was performed. No files were modified.
>
> The next sessions will execute **one phase at a time** after explicit approval.
>
> **When in doubt: choose the most conservative solution.**
