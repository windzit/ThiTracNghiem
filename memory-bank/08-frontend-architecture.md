# 08 — Frontend Architecture

## Overview
React 18 + TypeScript + Vite frontend with 23 routes, shadcn/ui components, and mock data services. Currently **disconnected** from the C++ backend — all data is mocked.

**Confidence**: Verified

## Routes (from App.tsx)

| Path | Page Component | Role |
|------|---------------|------|
| `/` | RoleSelection | Landing page |
| `/student/login` | StudentLogin | Student auth |
| `/student/dashboard` | StudentDashboard | Student home |
| `/student/take-exam` | TakeExam | Exam config |
| `/student/exam-confirm` | ExamConfirm | Confirm exam |
| `/student/taking-exam` | TakingExam | Active exam (ExamLayout) |
| `/student/exam-submitted` | ExamSubmitted | Post-submit |
| `/student/history` | ExamHistory | Past exams |
| `/student/profile` | StudentProfilePage | Profile |
| `/student/notifications` | StudentNotifications | Notifications |
| `/student/exams/:examId` | ExamDetail | Exam detail |
| `/teacher/login` | TeacherLogin | Teacher auth |
| `/teacher/dashboard` | TeacherDashboard | Teacher home |
| `/teacher/classes` | ClassManagement | Class list |
| `/teacher/classes/:classId` | ClassDetail | Class detail |
| `/teacher/subjects` | SubjectManagement | Subject list |
| `/teacher/subjects/:id` | SubjectDetail | Subject detail |
| `/teacher/questions` | QuestionManagement | Question list |
| `/teacher/questions/:id` | QuestionDetail | Question detail |
| `/teacher/create-exam` | CreateExam | Create exam |
| `/teacher/exam-preview` | ExamPreview | Preview exam |
| `/teacher/exam-results` | ExamResults | Results list |
| `/teacher/gradebook` | Gradebook | Gradebook view |
| `/teacher/exams` | TeacherExamList | Exam list |
| `/teacher/exams/:id` | TeacherExamDetail | Exam detail |
| `/teacher/students/:id` | StudentDetail | Student detail |
| `/teacher/profile` | TeacherProfilePage | Profile |
| `/teacher/activity` | TeacherActivity | Activity log |
| `*` | Navigate to `/` | Catch-all |

**Confidence**: Verified

## Services (from frontend/src/services/)

| Service | File | Methods | Status |
|---------|------|---------|--------|
| authService | authService.ts | login, logout, getCurrentUser | Mock |
| examService | examService.ts | getExams, getExamById, createExam, submitExam | Mock |
| questionService | questionService.ts | getQuestions, getQuestionById, createQuestion, updateQuestion, deleteQuestion | Mock |
| studentService | studentService.ts | getStudents, getStudentById | Mock |
| classService | classService.ts | getClasses, getClassById | Mock |
| subjectService | subjectService.ts | getSubjects, getSubjectById | Mock |
| resultService | resultService.ts | getResults, getResultById | Mock |
| notificationService | notificationService.ts | getNotifications, subscribe | Mock |
| examSessionService | examSessionService.ts | startSession, submitAnswer, endSession | Mock |

**Confidence**: Verified

## Mock Data (from frontend/src/data/)

| File | Content |
|------|---------|
| mockExams.ts | Exam data (mockExams, recentExamsTeacher, upcomingExamsStudent, upcomingExamsTeacher) |
| mockStudents.ts | Student data (firstNames, lastNames, middleNames, mockStudents) |
| mockResults.ts | Exam results (mockResults, barDataResults, donutDataResults, classSummary) |
| mockClasses.ts | Class data |
| mockSubjects.ts | Subject data |
| mockCharts.ts | Chart data (gradebookBarData, gradebookDonutData, profileChartData) |
| mockNotifications.ts | Notification data |
| mockProfile.ts | Profile data (examHistory, recentActivity, semesterResults) |
| difficulty.ts | Question difficulty types |

**Confidence**: Verified

## UI Components (shadcn/ui)

| Component | File |
|-----------|------|
| Button | components/ui/button.tsx |
| Card | components/ui/card.tsx |
| Badge | components/ui/badge.tsx |
| Input | components/ui/input.tsx |
| Select | components/ui/select.tsx |
| Switch | components/ui/switch.tsx |
| Label | components/ui/label.tsx |
| Table | components/ui/table.tsx |
| Form | components/ui/form.tsx |
| RadioGroup | components/ui/radio-group.tsx |
| Separator | components/ui/separator.tsx |
| Textarea | components/ui/textarea.tsx |

**Confidence**: Verified

## State Management
- No global state management library (Redux, Zustand, etc.)
- State managed locally within components and via React Router params
- Services are singleton objects with mock data

**Confidence**: Verified

## Key Observations
1. **Axios is installed but unused** — all services return mock data directly
2. **No API base URL configuration** — no `.env` or config for backend connection
3. **Frontend and backend are completely independent projects**
4. **Exam flow is fully mocked** — TakingExam.tsx has full UI but no real backend

**Confidence**: Verified