import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom"
import RoleSelection from "@/pages/auth/RoleSelection"
import StudentLogin from "@/pages/auth/StudentLogin"
import TeacherLogin from "@/pages/auth/TeacherLogin"
import StudentDashboard from "@/pages/dashboard/StudentDashboard"
import TeacherDashboard from "@/pages/dashboard/TeacherDashboard"
import ClassManagement from "@/pages/classes/ClassManagement"
import SubjectManagement from "@/pages/subjects/SubjectManagement"
import QuestionManagement from "@/pages/questions/QuestionManagement"
import TakeExam from "@/pages/exams/TakeExam"
import Gradebook from "@/pages/reports/Gradebook"
import ExamDetail from "@/pages/exams/ExamDetail"
import StudentDetail from "@/pages/students/StudentDetail"
import SubjectDetail from "@/pages/subjects/SubjectDetail"
import ClassDetail from "@/pages/classes/ClassDetail"
import QuestionDetail from "@/pages/questions/QuestionDetail"
import ExamConfirm from "@/pages/exams/ExamConfirm"
import TakingExam from "@/pages/exams/TakingExam"
import ExamSubmitted from "@/pages/exams/ExamSubmitted"
import ExamHistory from "@/pages/exams/ExamHistory"
import ExamLayout from "@/widgets/layouts/ExamLayout"
import { RootErrorBoundary, GlobalConnectionOverlay } from "@/shared/components"
import { ConnectionProvider } from "@/app/providers/ConnectionContext"
import { ToastProvider } from "@/app/providers/ToastContext"
import { ExamSessionProvider } from "@/app/providers/ExamSessionContext"

import ExamReview from "@/pages/exams/ExamReview"

export default function App() {
  return (
    <RootErrorBoundary>
      <ConnectionProvider>
        <ExamSessionProvider>
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
        </ExamSessionProvider>
      </ConnectionProvider>
    </RootErrorBoundary>
  )
}