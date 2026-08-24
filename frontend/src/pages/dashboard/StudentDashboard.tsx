import StudentLayout from "@/widgets/layouts/StudentLayout"
import { useNavigate } from "react-router-dom"
import {
  ClipboardList,
  TrendingUp,
  CalendarDays,
  ArrowRight,
  PlayCircle,
  Clock,
  CheckCircle2,
  Award,
} from "lucide-react"
import { PieChart, Pie, Cell, ResponsiveContainer } from "recharts"
import { authService } from "@/entities/session/authService"
import { resultService, type StudentStats } from "@/entities/exam/resultService"
import { useExamSession } from "@/app/providers/ExamSessionContext"
import { useState, useEffect } from "react"
import { Button } from "@/shared/ui/button"
import { EmptyState } from "@/shared/components"

const quickActions = [
  { icon: ClipboardList, iconBg: "bg-red-50", iconColor: "text-[#D9272B]", title: "Vào thi ngay", subtitle: "Tham gia kiểm tra kiến thức", path: "/student/take-exam" },
  { icon: CalendarDays, iconBg: "bg-blue-50", iconColor: "text-[#3B82F6]", title: "Lịch sử thi", subtitle: "Xem chi tiết kết quả bài thi", path: "/student/history" },
]

export default function StudentDashboard() {
  const navigate = useNavigate()
  const user = authService.getCurrentUser()
  const [stats, setStats] = useState<StudentStats | null>(null)
  const [loading, setLoading] = useState(true)
  const { activeSession } = useExamSession()

  useEffect(() => {
    const masv = user?.id || "N21DCCN004"
    const malop = user?.classCode || "D21CQCN01"

    setLoading(true)
    resultService.getStudentStats(malop, masv).then((data) => {
      setStats(data)
      setLoading(false)
    })
  }, [user])

  const handleResumeExam = () => {
    if (!activeSession) return
    const answersMap: Record<number, number> = {}
    const mappedQs = activeSession.questions.map((q) => {
      if (q.selectedAnswer) {
        const idx = ["A", "B", "C", "D"].indexOf(q.selectedAnswer.toUpperCase())
        if (idx !== -1) answersMap[q.id] = idx
      }
      return {
        id: String(q.id),
        numericId: q.id,
        content: q.noidung,
        options: [q.A, q.B, q.C, q.D],
        type: "Trắc nghiệm 1 đáp án",
      }
    })

    navigate("/student/taking-exam", {
      state: {
        resumedSession: {
          subject: activeSession.tenmh || activeSession.mamh,
          subjectId: activeSession.mamh,
          numQuestions: activeSession.questions.length,
          timeLimit: activeSession.thoiGianPhut,
          remainingSeconds: activeSession.remainingSeconds,
          questions: mappedQs,
          answersMap,
          fullscreenRequired: !!activeSession.fullscreenRequired,
        },
      },
    })
  }

  const takenExams = stats ? stats.takenExams : 0
  const passedExams = stats ? stats.passedExams : 0
  const passRate = takenExams > 0 ? Math.round((passedExams / takenExams) * 100) : 0

  const allScores = stats ? stats.scoresList.map((s) => s.score) : []
  const highestScore = allScores.length > 0 ? Math.max(...allScores).toFixed(1) : "-"
  const lowestScore = allScores.length > 0 ? Math.min(...allScores).toFixed(1) : "-"

  const statCards = [
    { icon: ClipboardList, value: String(takenExams), label: "Bài", sublabel: "Bài thi đã làm", iconBg: "bg-red-50", iconColor: "text-[#D9272B]" },
    { icon: CheckCircle2, value: String(passedExams), label: "Bài", sublabel: "Bài thi Đạt (≥5.0)", iconBg: "bg-green-50", iconColor: "text-[#10B981]" },
    { icon: TrendingUp, value: stats?.avgScoreFormatted || "0.00", label: "/ 10", sublabel: "Điểm trung bình", iconBg: "bg-blue-50", iconColor: "text-[#3B82F6]" },
    { icon: Award, value: `${passRate}%`, label: "", sublabel: "Tỷ lệ Đạt", iconBg: "bg-purple-50", iconColor: "text-[#8B5CF6]" },
  ]

  const chartData = stats?.chartData || [
    { name: "Đạt (≥5.0)", value: 0, color: "#10B981" },
    { name: "Chưa đạt (<5.0)", value: 0, color: "#EF4444" },
  ]

  return (
    <StudentLayout>
      <div className="space-y-6">
        {/* Active Resume Exam Banner */}
        {activeSession && activeSession.remainingSeconds > 0 && (
          <div className="bg-gradient-to-r from-red-600 to-amber-600 text-white rounded-2xl p-5 shadow-md flex flex-col sm:flex-row items-start sm:items-center justify-between gap-4 animate-pulse">
            <div className="space-y-1">
              <div className="flex items-center gap-2 text-xs font-semibold uppercase tracking-wider bg-white/20 px-2.5 py-0.5 rounded-full w-fit">
                <Clock className="h-3.5 w-3.5" /> Bài thi chưa hoàn thành
              </div>
              <h2 className="text-lg font-bold">
                Môn {activeSession.tenmh || activeSession.mamh} ({activeSession.mamh})
              </h2>
              <p className="text-xs text-white/90">
                Thời gian còn lại: {Math.floor(activeSession.remainingSeconds / 60)} phút {activeSession.remainingSeconds % 60} giây
              </p>
            </div>
            <Button
              onClick={handleResumeExam}
              className="bg-white text-red-700 hover:bg-gray-100 font-bold gap-2 flex-shrink-0"
            >
              <PlayCircle className="h-5 w-5 text-red-600" /> Tiếp tục bài thi
            </Button>
          </div>
        )}

        {/* Welcome */}
        <div>
          <h1 className="text-2xl font-bold text-gray-900 flex items-center gap-2">
            Xin chào, {user?.name || "Sinh viên"}!
          </h1>
          <p className="text-sm text-gray-500 mt-1">Chúc bạn một ngày học tập và thi cử hiệu quả!</p>
        </div>

        {/* Stat Cards */}
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
          {statCards.map((card) => {
            const Icon = card.icon
            return (
              <div key={card.sublabel} className="bg-white rounded-xl border border-gray-200 p-5 flex items-start gap-4 shadow-sm">
                <div className={`w-12 h-12 rounded-xl ${card.iconBg} flex items-center justify-center flex-shrink-0`}>
                  <Icon className={`h-6 w-6 ${card.iconColor}`} />
                </div>
                <div>
                  <div className="text-[13px] text-gray-500 mb-1">{card.sublabel}</div>
                  <div className="flex items-baseline gap-1">
                    <span className="text-[28px] font-bold text-gray-900 leading-none">{card.value}</span>
                    {card.label && <span className="text-sm text-gray-400">{card.label}</span>}
                  </div>
                </div>
              </div>
            )
          })}
        </div>

        {/* Extended Stats Bar */}
        {takenExams > 0 && (
          <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 bg-gray-50 p-4 rounded-xl border border-gray-200 text-sm">
            <div className="flex items-center justify-between px-2">
              <span className="text-gray-500 font-medium">Điểm cao nhất:</span>
              <span className="font-bold text-green-700 bg-green-50 px-2.5 py-1 rounded-md border border-green-200">{highestScore} / 10</span>
            </div>
            <div className="flex items-center justify-between px-2">
              <span className="text-gray-500 font-medium">Điểm thấp nhất:</span>
              <span className="font-bold text-red-700 bg-red-50 px-2.5 py-1 rounded-md border border-red-200">{lowestScore} / 10</span>
            </div>
          </div>
        )}

        {/* Main Content: Recent Results + Quick Stats */}
        <div className="grid grid-cols-1 lg:grid-cols-5 gap-6">
          {/* Recent Results */}
          <div className="lg:col-span-3 bg-white rounded-xl border border-gray-200 p-5 shadow-sm">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-base font-bold text-gray-900">Kết quả bài thi gần đây</h2>
              <button
                onClick={() => navigate("/student/history")}
                className="text-sm text-[#D9272B] font-medium flex items-center gap-1 hover:underline"
              >
                Xem tất cả <ArrowRight className="h-3.5 w-3.5" />
              </button>
            </div>
            <div className="space-y-0">
              {loading ? (
                <div className="py-8 text-center text-sm text-gray-400">Đang tải kết quả thi...</div>
              ) : !stats || stats.scoresList.length === 0 ? (
                <EmptyState
                  title="Bạn chưa tham gia bài thi nào"
                  description="Hãy bắt đầu kiểm tra kiến thức của bạn bằng cách tham gia bài thi đầu tiên."
                  actionLabel="Vào thi ngay"
                  onAction={() => navigate("/student/take-exam")}
                />
              ) : (
                stats.scoresList.map((item, i) => (
                  <div
                    key={item.mamh}
                    className={`flex items-center justify-between py-3 ${i > 0 ? "border-t border-gray-100" : ""}`}
                  >
                    <div>
                      <div className="text-sm font-semibold text-gray-900">Môn thi: {item.mamh}</div>
                    </div>
                    <div className="flex items-center gap-2 flex-shrink-0">
                      <span
                        className={`text-xs font-semibold px-2 py-0.5 rounded ${
                          item.score >= 5.0 ? "bg-green-100 text-green-800" : "bg-red-100 text-red-800"
                        }`}
                      >
                        {item.score >= 5.0 ? "Đạt" : "Chưa đạt"}
                      </span>
                      <span
                        className={`text-sm font-bold px-2.5 py-1 rounded-lg ${
                          item.score >= 5.0 ? "bg-[#F0FDF4] text-[#16A34A]" : "bg-[#FEF2F2] text-[#DC2626]"
                        }`}
                      >
                        {item.score.toFixed(2)} / 10
                      </span>
                    </div>
                  </div>
                ))
              )}
            </div>
          </div>

          {/* Quick Stats Pie Chart */}
          <div className="lg:col-span-2 bg-white rounded-xl border border-gray-200 p-5 shadow-sm">
            <h2 className="text-base font-bold text-gray-900 mb-4">Kết quả các bài thi</h2>
            {takenExams === 0 ? (
              <div className="py-6">
                <EmptyState
                  title="Chưa có dữ liệu bài thi"
                  description="Tham gia kỳ thi đầu tiên để xem phân tích biểu đồ kết quả."
                />
              </div>
            ) : (
              <div className="flex items-center gap-6">
                <div className="relative w-[130px] h-[130px] flex-shrink-0">
                  <ResponsiveContainer width="100%" height="100%">
                    <PieChart>
                      <Pie
                        data={chartData.filter((d) => d.value > 0)}
                        cx="50%"
                        cy="50%"
                        innerRadius={38}
                        outerRadius={58}
                        paddingAngle={3}
                        dataKey="value"
                        strokeWidth={0}
                      >
                        {chartData
                          .filter((d) => d.value > 0)
                          .map((entry, index) => (
                            <Cell key={`cell-${index}`} fill={entry.color} />
                          ))}
                      </Pie>
                    </PieChart>
                  </ResponsiveContainer>
                  <div className="absolute inset-0 flex flex-col items-center justify-center text-center">
                    <span className="text-2xl font-bold text-gray-900">{takenExams}</span>
                    <span className="text-[10px] text-gray-400 uppercase tracking-tight">Bài hoàn thành</span>
                  </div>
                </div>
                <div className="space-y-3 flex-1">
                  {chartData.map((item) => (
                    <div key={item.name} className="flex items-center justify-between text-sm">
                      <div className="flex items-center gap-2">
                        <div className="w-2.5 h-2.5 rounded-full" style={{ backgroundColor: item.color }} />
                        <span className="text-gray-600 font-medium">{item.name}</span>
                      </div>
                      <span className="font-bold text-gray-900">
                        {item.value} <span className="text-xs text-gray-400 font-normal">({takenExams > 0 ? Math.round((item.value / takenExams) * 100) : 0}%)</span>
                      </span>
                    </div>
                  ))}
                </div>
              </div>
            )}
          </div>
        </div>

        {/* Quick Actions */}
        <div>
          <h2 className="text-base font-bold text-gray-900 mb-4">Thao tác nhanh</h2>
          <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
            {quickActions.map((action) => {
              const Icon = action.icon
              return (
                <button
                  key={action.title}
                  onClick={() => navigate(action.path)}
                  className="bg-white rounded-xl border border-gray-200 p-5 flex items-center gap-4 hover:shadow-md hover:border-red-200 transition-all group text-left"
                >
                  <div className={`w-12 h-12 rounded-xl ${action.iconBg} flex items-center justify-center flex-shrink-0 group-hover:scale-105 transition-transform`}>
                    <Icon className={`h-6 w-6 ${action.iconColor}`} />
                  </div>
                  <div>
                    <div className="text-base font-bold text-gray-900 group-hover:text-[#D9272B] transition-colors">{action.title}</div>
                    <div className="text-xs text-gray-500 mt-0.5">{action.subtitle}</div>
                  </div>
                </button>
              )
            })}
          </div>
        </div>
      </div>
    </StudentLayout>
  )
}