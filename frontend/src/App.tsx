import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom"
import RoleSelection from "./pages/RoleSelection"
import StudentLogin from "./pages/StudentLogin"
import TeacherLogin from "./pages/TeacherLogin"
import StudentDashboard from "./pages/StudentDashboard"
import TeacherDashboard from "./pages/TeacherDashboard"
import ClassManagement from "./pages/ClassManagement"
import SubjectManagement from "./pages/SubjectManagement"
import QuestionManagement from "./pages/QuestionManagement"
import TakeExam from "./pages/TakeExam"
import Gradebook from "./pages/Gradebook"
import ExamDetail from "./pages/ExamDetail"
import StudentDetail from "./pages/StudentDetail"
import SubjectDetail from "./pages/SubjectDetail"
import ClassDetail from "./pages/ClassDetail"
import QuestionDetail from "./pages/QuestionDetail"
import ExamConfirm from "./pages/ExamConfirm"
import TakingExam from "./pages/TakingExam"
import ExamSubmitted from "./pages/ExamSubmitted"
import ExamHistory from "./pages/ExamHistory"
import ExamLayout from "./components/layouts/ExamLayout"
import { RootErrorBoundary, GlobalConnectionOverlay } from "./components/shared"
import { ConnectionProvider } from "./context/ConnectionContext"
import { ToastProvider } from "./context/ToastContext"

import ExamReview from "./pages/ExamReview"

export default function App() {
  return (
    <RootErrorBoundary>
      <ConnectionProvider>
        <ToastProvider>
          <GlobalConnectionOverlay />
        <BrowserRouter>
          <Routes>
            <Route path="/" element={<RoleSelection />} />
            
            {/* Student Routes */}
            <Route path="/student/login" element={<StudentLogin />} />
            <Route path="/student/dashboard" element={<StudentDashboard />} />
            <Route path="/student/take-exam" element={<TakeExam />} />
            <Route path="/student/exam-confirm" element={<ExamConfirm />} />
            <Route element={<ExamLayout />}>
              <Route path="/student/taking-exam" element={<TakingExam />} />
            </Route>
            <Route path="/student/exam-submitted" element={<ExamSubmitted />} />
            <Route path="/student/history" element={<ExamHistory />} />
            <Route path="/student/exams/:examId" element={<ExamDetail />} />

            {/* Teacher Routes */}
            <Route path="/teacher/login" element={<TeacherLogin />} />
            <Route path="/teacher/dashboard" element={<TeacherDashboard />} />
            <Route path="/teacher/classes" element={<ClassManagement />} />
            <Route path="/teacher/classes/:classId" element={<ClassDetail />} />
            <Route path="/teacher/students/:id" element={<StudentDetail />} />
            <Route path="/teacher/subjects" element={<SubjectManagement />} />
            <Route path="/teacher/subjects/:id" element={<SubjectDetail />} />
            <Route path="/teacher/questions" element={<QuestionManagement />} />
            <Route path="/teacher/questions/:id" element={<QuestionDetail />} />
            <Route path="/teacher/gradebook" element={<Gradebook />} />
            <Route path="/teacher/reports" element={<Gradebook />} />
            <Route path="/teacher/exam-review" element={<ExamReview />} />

            <Route path="*" element={<Navigate to="/" replace />} />
          </Routes>
        </BrowserRouter>
        </ToastProvider>
      </ConnectionProvider>
    </RootErrorBoundary>
  )
}