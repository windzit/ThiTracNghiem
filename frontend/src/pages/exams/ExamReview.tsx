import { useState, useEffect } from "react"
import { useNavigate, useLocation, useSearchParams } from "react-router-dom"
import {
  Target,
  CheckCircle,
  ArrowLeft,
  Printer,
  XCircle,
  AlertCircle,
  Award,
} from "lucide-react"
import TeacherLayout from "@/widgets/layouts/TeacherLayout"
import { Badge } from "@/shared/ui/badge"
import { Button } from "@/shared/ui/button"
import { Card, CardContent } from "@/shared/ui/card"
import { reportService, type ExamReportResponse } from "@/entities/report/reportService"

export default function ExamReview() {
  const navigate = useNavigate()
  const location = useLocation()
  const [searchParams] = useSearchParams()

  const state = location.state as { malop?: string; masv?: string; mamh?: string } | null
  const malop = state?.malop || searchParams.get("malop") || ""
  const masv = state?.masv || searchParams.get("masv") || ""
  const mamh = state?.mamh || searchParams.get("mamh") || ""

  const [report, setReport] = useState<ExamReportResponse | null>(null)
  const [loading, setLoading] = useState(true)
  const [errorMsg, setErrorMsg] = useState("")

  useEffect(() => {
    if (!masv || !mamh) {
      setErrorMsg("Thiếu thông tin sinh viên hoặc môn thi")
      setLoading(false)
      return
    }

    setLoading(true)
    reportService
      .getExamReport(malop, masv, mamh)
      .then((data) => {
        setReport(data)
        setLoading(false)
      })
      .catch((err: any) => {
        console.error("[ExamReview] getExamReport error:", err)
        setErrorMsg(err?.response?.data?.message || err?.message || "Không tìm thấy chi tiết bài thi")
        setLoading(false)
      })
  }, [malop, masv, mamh])

  const handlePrint = () => {
    window.print()
  }

  const scrollToQuestion = (idx: number) => {
    const el = document.getElementById(`q-card-${idx}`)
    if (el) {
      el.scrollIntoView({ behavior: "smooth", block: "center" })
      el.classList.add("ring-2", "ring-blue-500")
      setTimeout(() => el.classList.remove("ring-2", "ring-blue-500"), 1500)
    }
  }

  if (loading) {
    return (
      <TeacherLayout
        breadcrumb={[
          { label: "Bảng điểm", path: "/teacher/reports" },
          { label: "Chi tiết bài thi" },
        ]}
      >
        <div className="text-center py-12 text-gray-500">Đang tải chi tiết bài thi sinh viên...</div>
      </TeacherLayout>
    )
  }

  if (errorMsg || !report) {
    return (
      <TeacherLayout
        breadcrumb={[
          { label: "Bảng điểm", path: "/teacher/reports" },
          { label: "Chi tiết bài thi" },
        ]}
      >
        <div className="max-w-md mx-auto py-12 text-center space-y-4">
          <div className="text-lg font-semibold text-red-600">{errorMsg || "Không tìm thấy thông tin bài thi"}</div>
          <Button variant="outline" onClick={() => navigate("/teacher/reports")} className="gap-2">
            <ArrowLeft className="h-4 w-4" />
            Quay lại Bảng điểm
          </Button>
        </div>
      </TeacherLayout>
    )
  }

  const questions = report.questions || []
  const correctCount = questions.filter(
    (q) => q.selectedAnswer && q.selectedAnswer.toUpperCase() === q.dapan.toUpperCase()
  ).length
  const unansweredCount = questions.filter((q) => !q.selectedAnswer).length
  const incorrectCount = questions.length - correctCount - unansweredCount
  const percentCorrect = questions.length > 0 ? Math.round((correctCount / questions.length) * 100) : 0

  return (
    <TeacherLayout
      breadcrumb={[
        { label: "Bảng điểm", path: "/teacher/reports" },
        { label: `Chi tiết bài thi: ${report.masv} — ${report.mamh}` },
      ]}
    >
      <style>{`
        @media print {
          body { background-color: white !important; color: black !important; }
          .no-print { display: none !important; }
          .print-full-width { width: 100% !important; max-width: 100% !important; flex: none !important; grid-column: span 12 / span 12 !important; }
          .q-card-pagebreak { page-break-inside: avoid !important; break-inside: avoid !important; margin-bottom: 20px !important; }
        }
      `}</style>

      <div className="space-y-6">
        {/* Top Actions */}
        <div className="flex items-center justify-between no-print">
          <Button variant="ghost" className="gap-2 text-gray-600 hover:bg-gray-100" onClick={() => navigate("/teacher/reports")}>
            <ArrowLeft className="h-4 w-4" />
            Quay lại Bảng điểm
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

        {/* Score & Profile Header Card */}
        <div className="bg-white rounded-xl border border-gray-200 p-6 flex flex-col md:flex-row items-center justify-between gap-6 shadow-sm">
          <div className="space-y-2 text-center md:text-left">
            <h1 className="text-xl font-bold text-gray-900">{report.tenmh}</h1>
            <div className="text-sm text-gray-500 flex flex-wrap items-center justify-center md:justify-start gap-x-6 gap-y-1">
              <span>Sinh viên: <strong className="text-gray-900 font-semibold">{report.ho} {report.ten}</strong> ({report.masv})</span>
              <span>Giới tính: {report.phai}</span>
              <span>Lớp: <strong className="text-gray-900 font-semibold">{malop || "-"}</strong></span>
              <span>Tổng số câu: <strong className="text-gray-900 font-semibold">{questions.length}</strong></span>
            </div>
          </div>
          <div className="flex items-center gap-4 bg-red-50/50 p-4 rounded-xl border border-red-100 flex-shrink-0">
            <div className="w-14 h-14 rounded-full bg-[#D9272B] text-white flex items-center justify-center flex-shrink-0 shadow-md">
              <Award className="h-8 w-8" />
            </div>
            <div>
              <div className="text-xs text-gray-500 uppercase tracking-wider font-semibold">Điểm số bài thi</div>
              <div className="text-3xl font-extrabold text-[#D9272B]">
                {typeof report.diem === "number" ? report.diem.toFixed(2) : report.diem} / 10
              </div>
              <div className="text-xs text-gray-500 mt-0.5">Tỷ lệ đúng: {percentCorrect}%</div>
            </div>
          </div>
        </div>

        {/* Overview Stats */}
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4 no-print">
          <Card className="bg-white border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-blue-50 text-blue-600 flex items-center justify-center">
                <Target className="h-5 w-5" />
              </div>
              <div>
                <div className="text-xs text-gray-500">Tổng số câu</div>
                <div className="text-lg font-bold text-gray-900">{questions.length} câu</div>
              </div>
            </CardContent>
          </Card>
          <Card className="bg-white border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-green-50 text-green-600 flex items-center justify-center">
                <CheckCircle className="h-5 w-5" />
              </div>
              <div>
                <div className="text-xs text-gray-500">Trả lời đúng</div>
                <div className="text-lg font-bold text-green-600">{correctCount} câu</div>
              </div>
            </CardContent>
          </Card>
          <Card className="bg-white border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-red-50 text-red-600 flex items-center justify-center">
                <XCircle className="h-5 w-5" />
              </div>
              <div>
                <div className="text-xs text-gray-500">Trả lời sai</div>
                <div className="text-lg font-bold text-red-600">{incorrectCount} câu</div>
              </div>
            </CardContent>
          </Card>
          <Card className="bg-white border-gray-200">
            <CardContent className="p-4 flex items-center gap-3">
              <div className="w-10 h-10 rounded-lg bg-amber-50 text-amber-600 flex items-center justify-center">
                <AlertCircle className="h-5 w-5" />
              </div>
              <div>
                <div className="text-xs text-gray-500">Bỏ trống</div>
                <div className="text-lg font-bold text-amber-600">{unansweredCount} câu</div>
              </div>
            </CardContent>
          </Card>
        </div>

        {/* Main Content Layout */}
        <div className="grid grid-cols-12 gap-6 items-start">
          {/* Question List Column */}
          <div className="col-span-12 lg:col-span-8 space-y-4 print-full-width">
            <h2 className="text-lg font-bold text-gray-900 no-print">Chi tiết từng câu hỏi trong đề thi</h2>
            {questions.map((q, idx) => {
              const selectedUpper = q.selectedAnswer ? q.selectedAnswer.toUpperCase() : ""
              const dapanUpper = q.dapan.toUpperCase()
              const isCorrect = selectedUpper === dapanUpper
              const isUnanswered = !q.selectedAnswer

              const optionMap: Record<string, string> = { A: q.A, B: q.B, C: q.C, D: q.D }

              return (
                <Card
                  key={q.id}
                  id={`q-card-${idx}`}
                  className={`bg-white border transition-all q-card-pagebreak ${
                    isCorrect
                      ? "border-green-200"
                      : isUnanswered
                      ? "border-amber-200"
                      : "border-red-200"
                  }`}
                >
                  <CardContent className="p-5 space-y-4">
                    <div className="flex items-start justify-between gap-4 border-b border-gray-100 pb-3">
                      <div className="flex items-center gap-2">
                        <span className="text-sm font-bold text-gray-900 bg-gray-100 px-2.5 py-1 rounded-md">
                          Câu {idx + 1}
                        </span>
                        <span className="text-xs text-gray-400 font-mono">(Mã CH: #{q.id})</span>
                      </div>
                      <div>
                        {isCorrect ? (
                          <Badge className="bg-green-100 text-green-800 border-green-200 flex items-center gap-1">
                            <CheckCircle className="h-3.5 w-3.5" /> Đúng
                          </Badge>
                        ) : isUnanswered ? (
                          <Badge className="bg-amber-100 text-amber-800 border-amber-200 flex items-center gap-1">
                            <AlertCircle className="h-3.5 w-3.5" /> Bỏ trống
                          </Badge>
                        ) : (
                          <Badge className="bg-red-100 text-red-800 border-red-200 flex items-center gap-1">
                            <XCircle className="h-3.5 w-3.5" /> Sai
                          </Badge>
                        )}
                      </div>
                    </div>

                    {/* Question Content */}
                    <div className="text-sm font-medium text-gray-900 leading-relaxed whitespace-pre-wrap">
                      {q.noidung}
                    </div>

                    {/* Options List */}
                    <div className="space-y-2 pt-1">
                      {["A", "B", "C", "D"].map((optKey) => {
                        const optText = optionMap[optKey] || ""
                        const isChosen = selectedUpper === optKey
                        const isRightOpt = dapanUpper === optKey

                        let optionStyle = "border-gray-200 bg-gray-50/50 text-gray-700"
                        if (isRightOpt) {
                          optionStyle = "border-green-300 bg-green-50 text-green-900 font-medium ring-1 ring-green-400"
                        } else if (isChosen && !isRightOpt) {
                          optionStyle = "border-red-300 bg-red-50 text-red-900 font-medium ring-1 ring-red-400"
                        }

                        return (
                          <div
                            key={optKey}
                            className={`p-3 rounded-lg border text-sm flex items-start justify-between gap-3 ${optionStyle}`}
                          >
                            <div className="flex items-start gap-3">
                              <span
                                className={`w-6 h-6 rounded-full text-xs font-bold flex items-center justify-center flex-shrink-0 mt-0.5 ${
                                  isRightOpt
                                    ? "bg-green-600 text-white"
                                    : isChosen
                                    ? "bg-red-600 text-white"
                                    : "bg-gray-200 text-gray-700"
                                }`}
                              >
                                {optKey}
                              </span>
                              <span className="pt-0.5">{optText}</span>
                            </div>
                            <div className="flex-shrink-0 text-xs font-semibold">
                              {isRightOpt && <span className="text-green-700">Đáp án đúng</span>}
                              {isChosen && !isRightOpt && <span className="text-red-600">SV chọn</span>}
                            </div>
                          </div>
                        );
                      })}
                    </div>
                  </CardContent>
                </Card>
              )
            })}
          </div>

          {/* Question Navigator Sidebar (no-print) */}
          <div className="col-span-12 lg:col-span-4 sticky top-6 space-y-4 no-print">
            <Card className="bg-white border-gray-200">
              <CardContent className="p-5 space-y-4">
                <h3 className="text-sm font-bold text-gray-900 border-b border-gray-100 pb-3">
                  Danh sách câu hỏi ({questions.length})
                </h3>

                <div className="grid grid-cols-5 gap-2">
                  {questions.map((q, idx) => {
                    const selectedUpper = q.selectedAnswer ? q.selectedAnswer.toUpperCase() : ""
                    const dapanUpper = q.dapan.toUpperCase()
                    const isCorrect = selectedUpper === dapanUpper
                    const isUnanswered = !q.selectedAnswer

                    let bgStyle = "bg-red-100 text-red-800 border-red-300 hover:bg-red-200"
                    if (isCorrect) bgStyle = "bg-green-100 text-green-800 border-green-300 hover:bg-green-200"
                    if (isUnanswered) bgStyle = "bg-amber-100 text-amber-800 border-amber-300 hover:bg-amber-200"

                    return (
                      <button
                        key={q.id}
                        onClick={() => scrollToQuestion(idx)}
                        className={`h-9 text-xs font-bold rounded-lg border transition-colors flex items-center justify-center ${bgStyle}`}
                        title={`Câu ${idx + 1}: ${isCorrect ? "Đúng" : isUnanswered ? "Chưa làm" : "Sai"}`}
                      >
                        {idx + 1}
                      </button>
                    )
                  })}
                </div>

                <div className="pt-3 border-t border-gray-100 space-y-2 text-xs text-gray-600">
                  <div className="flex items-center justify-between">
                    <span className="flex items-center gap-1.5">
                      <span className="w-3 h-3 rounded-sm bg-green-200 border border-green-400" /> Đúng
                    </span>
                    <span className="font-semibold text-green-700">{correctCount}</span>
                  </div>
                  <div className="flex items-center justify-between">
                    <span className="flex items-center gap-1.5">
                      <span className="w-3 h-3 rounded-sm bg-red-200 border border-red-400" /> Sai
                    </span>
                    <span className="font-semibold text-red-700">{incorrectCount}</span>
                  </div>
                  <div className="flex items-center justify-between">
                    <span className="flex items-center gap-1.5">
                      <span className="w-3 h-3 rounded-sm bg-amber-200 border border-amber-400" /> Bỏ trống
                    </span>
                    <span className="font-semibold text-amber-700">{unansweredCount}</span>
                  </div>
                </div>
              </CardContent>
            </Card>
          </div>
        </div>
      </div>
    </TeacherLayout>
  )
}
