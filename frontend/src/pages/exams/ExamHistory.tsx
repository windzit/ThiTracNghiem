import { useState, useEffect } from "react"
import { useNavigate } from "react-router-dom"
import { FileText, Eye, CheckCircle2, Clock, AlertTriangle, BookOpen } from "lucide-react"
import StudentLayout from "@/widgets/layouts/StudentLayout"
import { Button } from "@/shared/ui/button"
import { authService } from "@/entities/session/authService"
import { reportService } from "@/entities/report/reportService"
import { subjectService } from "@/entities/subject/subjectService"

interface StudentExamResult {
  mamh: string
  tenmh: string
  score: number
}

export default function ExamHistory() {
  const navigate = useNavigate()
  const [takenExams, setTakenExams] = useState<StudentExamResult[]>([])
  const [loading, setLoading] = useState(true)
  const [errorMsg, setErrorMsg] = useState("")

  useEffect(() => {
    const user = authService.getCurrentUser()
    const masv = user?.id || "N21DCCN004"
    const malop = user?.classCode || "D21CQCN01"

    setLoading(true)
    setErrorMsg("")

    subjectService
      .getAll()
      .then((subList) => {
        const map: Record<string, string> = {}
        subList.forEach((s) => {
          map[s.code] = s.name
        })

        // Load student scores via scoreboard API
        reportService
          .getScoreboard(malop)
          .then((res) => {
            const me = res.students?.find((s) => s.masv === masv)
            if (me && me.scores) {
              const takenList: StudentExamResult[] = Object.entries(me.scores)
                .filter(([_, score]) => score !== null && score !== undefined)
                .map(([mamh, score]) => ({
                  mamh,
                  tenmh: map[mamh] || mamh,
                  score: score as number,
                }))
              setTakenExams(takenList)
            } else {
              setTakenExams([])
            }
            setLoading(false)
          })
          .catch((err: any) => {
            console.error("[ExamHistory] error loading scores:", err)
            setErrorMsg(err?.response?.data?.message || err?.message || "Không thể tải lịch sử điểm thi")
            setTakenExams([])
            setLoading(false)
          })
      })
      .catch((err: any) => {
        console.error("[ExamHistory] error loading subjects:", err)
        setErrorMsg(err?.message || "Không thể tải danh sách môn học")
        setLoading(false)
      })
  }, [])

  const avgScore =
    takenExams.length > 0
      ? (takenExams.reduce((sum, e) => sum + e.score, 0) / takenExams.length).toFixed(2)
      : "0.00"

  return (
    <StudentLayout
      breadcrumb={[
        { label: "Trang chủ", path: "/student/dashboard" },
        { label: "Lịch sử thi" },
      ]}
    >
      <div className="space-y-6">
        {/* Title */}
        <div>
          <h1 className="text-2xl font-bold text-gray-900">Lịch sử thi trắc nghiệm</h1>
          <p className="text-sm text-gray-500 mt-1">Xem lại kết quả tất cả các môn thi của bạn</p>
        </div>

        {/* Error Banner */}
        {errorMsg && (
          <div className="bg-red-50 border border-red-200 text-red-700 p-4 rounded-xl flex items-center justify-between text-sm">
            <div className="flex items-center gap-2">
              <AlertTriangle className="h-5 w-5 text-red-500 flex-shrink-0" />
              <span>{errorMsg}</span>
            </div>
            <Button variant="ghost" size="sm" onClick={() => setErrorMsg("")} className="text-red-700 hover:bg-red-100">
              Đóng
            </Button>
          </div>
        )}

        {/* Stats */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          <div className="bg-white rounded-xl border border-gray-200 p-5 flex items-center gap-4">
            <div className="w-12 h-12 rounded-xl bg-green-50 flex items-center justify-center flex-shrink-0">
              <CheckCircle2 className="h-6 w-6 text-green-500" />
            </div>
            <div>
              <div className="text-xs text-gray-500">Số môn đã thi</div>
              <div className="text-2xl font-bold text-gray-900">{takenExams.length}</div>
            </div>
          </div>
          <div className="bg-white rounded-xl border border-gray-200 p-5 flex items-center gap-4">
            <div className="w-12 h-12 rounded-xl bg-purple-50 flex items-center justify-center flex-shrink-0">
              <Clock className="h-6 w-6 text-purple-500" />
            </div>
            <div>
              <div className="text-xs text-gray-500">Điểm trung bình các môn đã thi</div>
              <div className="text-2xl font-bold text-gray-900">{avgScore}</div>
            </div>
          </div>
        </div>

        {/* Exam List */}
        <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
          <div className="px-5 py-4 border-b border-gray-200">
            <h2 className="text-base font-bold text-gray-900">Danh sách kết quả các môn đã thi</h2>
          </div>

          <div className="divide-y divide-gray-100">
            {loading ? (
              <div className="p-8 text-center text-gray-500 text-sm">Đang tải lịch sử thi...</div>
            ) : takenExams.length === 0 ? (
              <div className="p-12 text-center space-y-3">
                <div className="w-12 h-12 rounded-full bg-blue-50 text-blue-500 flex items-center justify-center mx-auto">
                  <BookOpen className="h-6 w-6 text-blue-500" />
                </div>
                <div className="text-base font-bold text-gray-900">Bạn chưa tham gia kỳ thi nào.</div>
                <p className="text-sm text-gray-500">Hãy chọn môn học và bắt đầu bài thi trắc nghiệm.</p>
                <Button
                  onClick={() => navigate("/student/take-exam")}
                  className="bg-[#D9272B] hover:bg-[#C42226] text-white gap-2 mt-2"
                >
                  Vào thi ngay
                </Button>
              </div>
            ) : (
              takenExams.map((exam) => (
                <div
                  key={exam.mamh}
                  className="flex items-center justify-between px-5 py-4 hover:bg-gray-50/50 transition-colors"
                >
                  <div className="flex items-center gap-4 flex-1 min-w-0">
                    <div className="w-10 h-10 rounded-lg bg-blue-50 flex items-center justify-center flex-shrink-0">
                      <FileText className="h-5 w-5 text-blue-500" />
                    </div>
                    <div className="flex-1 min-w-0">
                      <div className="text-sm font-semibold text-gray-900 truncate">
                        {exam.tenmh} ({exam.mamh})
                      </div>
                    </div>
                  </div>
                  <div className="flex items-center gap-4 flex-shrink-0">
                    <div className="text-right">
                      <div
                        className={`text-lg font-bold ${
                          exam.score >= 5.0 ? "text-green-600" : "text-red-500"
                        }`}
                      >
                        {exam.score.toFixed(2)}
                      </div>
                      <div className="text-[10px] text-gray-400">/ 10 điểm</div>
                    </div>

                    <Button
                      variant="outline"
                      size="sm"
                      className="gap-1.5"
                      onClick={() => navigate(`/student/exams/${exam.mamh}`)}
                    >
                      <Eye className="h-4 w-4" />
                      Xem chi tiết
                    </Button>
                  </div>
                </div>
              ))
            )}
          </div>
        </div>
      </div>
    </StudentLayout>
  )
}