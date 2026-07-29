import { useNavigate, useLocation } from "react-router-dom"
import { CheckCircle2, Home, BookOpen, FileText, Clock, Calendar } from "lucide-react"
import { Button } from "@/components/ui/button"
import { Badge } from "@/components/ui/badge"
import StudentLayout from "@/components/layouts/StudentLayout"

export default function ExamSubmitted() {
  const navigate = useNavigate()
  const location = useLocation()
  const examData = location.state as {
    subject: string
    subjectId: string
    numQuestions: number
    timeLimit: number
    soDung?: number
    total?: number
    diem?: number
  } | null

  if (!examData) {
    navigate("/student/dashboard")
    return null
  }

  const now = new Date()
  const submittedAt = now.toLocaleString("vi-VN", {
    day: "2-digit",
    month: "2-digit",
    year: "numeric",
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
  })
  const examCode = `${examData.subjectId.toUpperCase()}-${now.getTime().toString(36).toUpperCase()}`

  return (
    <StudentLayout
      breadcrumb={[
        { label: "Trang chủ", path: "/student/dashboard" },
        { label: "Nộp bài thành công" },
      ]}
    >
      <div className="flex items-center justify-center p-6">
        <div className="max-w-lg w-full">
          <div className="bg-white rounded-2xl border border-gray-200 p-8 text-center space-y-6">
            {/* Icon */}
            <div className="flex justify-center">
              <div className="w-20 h-20 rounded-full bg-green-50 flex items-center justify-center">
                <CheckCircle2 className="h-10 w-10 text-green-500" />
              </div>
            </div>

            {/* Title */}
            <div>
              <h1 className="text-2xl font-bold text-gray-900">Bài thi đã được nộp thành công</h1>
              <p className="text-sm text-gray-500 mt-2">
                {examData.diem !== undefined
                  ? `Kết quả thi của bạn: ${Number(examData.diem).toFixed(2)} điểm (${examData.soDung}/${examData.total} câu đúng)`
                  : "Bài làm của bạn đã được ghi nhận."}
              </p>
            </div>

            {/* Info */}
            <div className="bg-gray-50 rounded-xl p-5 space-y-3 text-left">
              <div className="flex items-center gap-3">
                <BookOpen className="h-4 w-4 text-gray-400 flex-shrink-0" />
                <div>
                  <div className="text-xs text-gray-500">Môn học</div>
                  <div className="text-sm font-medium text-gray-900">{examData.subject}</div>
                </div>
              </div>
              {examData.diem !== undefined && (
                <div className="flex items-center gap-3">
                  <FileText className="h-4 w-4 text-gray-400 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Điểm số đạt được</div>
                    <div className="text-lg font-bold text-[#D9272B]">{Number(examData.diem).toFixed(2)} / 10 điểm</div>
                  </div>
                </div>
              )}
              <div className="flex items-center gap-3">
                <FileText className="h-4 w-4 text-gray-400 flex-shrink-0" />
                <div>
                  <div className="text-xs text-gray-500">Mã bài thi</div>
                  <div className="text-sm font-medium text-gray-900">{examCode}</div>
                </div>
              </div>
              <div className="flex items-center gap-3">
                <Clock className="h-4 w-4 text-gray-400 flex-shrink-0" />
                <div>
                  <div className="text-xs text-gray-500">Thời gian làm bài</div>
                  <div className="text-sm font-medium text-gray-900">{examData.timeLimit} phút</div>
                </div>
              </div>
              <div className="flex items-center gap-3">
                <Calendar className="h-4 w-4 text-gray-400 flex-shrink-0" />
                <div>
                  <div className="text-xs text-gray-500">Thời điểm nộp bài</div>
                  <div className="text-sm font-medium text-gray-900">{submittedAt}</div>
                </div>
              </div>
            </div>

            {/* Status */}
            <div className="flex items-center justify-center gap-2">
              <Badge className="bg-green-50 text-green-700 border border-green-200 font-medium text-sm px-3 py-1.5 rounded-full">
                Đã hoàn thành và ghi nhận điểm số
              </Badge>
            </div>

            {/* Button */}
            <Button
              className="w-full h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold gap-2"
              onClick={() => navigate("/student/dashboard")}
            >
              <Home className="h-4 w-4" />
              Về Dashboard
            </Button>
          </div>
        </div>
      </div>
    </StudentLayout>
  )
}