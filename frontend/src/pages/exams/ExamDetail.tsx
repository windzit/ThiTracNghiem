import { useState, useEffect } from "react"
import { useNavigate, useParams } from "react-router-dom"
import {
  Target,
  CheckCircle,
  BookOpen,
  ArrowLeft,
  Printer,
  XCircle,
  AlertCircle,
  Award,
  HelpCircle,
} from "lucide-react"
import StudentLayout from "@/widgets/layouts/StudentLayout"
import { Badge } from "@/shared/ui/badge"
import { Button } from "@/shared/ui/button"
import { Card, CardContent } from "@/shared/ui/card"
import { authService } from "@/entities/session/authService"
import { reportService, type ExamReportResponse } from "@/entities/report/reportService"

export default function ExamDetail() {
  const navigate = useNavigate()
  const { examId } = useParams<{ examId: string }>()
  const [report, setReport] = useState<ExamReportResponse | null>(null)
  const [loading, setLoading] = useState(true)
  const [errorMsg, setErrorMsg] = useState("")

  useEffect(() => {
    const user = authService.getCurrentUser()
    const masv = user?.id || "N21DCCN004"
    const malop = user?.classCode || "D21CQCN01"
    const mamh = examId || "CTDLGT"

    setLoading(true)
    reportService
      .getExamReport(malop, masv, mamh)
      .then((data) => {
        setReport(data)
        setLoading(false)
      })
      .catch((err: any) => {
        console.error("[ExamDetail] getExamReport error:", err)
        setErrorMsg(err?.response?.data?.message || err?.message || "Không tìm thấy chi tiết bài thi")
        setLoading(false)
      })
  }, [examId])

  const handlePrint = () => {
    window.print()
  }

  const scrollToQuestion = (idx: number) => {
    const el = document.getElementById(`q-card-${idx}`)
    if (el) {
      el.scrollIntoView({ behavior: "smooth", block: "center" })
      // Add brief highlight flash animation
      el.classList.add("ring-2", "ring-blue-500")
      setTimeout(() => el.classList.remove("ring-2", "ring-blue-500"), 1500)
    }
  }

  if (loading) {
    return (
      <StudentLayout>
        <div className="text-center py-12 text-gray-500">Đang tải chi tiết bài thi...</div>
      </StudentLayout>
    )
  }

  if (errorMsg || !report) {
    return (
      <StudentLayout>
        <div className="max-w-md mx-auto py-12 text-center space-y-4">
          <div className="text-lg font-semibold text-red-600">{errorMsg || "Không tìm thấy thông tin bài thi"}</div>
          <Button variant="outline" onClick={() => navigate("/student/history")} className="gap-2">
            <ArrowLeft className="h-4 w-4" />
            Quay lại Lịch sử thi
          </Button>
        </div>
      </StudentLayout>
    )
  }

  const questions = report.questions || []
  const correctCount = questions.filter(
    (q) => q.selectedAnswer && q.selectedAnswer.toUpperCase() === q.dapan.toUpperCase()
  ).length
  const unansweredCount = questions.filter((q) => !q.selectedAnswer).length
  const incorrectCount = questions.length - correctCount - unansweredCount
  const percentCorrect = questions.length > 0 ? Math.round((correctCount / questions.length) * 100) : 0
  const avgQuestionPoints = questions.length > 0 ? (10 / questions.length) : 0

  return (
    <StudentLayout
      breadcrumb={[
        { label: "Lịch sử thi", path: "/student/history" },
        { label: `Chi tiết bài thi ${report.tenmh}` },
      ]}
    >
      {/* Print-specific style block */}
      <style>{`
        @media print {
          body {
            background-color: white !important;
            color: black !important;
          }
          .no-print {
            display: none !important;
          }
          .print-full-width {
            width: 100% !important;
            max-width: 100% !important;
            flex: none !important;
            grid-column: span 12 / span 12 !important;
          }
          .q-card-pagebreak {
            page-break-inside: avoid !important;
            break-inside: avoid !important;
            margin-bottom: 20px !important;
          }
        }
      `}</style>

      <div className="space-y-6">
        {/* Top Header */}
        <div className="flex items-center justify-between no-print">
          <Button variant="ghost" className="gap-2 text-gray-600 hover:bg-gray-100" onClick={() => navigate("/student/history")}>
            <ArrowLeft className="h-4 w-4" />
            Quay lại Lịch sử thi
          </Button>
          <div className="flex gap-2">
            <Button onClick={handlePrint} variant="outline" className="gap-2 border-gray-300">
              <Printer className="h-4 w-4" />
              In bài thi
            </Button>
            <Badge className="bg-[#FEF2F2] text-[#D9272B] border border-red-200 text-sm px-3 py-1 font-bold">
              Môn thi: {report.mamh}
            </Badge>
          </div>
        </div>

        {/* Score & Profile Card */}
        <div className="bg-white rounded-xl border border-gray-200 p-6 flex flex-col md:flex-row items-center justify-between gap-6 shadow-sm">
          <div className="space-y-2">
            <h1 className="text-xl font-bold text-gray-900">{report.tenmh}</h1>
            <div className="text-sm text-gray-500 flex flex-wrap items-center gap-x-6 gap-y-1">
              <span>Sinh viên: <strong className="text-gray-700">{report.ho} {report.ten}</strong> ({report.masv})</span>
              <span>Giới tính: {report.phai}</span>
              <span>Tổng số câu: <strong className="text-gray-700">{questions.length}</strong></span>
            </div>
          </div>

          <div className="flex items-center gap-6">
            <div className="text-center px-6 py-3 bg-red-50 rounded-xl border border-red-100 min-w-[120px]">
              <div className="text-xs text-red-500 font-semibold uppercase tracking-wider">Điểm số</div>
              <div className="text-3xl font-extrabold text-[#D9272B]">{report.diem.toFixed(2)} / 10</div>
            </div>
          </div>
        </div>

        {/* Stats Dashboard Grid */}
        <div className="grid grid-cols-2 sm:grid-cols-4 gap-4">
          <Card className="bg-white shadow-sm border border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-green-50 text-green-600 flex items-center justify-center">
                <CheckCircle className="h-5 w-5" />
              </div>
              <div>
                <div className="text-[11px] text-gray-500 font-semibold uppercase tracking-wider">Câu đúng</div>
                <div className="text-lg font-bold text-green-700">{correctCount} câu</div>
              </div>
            </CardContent>
          </Card>

          <Card className="bg-white shadow-sm border border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-red-50 text-red-600 flex items-center justify-center">
                <XCircle className="h-5 w-5" />
              </div>
              <div>
                <div className="text-[11px] text-gray-500 font-semibold uppercase tracking-wider">Câu sai</div>
                <div className="text-lg font-bold text-red-700">{incorrectCount} câu</div>
              </div>
            </CardContent>
          </Card>

          <Card className="bg-white shadow-sm border border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-gray-50 text-gray-500 flex items-center justify-center">
                <HelpCircle className="h-5 w-5" />
              </div>
              <div>
                <div className="text-[11px] text-gray-500 font-semibold uppercase tracking-wider">Bỏ trống</div>
                <div className="text-lg font-bold text-gray-700">{unansweredCount} câu</div>
              </div>
            </CardContent>
          </Card>

          <Card className="bg-white shadow-sm border border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-blue-50 text-blue-600 flex items-center justify-center">
                <Award className="h-5 w-5" />
              </div>
              <div>
                <div className="text-[11px] text-gray-500 font-semibold uppercase tracking-wider">Tỷ lệ đúng</div>
                <div className="text-lg font-bold text-blue-700">{percentCorrect}%</div>
              </div>
            </CardContent>
          </Card>
        </div>

        {/* Main Content Layout */}
        <div className="grid grid-cols-1 lg:grid-cols-4 gap-6">
          {/* Left - Question List */}
          <div className="lg:col-span-3 space-y-6 print-full-width">
            <div className="bg-white rounded-xl border border-gray-200 p-6 space-y-6 shadow-sm">
              <h2 className="text-lg font-bold text-gray-900 flex items-center gap-2 border-b border-gray-100 pb-4">
                <BookOpen className="h-5 w-5 text-blue-500" />
                Chi tiết bài thi của sinh viên
              </h2>

              <div className="space-y-6">
                {questions.map((q, idx) => {
                  const studentAns = q.selectedAnswer?.toUpperCase() || ""
                  const isCorrect = studentAns === q.dapan.toUpperCase()
                  const status = !studentAns ? "unanswered" : isCorrect ? "correct" : "incorrect"
                  const points = isCorrect ? avgQuestionPoints : 0

                  return (
                    <div
                      key={q.id}
                      id={`q-card-${idx}`}
                      className="p-5 rounded-xl border border-gray-200 space-y-4 shadow-sm q-card-pagebreak transition-all duration-300"
                    >
                      <div className="flex items-center justify-between border-b border-gray-50 pb-2">
                        <div className="flex items-center gap-2">
                          <span className="font-extrabold text-blue-600 text-base">Câu {idx + 1}</span>
                          <Badge variant="outline" className="text-[10px] text-gray-400">ID: {q.id}</Badge>
                        </div>
                        <div className="flex items-center gap-2">
                          {status === "correct" && (
                            <Badge className="bg-green-50 text-green-700 border border-green-200 hover:bg-green-50 font-bold text-xs gap-1">
                              <CheckCircle className="h-3 w-3" /> Đúng (+{points.toFixed(2)} đ)
                            </Badge>
                          )}
                          {status === "incorrect" && (
                            <Badge className="bg-red-50 text-red-700 border border-red-200 hover:bg-red-50 font-bold text-xs gap-1">
                              <XCircle className="h-3 w-3" /> Sai (+0.00 đ)
                            </Badge>
                          )}
                          {status === "unanswered" && (
                            <Badge className="bg-gray-50 text-gray-500 border border-gray-200 hover:bg-gray-50 font-bold text-xs gap-1">
                              <AlertCircle className="h-3 w-3" /> Bỏ trống (+0.00 đ)
                            </Badge>
                          )}
                        </div>
                      </div>

                      <div className="text-sm font-semibold text-gray-900 leading-relaxed">{q.noidung}</div>

                      <div className="grid grid-cols-1 sm:grid-cols-2 gap-3 text-sm">
                        {["A", "B", "C", "D"].map((optLetter) => {
                          const optionText = q[optLetter as "A" | "B" | "C" | "D"]
                          const isCorrectOpt = q.dapan.toUpperCase() === optLetter
                          const isStudentSelected = studentAns === optLetter

                          let optStyle = "border-gray-200 bg-white"
                          if (isCorrectOpt) {
                            optStyle = "bg-green-50 border-green-400 text-green-900 font-medium"
                          } else if (isStudentSelected && !isCorrect) {
                            optStyle = "bg-red-50 border-red-400 text-red-900"
                          }

                          return (
                            <div key={optLetter} className={`p-3 rounded-lg border-2 flex items-center justify-between ${optStyle}`}>
                              <span className="leading-relaxed">
                                <strong className="mr-1">{optLetter}.</strong> {optionText}
                              </span>
                              <div className="flex gap-1 flex-shrink-0">
                                {isStudentSelected && (
                                  <span className="text-[10px] uppercase font-bold bg-blue-100 text-blue-700 px-1.5 py-0.5 rounded">Bạn chọn</span>
                                )}
                                {isCorrectOpt && (
                                  <span className="text-[10px] uppercase font-bold bg-green-100 text-green-700 px-1.5 py-0.5 rounded">Đáp án đúng</span>
                                )}
                              </div>
                            </div>
                          )
                        })}
                      </div>
                    </div>
                  )
                })}
              </div>
            </div>
          </div>

          {/* Right - Sidebar Question Navigator (no-print) */}
          <div className="space-y-6 no-print">
            <Card className="bg-white sticky top-6 shadow-sm border border-gray-200">
              <CardContent className="p-4 space-y-4">
                <h3 className="text-sm font-bold text-gray-900 border-b border-gray-100 pb-2 flex items-center gap-1.5">
                  <Target className="h-4 w-4 text-blue-500" />
                  Bảng điều hướng câu hỏi
                </h3>

                <div className="grid grid-cols-5 gap-2">
                  {questions.map((q, idx) => {
                    const studentAns = q.selectedAnswer?.toUpperCase() || ""
                    const isCorrect = studentAns === q.dapan.toUpperCase()
                    const status = !studentAns ? "unanswered" : isCorrect ? "correct" : "incorrect"

                    let btnStyle = "bg-gray-100 text-gray-500 border border-gray-200 hover:bg-gray-200"
                    if (status === "correct") {
                      btnStyle = "bg-green-500 hover:bg-green-600 text-white border-green-600"
                    } else if (status === "incorrect") {
                      btnStyle = "bg-red-500 hover:bg-red-600 text-white border-red-600"
                    }

                    return (
                      <button
                        key={q.id}
                        onClick={() => scrollToQuestion(idx)}
                        className={`h-9 w-9 text-xs rounded-lg font-bold flex items-center justify-center transition-all duration-200 shadow-sm ${btnStyle}`}
                      >
                        {idx + 1}
                      </button>
                    )
                  })}
                </div>

                <div className="border-t border-gray-100 pt-3 space-y-2 text-xs text-gray-500">
                  <div className="flex items-center gap-2">
                    <div className="h-3 w-3 bg-green-500 rounded" />
                    <span>Trả lời đúng</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <div className="h-3 w-3 bg-red-500 rounded" />
                    <span>Trả lời sai</span>
                  </div>
                  <div className="flex items-center gap-2">
                    <div className="h-3 w-3 bg-gray-100 border border-gray-200 rounded" />
                    <span>Bỏ trống</span>
                  </div>
                </div>
              </CardContent>
            </Card>
          </div>
        </div>
      </div>
    </StudentLayout>
  )
}