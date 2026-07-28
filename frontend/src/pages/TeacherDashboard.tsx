import TeacherLayout from "@/components/layouts/TeacherLayout"
import { useNavigate } from "react-router-dom"
import {
  Users,
  User,
  BookOpen,
  HelpCircle,
  AlertCircle,
  BarChart3,
  Calendar,
  Layers,
  FileCheck,
  Shield,
} from "lucide-react"
import { PieChart, Pie, Cell, ResponsiveContainer, BarChart, Bar, XAxis, YAxis, Tooltip } from "recharts"
import { authService } from "@/services/authService"
import { classService } from "@/services/classService"
import { subjectService } from "@/services/subjectService"
import { systemSettingsService } from "@/services/systemSettingsService"
import type { ClassItem, Subject } from "@/types"
import { useState, useEffect } from "react"
import { EmptyState } from "@/components/shared"
import { useToast } from "@/context/ToastContext"
import { ApiErrorHandler } from "@/utils/ApiErrorHandler"

const COLORS = ["#8B5CF6", "#3B82F6", "#10B981", "#F59E0B", "#EF4444", "#EC4899", "#6366F1"]

const quickActions = [
  { icon: Users, iconBg: "bg-purple-50", iconColor: "text-[#8B5CF6]", title: "Quản lý lớp học", subtitle: "Xem danh sách và sĩ số", path: "/teacher/classes" },
  { icon: BookOpen, iconBg: "bg-green-50", iconColor: "text-[#10B981]", title: "Quản lý môn học", subtitle: "Môn học và câu hỏi", path: "/teacher/subjects" },
  { icon: HelpCircle, iconBg: "bg-orange-50", iconColor: "text-[#F59E0B]", title: "Ngân hàng câu hỏi", subtitle: "Tạo và sửa câu hỏi", path: "/teacher/questions" },
  { icon: FileCheck, iconBg: "bg-red-50", iconColor: "text-[#D9272B]", title: "Bảng điểm báo cáo", subtitle: "Xem kết quả thi", path: "/teacher/reports" },
]

export default function TeacherDashboard() {
  const navigate = useNavigate()
  const user = authService.getCurrentUser()
  const { showSuccess, showError } = useToast()
  const [classes, setClasses] = useState<ClassItem[]>([])
  const [subjects, setSubjects] = useState<Subject[]>([])
  const [loading, setLoading] = useState(true)
  const [fullscreenRequired, setFullscreenRequired] = useState(false)
  const [isUpdatingSettings, setIsUpdatingSettings] = useState(false)

  useEffect(() => {
    setLoading(true)
    Promise.all([
      classService.getAll().catch(() => []),
      subjectService.getAll().catch(() => []),
      systemSettingsService.getSettings().catch(() => ({ fullscreenRequired: false })),
    ]).then(([clsData, subData, settingsData]) => {
      setClasses(clsData || [])
      setSubjects(subData || [])
      setFullscreenRequired(!!settingsData.fullscreenRequired)
      setLoading(false)
    })
  }, [])

  const handleToggleFullscreen = async () => {
    const newValue = !fullscreenRequired
    setIsUpdatingSettings(true)
    try {
      await systemSettingsService.updateSettings(newValue)
      setFullscreenRequired(newValue)
      showSuccess("Cập nhật cài đặt hệ thống thành công", newValue ? "Đã BẬT yêu cầu toàn màn hình khi thi" : "Đã TẮT yêu cầu toàn màn hình khi thi")
    } catch (err: any) {
      const parsed = ApiErrorHandler.handle(err)
      showError("Không thể cập nhật cài đặt", parsed.message, parsed.code)
    } finally {
      setIsUpdatingSettings(false)
    }
  }

  const totalClasses = classes.length
  const totalStudents = classes.reduce((sum, c) => sum + (c.current || c.total || 0), 0)
  const totalSubjects = subjects.length
  const totalQuestions = subjects.reduce((sum, s) => sum + (s.questionCount || 0), 0)
  const emptySubjects = subjects.filter((s) => (s.questionCount || 0) === 0).length

  const statCards = [
    { icon: Users, value: String(totalClasses), label: "Tổng lớp học", sublabel: "Lớp học trong hệ thống", iconBg: "bg-purple-50", iconColor: "text-[#8B5CF6]" },
    { icon: User, value: String(totalStudents), label: "Tổng sinh viên", sublabel: "Sinh viên đăng ký", iconBg: "bg-blue-50", iconColor: "text-[#3B82F6]" },
    { icon: BookOpen, value: String(totalSubjects), label: "Tổng môn học", sublabel: "Môn học đã khởi tạo", iconBg: "bg-green-50", iconColor: "text-[#10B981]" },
    { icon: HelpCircle, value: String(totalQuestions), label: "Tổng câu hỏi", sublabel: "Ngân hàng trắc nghiệm", iconBg: "bg-orange-50", iconColor: "text-[#F59E0B]" },
    { icon: AlertCircle, value: String(emptySubjects), label: "Môn chưa có câu hỏi", sublabel: "Cần bổ sung ngân hàng", iconBg: "bg-red-50", iconColor: "text-[#D9272B]" },
  ]

  // Chart 1: Real Question count per subject
  const subjectBarData = subjects.map((s) => ({
    name: s.code,
    value: s.questionCount || 0,
    fullName: s.name,
  }))

  // Chart 2: Real Student count per class
  const classPieData = classes.map((c, i) => ({
    name: c.id,
    value: c.current || c.total || 0,
    fullName: c.name,
    color: COLORS[i % COLORS.length],
  }))

  // Top classes by student count
  const topClasses = [...classes].sort((a, b) => (b.current || b.total || 0) - (a.current || a.total || 0)).slice(0, 4)

  // Top subjects by question count
  const topSubjects = [...subjects].sort((a, b) => (b.questionCount || 0) - (a.questionCount || 0)).slice(0, 4)

  const currentDate = new Date().toLocaleDateString("vi-VN", {
    weekday: "long",
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
  })

  return (
    <TeacherLayout>
      <div className="space-y-6">
        {/* Welcome + Date */}
        <div className="flex flex-col lg:flex-row items-start justify-between gap-4">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Xin chào, {user?.name || "Giảng viên"}!</h1>
            <p className="text-sm text-gray-500 mt-1">Tổng quan quản lý lớp học và ngân hàng đề thi hệ thống.</p>
          </div>
          <div className="bg-white rounded-xl border border-gray-200 px-5 py-3 flex items-center gap-3 shadow-sm">
            <Calendar className="h-5 w-5 text-[#D9272B]" />
            <div>
              <div className="text-sm font-semibold text-gray-900 capitalize">{currentDate}</div>
              <div className="text-xs text-gray-400">Hệ thống Thi Trắc Nghiệm PTIT</div>
            </div>
          </div>
        </div>

        {/* Stat Cards */}
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-5 gap-4">
          {statCards.map((card) => {
            const Icon = card.icon
            return (
              <div key={card.label} className="bg-white rounded-xl border border-gray-200 p-5 flex items-start gap-4 shadow-sm">
                <div className={`w-12 h-12 rounded-xl ${card.iconBg} flex items-center justify-center flex-shrink-0`}>
                  <Icon className={`h-6 w-6 ${card.iconColor}`} />
                </div>
                <div>
                  <div className="text-[13px] text-gray-500 mb-1">{card.label}</div>
                  <div className="text-[26px] font-bold text-gray-900 leading-none">{loading ? "..." : card.value}</div>
                  <div className="text-[11px] text-gray-400 mt-1.5">{card.sublabel}</div>
                </div>
              </div>
            )
          })}
        </div>

        {/* System Settings Global Card */}
        <div className="bg-white rounded-xl border border-gray-200 p-5 shadow-sm">
          <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
            <div className="flex items-start gap-3">
              <div className="w-10 h-10 rounded-xl bg-red-50 flex items-center justify-center flex-shrink-0 mt-0.5">
                <Shield className="h-5 w-5 text-[#D9272B]" />
              </div>
              <div>
                <div className="flex items-center gap-2">
                  <h2 className="text-base font-bold text-gray-900">Cài đặt hệ thống (Toàn cục)</h2>
                  <span className={`text-xs px-2.5 py-0.5 rounded-full font-semibold ${fullscreenRequired ? "bg-red-100 text-red-700" : "bg-gray-100 text-gray-600"}`}>
                    {fullscreenRequired ? "Đang bật" : "Đang tắt"}
                  </span>
                </div>
                <p className="text-xs text-gray-500 mt-1">
                  Yêu cầu chế độ toàn màn hình khi sinh viên thực hiện bài thi. Cài đặt này áp dụng cho toàn hệ thống và có hiệu lực ngay cho các bài thi được tạo sau khi lưu.
                </p>
              </div>
            </div>
            <button
              onClick={handleToggleFullscreen}
              disabled={isUpdatingSettings}
              className={`relative inline-flex h-6 w-11 flex-shrink-0 cursor-pointer rounded-full border-2 border-transparent transition-colors duration-200 ease-in-out focus:outline-none ${fullscreenRequired ? "bg-[#D9272B]" : "bg-gray-300"}`}
            >
              <span
                className={`pointer-events-none inline-block h-5 w-5 transform rounded-full bg-white shadow ring-0 transition duration-200 ease-in-out ${fullscreenRequired ? "translate-x-5" : "translate-x-0"}`}
              />
            </button>
          </div>
        </div>

        {/* Charts Row */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-6">
          {/* Chart 1: Questions per subject (Bar) */}
          <div className="lg:col-span-7 bg-white rounded-xl border border-gray-200 p-5 shadow-sm">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-base font-bold text-gray-900 flex items-center gap-2">
                <BarChart3 className="h-5 w-5 text-[#D9272B]" /> Ngân hàng câu hỏi theo môn học
              </h2>
              <button onClick={() => navigate("/teacher/subjects")} className="text-xs text-[#D9272B] font-medium hover:underline">
                Quản lý môn học
              </button>
            </div>
            {subjects.length === 0 ? (
              <EmptyState title="Chưa có môn học nào" description="Tạo môn học mới để bắt đầu xây dựng ngân hàng đề." />
            ) : (
              <ResponsiveContainer width="100%" height={220}>
                <BarChart data={subjectBarData} barSize={32}>
                  <XAxis dataKey="name" tick={{ fontSize: 11, fill: "#6B7280" }} axisLine={false} tickLine={false} />
                  <YAxis tick={{ fontSize: 11, fill: "#6B7280" }} axisLine={false} tickLine={false} />
                  <Tooltip
                    contentStyle={{ fontSize: 12, borderRadius: 8, border: "1px solid #E5E7EB" }}
                    formatter={(value, _, props) => [`${value} câu hỏi`, props.payload.fullName]}
                  />
                  <Bar dataKey="value" fill="#D9272B" radius={[4, 4, 0, 0]} />
                </BarChart>
              </ResponsiveContainer>
            )}
          </div>

          {/* Chart 2: Student count per class (Pie) */}
          <div className="lg:col-span-5 bg-white rounded-xl border border-gray-200 p-5 shadow-sm">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-base font-bold text-gray-900 flex items-center gap-2">
                <Layers className="h-5 w-5 text-[#3B82F6]" /> Sĩ số sinh viên theo lớp
              </h2>
              <button onClick={() => navigate("/teacher/classes")} className="text-xs text-[#3B82F6] font-medium hover:underline">
                Xem tất cả
              </button>
            </div>
            {classes.length === 0 ? (
              <EmptyState title="Chưa có lớp học nào" description="Thêm lớp học mới để bắt đầu quản lý sinh viên." />
            ) : (
              <div className="flex items-center gap-6">
                <div className="relative w-[130px] h-[130px] flex-shrink-0">
                  <ResponsiveContainer width="100%" height="100%">
                    <PieChart>
                      <Pie
                        data={classPieData}
                        cx="50%"
                        cy="50%"
                        innerRadius={38}
                        outerRadius={58}
                        paddingAngle={2}
                        dataKey="value"
                        strokeWidth={0}
                      >
                        {classPieData.map((entry, index) => (
                          <Cell key={`cell-${index}`} fill={entry.color} />
                        ))}
                      </Pie>
                    </PieChart>
                  </ResponsiveContainer>
                  <div className="absolute inset-0 flex flex-col items-center justify-center text-center">
                    <span className="text-2xl font-bold text-gray-900">{totalStudents}</span>
                    <span className="text-[10px] text-gray-400 uppercase tracking-tight">Sinh viên</span>
                  </div>
                </div>
                <div className="space-y-2 flex-1 max-h-[140px] overflow-y-auto pr-1">
                  {classPieData.map((item) => (
                    <div key={item.name} className="flex items-center justify-between text-xs">
                      <div className="flex items-center gap-2 truncate">
                        <div className="w-2.5 h-2.5 rounded-full flex-shrink-0" style={{ backgroundColor: item.color }} />
                        <span className="text-gray-700 font-medium truncate">{item.name}</span>
                      </div>
                      <span className="font-bold text-gray-900 flex-shrink-0">{item.value} SV</span>
                    </div>
                  ))}
                </div>
              </div>
            )}
          </div>
        </div>

        {/* Management Analytics Cards */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {/* Top Classes */}
          <div className="bg-white rounded-xl border border-gray-200 p-5 shadow-sm">
            <h2 className="text-base font-bold text-gray-900 mb-3">Top lớp học đông nhất</h2>
            <div className="space-y-3">
              {topClasses.length === 0 ? (
                <div className="text-sm text-gray-400 text-center py-4">Chưa có lớp học nào</div>
              ) : (
                topClasses.map((cls) => {
                  const cnt = cls.current || cls.total || 0
                  const pct = totalStudents > 0 ? Math.round((cnt / totalStudents) * 100) : 0
                  return (
                    <div key={cls.id} className="space-y-1">
                      <div className="flex items-center justify-between text-xs">
                        <span className="font-semibold text-gray-900">{cls.id} - {cls.name}</span>
                        <span className="text-gray-500">{cnt} sinh viên ({pct}%)</span>
                      </div>
                      <div className="w-full bg-gray-100 rounded-full h-2 overflow-hidden">
                        <div className="bg-[#8B5CF6] h-full rounded-full transition-all" style={{ width: `${Math.min(pct * 2, 100)}%` }} />
                      </div>
                    </div>
                  )
                })
              )}
            </div>
          </div>

          {/* Top Subjects */}
          <div className="bg-[#FFFFFF] rounded-xl border border-gray-200 p-5 shadow-sm">
            <h2 className="text-base font-bold text-gray-900 mb-3">Top môn học có ngân hàng câu hỏi lớn nhất</h2>
            <div className="space-y-3">
              {topSubjects.length === 0 ? (
                <div className="text-sm text-gray-400 text-center py-4">Chưa có môn học nào</div>
              ) : (
                topSubjects.map((sub) => {
                  const qCount = sub.questionCount || 0
                  const pct = totalQuestions > 0 ? Math.round((qCount / totalQuestions) * 100) : 0
                  return (
                    <div key={sub.code} className="space-y-1">
                      <div className="flex items-center justify-between text-xs">
                        <span className="font-semibold text-gray-900">{sub.code} - {sub.name}</span>
                        <span className="text-gray-500">{qCount} câu hỏi</span>
                      </div>
                      <div className="w-full bg-gray-100 rounded-full h-2 overflow-hidden">
                        <div className="bg-[#10B981] h-full rounded-full transition-all" style={{ width: `${Math.min(pct * 3, 100)}%` }} />
                      </div>
                    </div>
                  )
                })
              )}
            </div>
          </div>
        </div>

        {/* Quick Actions */}
        <div>
          <h2 className="text-base font-bold text-gray-900 mb-4">Thao tác quản lý</h2>
          <div className="grid grid-cols-2 lg:grid-cols-4 gap-4">
            {quickActions.map((action) => {
              const Icon = action.icon
              return (
                <button
                  key={action.title}
                  onClick={() => navigate(action.path)}
                  className="bg-white rounded-xl border border-gray-200 p-4 flex items-center gap-3 hover:shadow-md hover:border-red-200 transition-all group text-left"
                >
                  <div className={`w-10 h-10 rounded-xl ${action.iconBg} flex items-center justify-center flex-shrink-0 group-hover:scale-105 transition-transform`}>
                    <Icon className={`h-5 w-5 ${action.iconColor}`} />
                  </div>
                  <div>
                    <div className="text-sm font-bold text-gray-900 group-hover:text-[#D9272B] transition-colors">{action.title}</div>
                    <div className="text-[11px] text-gray-500 mt-0.5">{action.subtitle}</div>
                  </div>
                </button>
              )
            })}
          </div>
        </div>
      </div>
    </TeacherLayout>
  )
}