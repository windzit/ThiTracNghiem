# 03 — Tech Stack

## Backend (C++ Console Application)

| Technology | Version | Usage | Status |
|-----------|---------|-------|--------|
| C++ Standard | C++17 | Core application language | ✅ Verified |
| CMake | ≥ 3.20 | Build system | ✅ Verified |
| httplib.h | Header-only | HTTP server library (server.cpp) | ⚠ Stub only |
| conio.h | Windows | Console input (getch) | ✅ Verified |
| windows.h | Windows | Console API | ✅ Verified |
| fstream | C++ std | File I/O | ✅ Verified |
| filesystem | C++ std | Path handling | ✅ Verified |

**Confidence**: Verified

## Frontend (React + TypeScript + Vite)

| Technology | Version | Usage | Status |
|-----------|---------|-------|--------|
| React | ^18 | UI framework | ✅ Verified |
| TypeScript | ~5.7 | Type safety | ✅ Verified |
| Vite | ^6 | Build tool | ✅ Verified |
| React Router | ^7 | Client-side routing | ✅ Verified |
| Tailwind CSS | ^4 | Utility CSS | ✅ Verified |
| shadcn/ui | Latest | UI component library | ✅ Verified |
| Recharts | ^2 | Charts | ✅ Verified |
| Zod | ^3 | Schema validation | ✅ Verified |
| Axios | ^1 | HTTP client | ✅ Installed, unused |
| React Hook Form | ^7 | Form management | ✅ Verified |
| lucide-react | Latest | Icons | ✅ Verified |

**Confidence**: Verified

## Frontend Dependencies (from package.json)

### Runtime Dependencies
- `react`, `react-dom`: Core
- `react-router-dom`: Routing
- `recharts`: Charts for gradebook/results
- `zod`: Schema validation
- `react-hook-form`, `@hookform/resolvers`: Form handling
- `axios`: HTTP (unused — frontend uses mock data)
- `@radix-ui/react-slot`, `@radix-ui/react-radio-group`: UI primitives
- `@base-ui/react`: Base UI components
- `class-variance-authority`, `clsx`, `tailwind-merge`: CSS utilities
- `lucide-react`: Icons
- `shadcn`: CLI tool
- `tw-animate-css`: Animation

### Dev Dependencies
- `typescript`, `@types/react`, `@types/react-dom`
- `vite`, `@tailwindcss/vite`
- `eslint`, `@eslint/js`, `eslint-plugin-react-hooks`, `eslint-plugin-react-refresh`
- `tailwindcss`

**Confidence**: Verified