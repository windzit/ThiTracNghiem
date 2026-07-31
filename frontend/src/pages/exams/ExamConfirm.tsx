import { useState } from "react"
import { useNavigate, useLocation } from "react-router-dom"
import { ArrowLeft, BookOpen, Hash, Clock, AlertTriangle, Shield } from "lucide-react"
import StudentLayout from "@/widgets/layouts/StudentLayout"
import { Button } from "@/shared/ui/button"
import { Card, CardContent, CardHeader, CardTitle } from "@/shared/ui/card"
import { examSessionService } from "@/entities/exam/examSessionService"

export default function ExamConfirm() {
  const navigate = useNavigate()
  const location = useLocation()
  const examData = location.state as {
    subject: string
    subjectId: string
    numQuestions: number
    timeLimit: number
    timeLimitSec?: number
    timeLimitLabel?: string
  } | null

  const [isStarting, setIsStarting] = useState(false)

  if (!examData) {
    navigate("/student/take-exam")
    return null
  }

  const handleStart = async () => {
    if (isStarting) return
    setIsStarting(true)

    // Start exam session
    examSessionService.startSession()

    // Request fullscreen
    try {
      const el = document.documentElement
      if (el.requestFullscreen) {
        await el.requestFullscreen()
      }
    } catch {
      // Fullscreen not supported or denied — continue anyway
    }

    navigate("/student/taking-exam", { state: examData })
  }

  const rules = [
    "Bài thi được random từ ngân hàng câu hỏi của môn học đã chọn.",
    "Thời gian làm bài tính từ khi bạn nhấn \"Bắt đầu làm bài\". Không thể tạm dừng.",
    "Không được chuyển tab, mở ứng dụng khác hoặc rời khỏi màn hình làm bài.",
    "Hệ thống sẽ tự động nộp bài nếu phát hiện vi phạm quá 3 lần.",
    "Kết quả bài thi sẽ được lưu và hiển thị ngay sau khi nộp bài.",
  ]

  return (
    <StudentLayout>
      <div className="max-w-3xl mx-auto space-y-6">
        {/* Title */}
        <div className="flex items-center gap-3">
          <button
            onClick={() => navigate("/student/take-exam")}
            className="p-2 rounded-lg hover:bg-gray-100 transition-colors"
          >
            <ArrowLeft className="h-5 w-5 text-gray-500" />
          </button>
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Xác nhận thông tin bài thi</h1>
            <p className="text-sm text-gray-500 mt-0.5">Vui lòng kiểm tra kỹ trước khi bắt đầu</p>
          </div>
        </div>

        {/* Exam Info Card */}
        <Card>
          <CardHeader>
            <CardTitle className="text-lg flex items-center gap-2">
              <BookOpen className="h-5 w-5 text-blue-500" />
              Thông tin bài thi
            </CardTitle>
          </CardHeader>
          <CardContent>
            <div className="grid grid-cols-3 gap-4">
              <div className="bg-gray-50 rounded-lg p-4">
                <div className="text-xs text-gray-500 mb-1">Môn học</div>
                <div className="text-sm font-semibold text-gray-900">{examData.subject}</div>
              </div>
              <div className="bg-gray-50 rounded-lg p-4">
                <div className="text-xs text-gray-500 mb-1 flex items-center gap-1">
                  <Hash className="h-3 w-3" /> Số câu hỏi
                </div>
                <div className="text-sm font-semibold text-gray-900">{examData.numQuestions} câu</div>
              </div>
              <div className="bg-gray-50 rounded-lg p-4">
                <div className="text-xs text-gray-500 mb-1 flex items-center gap-1">
                  <Clock className="h-3 w-3" /> Thời gian
                </div>
                <div className="text-sm font-semibold text-gray-900">
                  {examData.timeLimitLabel || `${examData.timeLimit} phút`}
                </div>
              </div>
            </div>
          </CardContent>
        </Card>

        {/* Rules Card */}
        <Card className="border-red-200">
          <CardHeader className="bg-red-50 border-b border-red-200">
            <CardTitle className="text-lg flex items-center gap-2 text-red-800">
              <Shield className="h-5 w-5" />
              Quy định phòng thi
            </CardTitle>
          </CardHeader>
          <CardContent className="pt-4">
            <ol className="space-y-3">
              {rules.map((rule, i) => (
                <li key={i} className="flex items-start gap-3">
                  <span className="flex-shrink-0 w-6 h-6 rounded-full bg-red-100 text-red-700 text-xs font-bold flex items-center justify-center mt-0.5">
                    {i + 1}
                  </span>
                  <span className="text-sm text-gray-700">{rule}</span>
                </li>
              ))}
            </ol>

            <div className="mt-4 bg-amber-50 border border-amber-200 rounded-lg p-3 flex items-start gap-2">
              <AlertTriangle className="h-4 w-4 text-amber-500 mt-0.5 flex-shrink-0" />
              <p className="text-xs text-amber-700 font-medium">
                Bằng việc nhấn "Bắt đầu làm bài", bạn đồng ý tuân thủ tất cả quy định trên. Vi phạm sẽ dẫn đến việc bài thi bị tự động nộp.
              </p>
            </div>
          </CardContent>
        </Card>

        {/* Action Buttons */}
        <div className="flex items-center gap-3">
          <Button
            variant="outline"
            className="flex-1 h-11"
            onClick={() => navigate("/student/take-exam")}
          >
            <ArrowLeft className="h-4 w-4 mr-2" />
            Quay lại
          </Button>
          <Button
            className="flex-1 h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
            disabled={isStarting}
            onClick={handleStart}
          >
            {isStarting ? "Đang khởi tạo..." : "Bắt đầu làm bài"}
          </Button>
        </div>
      </div>
    </StudentLayout>
  )
}