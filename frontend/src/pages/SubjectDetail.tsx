import { useState, useEffect } from "react"
import { useParams } from "react-router-dom"
import {
  BookOpen,
  Pencil,
  Sparkles,
  FileText,
  Download,
  ChevronDown,
  Users,
  BarChart3,
  Star,
  User,
  Clock,
  Plus,
  Search,
  Eye,
  Trash2,
  Copy,
  Calendar,
  ArrowRight,
  Activity,
  FileSpreadsheet,
  Printer,
  Settings,
  Upload,
  FolderOpen,
  CheckSquare,
  AlertCircle,
} from "lucide-react"
import TeacherLayout from "@/components/layouts/TeacherLayout"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Badge } from "@/components/ui/badge"
import { Select } from "@/components/ui/select"
import {
  PieChart,
  Pie,
  Cell,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  LineChart,
  Line,
  LabelList,
} from "recharts"
import { subjectService } from "@/services/subjectService"
import { questionService } from "@/services/questionService"
import type { Subject } from "@/types"
import {
  subjectDifficultyData as difficultyData,
  subjectChapterData as chapterData,
  subjectClassPassRateData as classPassRateData,
  subjectExamPassRateData as examPassRateData,
  subjectScoreDistributionData as scoreDistributionData,
  subjectTrendData as trendData,
} from "@/data/mockCharts"

const subjectData = {
  id: "1",
  code: "CTDLGT",
  name: "Cấu trúc dữ liệu và giải thuật",
  department: "Công nghệ thông tin",
  teacher: "Đỗ Minh Triết",
  status: "active" as const,
  description: "Môn học cung cấp kiến thức nền tảng về cấu trúc dữ liệu cơ bản và các thuật toán để giải quyết các bài toán thực tế một cách hiệu quả.",
  bankDescription: "Ngân hàng câu hỏi của môn học chứa tất cả các câu hỏi được giảng viên tạo và quản lý. Bạn có thể thêm mới, chỉnh sửa, xóa hoặc phân loại câu hỏi theo chủ đề và mức độ.",
  createdAt: "10/02/2025 14:30",
  updatedAt: "19/06/2026 10:45",
  program: "Kỹ sư CNTT",
  semester: "2 (2025 - 2026)",
  credits: 3,
  totalQuestions: 156,
  newQuestions: 12,
  totalExams: 8,
  newExams: 1,
  totalClasses: 6,
  totalStudents: 120,
  totalAttempts: 342,
  avgPassRate: 78.6,
  passRateChange: 5.2,
}

const classes = [
  { code: "D24CQCE01-N", students: 45 },
  { code: "D24CQCE02-N", students: 42 },
  { code: "D24CQCE03-N", students: 40 },
  { code: "D24CQCE04-N", students: 38 },
  { code: "D24CQCE05-N", students: 36 },
  { code: "D24CQCE06-N", students: 28 },
]


// Question bank mock data
const questions = [
  { id: "Q156", content: "Độ phức tạp thời gian của thuật toán Binary Search là?", type: "Trắc nghiệm", difficulty: "Dễ", topic: "Thuật toán tìm kiếm", date: "20/06/2026" },
  { id: "Q155", content: "Cấu trúc dữ liệu nào hoạt động theo nguyên tắc LIFO?", type: "Trắc nghiệm", difficulty: "Dễ", topic: "Cấu trúc dữ liệu", date: "19/06/2026" },
  { id: "Q154", content: "Cho dãy: 5, 2, 8, 1, 9. Kết quả sau khi sắp xếp tăng dần là?", type: "Trắc nghiệm", difficulty: "Trung bình", topic: "Sắp xếp", date: "18/06/2026" },
  { id: "Q153", content: "Trong đồ thị vô hướng, bậc của một đỉnh là gì?", type: "Tự luận", difficulty: "Trung bình", topic: "Đồ thị", date: "17/06/2026" },
  { id: "Q152", content: "Cây nhị phân đầy đủ là cây mà:", type: "Trắc nghiệm", difficulty: "Khó", topic: "Cây", date: "16/06/2026" },
  { id: "Q151", content: "Phân tích độ phức tạp không gian của Quick Sort.", type: "Tự luận", difficulty: "Khó", topic: "Sắp xếp", date: "15/06/2026" },
  { id: "Q150", content: "Kết quả của phép duyệt DFS trên đồ thị bắt đầu từ đỉnh A là?", type: "Trắc nghiệm", difficulty: "Trung bình", topic: "Đồ thị", date: "14/06/2026" },
  { id: "Q149", content: "Trong Hash Table, va chạm xảy ra khi nào?", type: "Trắc nghiệm", difficulty: "Dễ", topic: "Bảng băm", date: "13/06/2026" },
]

// Exam mock data
const exams = [
  { id: "E08", name: "Thi giữa kỳ HK2 - 2024/2025 (Đề số 01)", duration: "90 phút", questions: 40, date: "10/02/2025 14:30", status: "active" as const },
  { id: "E07", name: "Thi giữa kỳ HK1 - 2024/2025 (Đề số 01)", duration: "90 phút", questions: 40, date: "15/10/2024 09:15", status: "ended" as const },
  { id: "E06", name: "Thi cuối kỳ HK2 - 2023/2024 (Đề số 02)", duration: "120 phút", questions: 50, date: "20/06/2024 10:00", status: "ended" as const },
  { id: "E05", name: "Thi cuối kỳ HK2 - 2023/2024 (Đề số 01)", duration: "120 phút", questions: 50, date: "18/06/2024 08:30", status: "ended" as const },
  { id: "E04", name: "Thi thực hành - Sorting Algorithms", duration: "60 phút", questions: 25, date: "05/05/2024 13:00", status: "inactive" as const },
  { id: "E03", name: "Kiểm tra 15 phút - Chương 4", duration: "15 phút", questions: 10, date: "25/04/2024 07:45", status: "ended" as const },
  { id: "E02", name: "Kiểm tra 15 phút - Chương 3", duration: "15 phút", questions: 10, date: "18/04/2024 07:45", status: "ended" as const },
  { id: "E01", name: "Kiểm tra 15 phút - Chương 2", duration: "15 phút", questions: 10, date: "11/04/2024 07:45", status: "ended" as const },
]


// Activity history mock data
const activities = [
  { time: "20/06/2026 16:20:35", user: "Đỗ Minh Triết", role: "Giảng viên", action: "Tạo câu hỏi mới", object: "Câu hỏi ID: Q156", detail: "Đề thêm câu hỏi \"Độ phức tạp thời gian của thuật toán Binary Search là?\"", icon: "plus" },
  { time: "20/06/2026 15:45:12", user: "Đỗ Minh Triết", role: "Giảng viên", action: "Cập nhật đề thi", object: "Đề thi ID: E08", detail: "Cập nhật thông tin và câu hỏi trong đề thi giữa kỳ HK2 - 2024/2025", icon: "edit" },
  { time: "20/06/2026 15:10:05", user: "Nguyễn Văn An", role: "Sinh viên", action: "Làm bài thi", object: "Đề thi ID: E08", detail: "Nộp bài thi sau 45 phút 32 giây", icon: "exam" },
  { time: "20/06/2026 14:32:41", user: "Đỗ Minh Triết", role: "Giảng viên", action: "Xóa câu hỏi", object: "Câu hỏi ID: Q148", detail: "Xóa câu hỏi \"Trong Quick Sort, phần tử pivot được chọn như thế nào?\"", icon: "trash" },
  { time: "20/06/2026 13:55:09", user: "Đỗ Minh Triết", role: "Giảng viên", action: "Tạo đề thi mới", object: "Đề thi ID: E09", detail: "Tạo đề thi cuối kỳ HK2 - 2024/2025", icon: "create" },
  { time: "20/06/2026 11:25:33", user: "Nguyễn Thị Mai", role: "Giảng viên", action: "Cập nhật môn học", object: "CTDLGT", detail: "Cập nhật mô tả môn học", icon: "edit" },
  { time: "19/06/2026 22:18:47", user: "Trần Minh Đức", role: "Sinh viên", action: "Xem kết quả thi", object: "Đề thi ID: E07", detail: "Xem kết quả bài thi giữa kỳ HK1 - 2024/2025", icon: "view" },
  { time: "19/06/2026 20:05:22", user: "Đỗ Minh Triết", role: "Giảng viên", action: "Xuất dữ liệu", object: "Ngân hàng câu hỏi", detail: "Xuất file Excel ngân hàng câu hỏi gồm 156 câu", icon: "export" },
]

const TABS = [
  { id: "overview", label: "Tổng quan", icon: User },
  { id: "questions", label: "Ngân hàng câu hỏi", icon: FolderOpen },
  { id: "exams", label: "Đề thi", icon: FileText },
  { id: "stats", label: "Thống kê", icon: BarChart3 },
  { id: "history", label: "Lịch sử hoạt động", icon: Clock },
]

export default function SubjectDetail() {
  const [activeTab, setActiveTab] = useState("overview")

  const breadcrumb = [
    { label: "Quản lý môn học", path: "/teacher/subjects" },
    { label: "Chi tiết môn học" },
  ]

  return (
    <TeacherLayout breadcrumb={breadcrumb}>
      <div className="space-y-6">
        {/* Subject Header */}
        <div className="flex items-start justify-between">
          <div className="flex items-start gap-4">
            <div className="w-16 h-16 rounded-xl bg-purple-100 flex items-center justify-center flex-shrink-0">
              <BookOpen className="h-8 w-8 text-purple-600" />
            </div>
            <div>
              <div className="flex items-center gap-3">
                <h1 className="text-2xl font-bold text-gray-900">{subjectData.name}</h1>
                <Badge className="bg-green-50 text-green-700 border border-green-200 hover:bg-green-50 font-medium text-xs px-2.5 py-0.5 rounded-full">
                  Đang sử dụng
                </Badge>
              </div>
              <p className="text-sm text-gray-500 mt-1">
                Mã môn học: {subjectData.code} &nbsp;•&nbsp; Khoa {subjectData.department}
              </p>
            </div>
          </div>
          <div className="flex items-center gap-2">
            <Button variant="outline" className="h-10 gap-2">
              <Pencil className="h-4 w-4" />
              Chỉnh sửa
            </Button>
            <Button variant="outline" className="h-10 gap-2 border-purple-200 text-purple-700 hover:bg-purple-50">
              <Sparkles className="h-4 w-4" />
              Quản lý câu hỏi
            </Button>
            <Button variant="outline" className="h-10 gap-2 border-green-200 text-green-700 hover:bg-green-50">
              <FileText className="h-4 w-4" />
              Tạo đề thi
            </Button>
            <Button variant="outline" className="h-10 gap-2">
              <Download className="h-4 w-4" />
              Xuất dữ liệu
              <ChevronDown className="h-4 w-4" />
            </Button>
          </div>
        </div>

        {/* Stat Cards */}
        <div className="grid grid-cols-5 gap-4">
          <StatCard
            icon={<CheckSquare className="h-6 w-6 text-purple-500" />}
            iconBg="bg-purple-50"
            label="Tổng số câu hỏi"
            value={subjectData.totalQuestions}
            sub={`+${subjectData.newQuestions} câu hỏi mới`}
            subColor="text-green-600"
          />
          <StatCard
            icon={<FileText className="h-6 w-6 text-green-500" />}
            iconBg="bg-green-50"
            label="Tổng số đề thi"
            value={subjectData.totalExams}
            sub={`+${subjectData.newExams} đề thi mới`}
            subColor="text-green-600"
          />
          <StatCard
            icon={<Users className="h-6 w-6 text-orange-500" />}
            iconBg="bg-orange-50"
            label="Số lớp đang học"
            value={subjectData.totalClasses}
            sub={`${subjectData.totalStudents} sinh viên`}
          />
          <StatCard
            icon={<BarChart3 className="h-6 w-6 text-blue-500" />}
            iconBg="bg-blue-50"
            label="Lượt sử dụng"
            value={subjectData.totalAttempts}
            sub="(Lượt làm bài)"
          />
          <StatCard
            icon={<Star className="h-6 w-6 text-red-500" />}
            iconBg="bg-red-50"
            label="Tỷ lệ đạt trung bình"
            value={`${subjectData.avgPassRate}%`}
            sub={`+${subjectData.passRateChange}% so với kỳ trước`}
            subColor="text-green-600"
          />
        </div>

        {/* Tabs */}
        <div className="border-b border-gray-200">
          <div className="flex gap-0">
            {TABS.map((tab) => {
              const Icon = tab.icon
              return (
                <button
                  key={tab.id}
                  onClick={() => setActiveTab(tab.id)}
                  className={`flex items-center gap-2 px-4 py-3 text-sm font-medium border-b-2 transition-colors ${
                    activeTab === tab.id
                      ? "border-[#D9272B] text-[#D9272B]"
                      : "border-transparent text-gray-500 hover:text-gray-700"
                  }`}
                >
                  <Icon className="h-4 w-4" />
                  {tab.label}
                </button>
              )
            })}
          </div>
        </div>

        {/* Tab Content + Right Sidebar */}
        <div className="flex gap-6">
          {/* Main Content */}
          <div className="flex-1 min-w-0">
            {activeTab === "overview" && <OverviewTab />}
            {activeTab === "questions" && <QuestionsTab />}
            {activeTab === "exams" && <ExamsTab />}
            {activeTab === "stats" && <StatsTab />}
            {activeTab === "history" && <HistoryTab />}
          </div>

          {/* Right Sidebar */}
          <div className="w-[320px] flex-shrink-0 space-y-4">
            <RightSidebar activeTab={activeTab} />
          </div>
        </div>
      </div>
    </TeacherLayout>
  )
}

// Stat Card Component
function StatCard({ icon, iconBg, label, value, sub, subColor }: {
  icon: React.ReactNode
  iconBg: string
  label: string
  value: string | number
  sub?: string
  subColor?: string
}) {
  return (
    <div className="bg-white rounded-xl border border-gray-200 p-4 flex items-center gap-3">
      <div className={`w-12 h-12 rounded-xl ${iconBg} flex items-center justify-center flex-shrink-0`}>
        {icon}
      </div>
      <div>
        <div className="text-xs text-gray-500">{label}</div>
        <div className="text-2xl font-bold text-gray-900">{value}</div>
        {sub && <div className={`text-xs ${subColor || "text-gray-400"}`}>{sub}</div>}
      </div>
    </div>
  )
}

// Overview Tab
function OverviewTab() {
  return (
    <div className="space-y-6">
      {/* Info Grid */}
      <div className="grid grid-cols-3 gap-6">
        {/* Thông tin chung */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <h3 className="text-base font-bold text-gray-900 mb-4">Thông tin chung</h3>
          <div className="space-y-3">
            <InfoRow label="Mã môn học" value={subjectData.code} />
            <InfoRow label="Tên môn học" value={subjectData.name} />
            <InfoRow label="Giảng viên phụ trách" value={subjectData.teacher} hasAvatar />
            <InfoRow label="Khoa" value={subjectData.department} />
            <div className="pt-2">
              <div className="text-xs text-gray-500 mb-1">Mô tả</div>
              <p className="text-sm text-gray-700 leading-relaxed">{subjectData.description}</p>
            </div>
          </div>
        </div>

        {/* Thời gian & trạng thái */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <h3 className="text-base font-bold text-gray-900 mb-4">Thời gian & trạng thái</h3>
          <div className="space-y-3">
            <InfoRow label="Ngày tạo" value={subjectData.createdAt} />
            <InfoRow label="Cập nhật gần nhất" value={subjectData.updatedAt} />
            <div className="flex items-center justify-between">
              <span className="text-sm text-gray-500">Trạng thái</span>
              <Badge className="bg-green-50 text-green-700 border border-green-200 hover:bg-green-50 font-medium text-xs px-2.5 py-0.5 rounded-full">
                Đang sử dụng
              </Badge>
            </div>
            <InfoRow label="Chương trình đào tạo" value={subjectData.program} />
            <InfoRow label="Học kỳ áp dụng" value={subjectData.semester} />
            <InfoRow label="Số tín chỉ" value={String(subjectData.credits)} />
          </div>
        </div>

        {/* Các lớp đang học */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <h3 className="text-base font-bold text-gray-900 mb-4">Các lớp đang học ({classes.length})</h3>
          <div className="space-y-2">
            {classes.map((cls) => (
              <div key={cls.code} className="flex items-center justify-between py-1.5">
                <span className="text-sm text-blue-600 hover:underline cursor-pointer">{cls.code}</span>
                <span className="text-sm text-gray-500">{cls.students} SV</span>
              </div>
            ))}
          </div>
          <button className="flex items-center gap-1 text-sm text-blue-600 hover:underline mt-3">
            Xem tất cả lớp học <ArrowRight className="h-3.5 w-3.5" />
          </button>
        </div>
      </div>

      {/* Charts */}
      <div className="grid grid-cols-3 gap-6">
        {/* Donut - Độ khó */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <h3 className="text-base font-bold text-gray-900 mb-4">Phân bố câu hỏi theo độ khó</h3>
          <div className="flex items-center gap-4">
            <div className="relative w-36 h-36">
              <ResponsiveContainer width="100%" height="100%">
                <PieChart>
                  <Pie
                    data={difficultyData}
                    cx="50%"
                    cy="50%"
                    innerRadius={40}
                    outerRadius={65}
                    dataKey="value"
                    strokeWidth={0}
                  >
                    {difficultyData.map((entry, index) => (
                      <Cell key={index} fill={entry.color} />
                    ))}
                  </Pie>
                </PieChart>
              </ResponsiveContainer>
              <div className="absolute inset-0 flex flex-col items-center justify-center">
                <span className="text-2xl font-bold text-gray-900">156</span>
                <span className="text-xs text-gray-500">Tổng số</span>
              </div>
            </div>
            <div className="space-y-2">
              {difficultyData.map((d) => (
                <div key={d.name} className="flex items-center gap-2">
                  <div className="w-2.5 h-2.5 rounded-full" style={{ backgroundColor: d.color }} />
                  <span className="text-sm text-gray-600">{d.name}</span>
                  <span className="text-sm text-gray-400 ml-1">{d.value} ({d.percent}%)</span>
                </div>
              ))}
            </div>
          </div>
        </div>

        {/* Bar - Theo chương */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <h3 className="text-base font-bold text-gray-900 mb-4">Số câu hỏi theo chương</h3>
          <ResponsiveContainer width="100%" height={200}>
            <BarChart data={chapterData}>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#F3F4F6" />
              <XAxis dataKey="name" tick={{ fontSize: 11 }} tickLine={false} axisLine={false} />
              <YAxis tick={{ fontSize: 11 }} tickLine={false} axisLine={false} />
              <Tooltip />
              <Bar dataKey="value" fill="#3B82F6" radius={[4, 4, 0, 0]} barSize={28}>
                <LabelList dataKey="value" position="top" fontSize={11} fill="#6B7280" />
              </Bar>
            </BarChart>
          </ResponsiveContainer>
          <button className="flex items-center gap-1 text-sm text-blue-600 hover:underline mt-2 justify-center">
            Xem chi tiết theo chương <ArrowRight className="h-3.5 w-3.5" />
          </button>
        </div>

        {/* Horizontal Bar - Tỷ lệ đúng theo lớp */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <h3 className="text-base font-bold text-gray-900 mb-4">Tỷ lệ đúng trung bình theo lớp</h3>
          <div className="space-y-3">
            {classPassRateData.map((cls) => (
              <div key={cls.name} className="space-y-1">
                <div className="flex items-center justify-between">
                  <span className="text-xs text-gray-600">{cls.name}</span>
                  <span className="text-xs font-medium text-gray-700">{cls.value}%</span>
                </div>
                <div className="w-full h-2 bg-gray-100 rounded-full overflow-hidden">
                  <div
                    className="h-full rounded-full transition-all"
                    style={{ width: `${cls.value}%`, backgroundColor: cls.color }}
                  />
                </div>
              </div>
            ))}
          </div>
          <button className="flex items-center gap-1 text-sm text-blue-600 hover:underline mt-3 justify-center">
            Xem tất cả <ArrowRight className="h-3.5 w-3.5" />
          </button>
        </div>
      </div>
    </div>
  )
}

// Questions Tab
function QuestionsTab() {
  const [searchTerm, setSearchTerm] = useState("")
  const [typeFilter, setTypeFilter] = useState("all")
  const [difficultyFilter, setDifficultyFilter] = useState("all")

  const filtered = questions.filter((q) => {
    const matchSearch = q.content.toLowerCase().includes(searchTerm.toLowerCase()) || q.id.toLowerCase().includes(searchTerm.toLowerCase())
    const matchType = typeFilter === "all" || q.type === typeFilter
    const matchDifficulty = difficultyFilter === "all" || q.difficulty === difficultyFilter
    return matchSearch && matchType && matchDifficulty
  })

  return (
    <div className="space-y-4">
      <h3 className="text-lg font-bold text-gray-900">Ngân hàng câu hỏi</h3>
      <div className="flex items-center gap-3">
        <div className="relative flex-1 max-w-md">
          <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
          <Input placeholder="Tìm kiếm câu hỏi..." className="pl-10 h-10" value={searchTerm} onChange={(e) => setSearchTerm(e.target.value)} />
        </div>
        <Select className="w-[160px] h-10" value={typeFilter} onChange={(e) => setTypeFilter(e.target.value)} options={[
          { value: "all", label: "Tất cả dạng câu hỏi" },
          { value: "Trắc nghiệm", label: "Trắc nghiệm" },
          { value: "Tự luận", label: "Tự luận" },
        ]} />
        <Select className="w-[140px] h-10" value={difficultyFilter} onChange={(e) => setDifficultyFilter(e.target.value)} options={[
          { value: "all", label: "Tất cả mức độ" },
          { value: "Dễ", label: "Dễ" },
          { value: "Trung bình", label: "Trung bình" },
          { value: "Khó", label: "Khó" },
        ]} />
        <div className="flex-1" />
        <Button className="h-10 gap-2 border border-blue-200 text-blue-700 hover:bg-blue-50 bg-white">
          <Plus className="h-4 w-4" />
          Thêm câu hỏi
        </Button>
      </div>

      <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
        <table className="w-full">
          <thead>
            <tr className="border-b border-gray-200 bg-gray-50/50">
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">ID</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Nội dung câu hỏi</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Dạng câu hỏi</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Mức độ</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Chủ đề</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Ngày tạo</th>
              <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">Thao tác</th>
            </tr>
          </thead>
          <tbody>
            {filtered.map((q) => (
              <tr key={q.id} className="border-b border-gray-100 hover:bg-gray-50/50">
                <td className="px-4 py-3 text-sm font-medium text-gray-900">{q.id}</td>
                <td className="px-4 py-3 text-sm text-gray-700 max-w-[300px] truncate">{q.content}</td>
                <td className="px-4 py-3">
                  <Badge className={`text-xs px-2 py-0.5 rounded-full font-medium ${
                    q.type === "Trắc nghiệm" ? "bg-blue-50 text-blue-700 border border-blue-200" : "bg-purple-50 text-purple-700 border border-purple-200"
                  }`}>{q.type}</Badge>
                </td>
                <td className="px-4 py-3">
                  <Badge className={`text-xs px-2 py-0.5 rounded-full font-medium ${
                    q.difficulty === "Dễ" ? "bg-green-50 text-green-700 border border-green-200" :
                    q.difficulty === "Trung bình" ? "bg-orange-50 text-orange-700 border border-orange-200" :
                    "bg-red-50 text-red-700 border border-red-200"
                  }`}>{q.difficulty}</Badge>
                </td>
                <td className="px-4 py-3 text-sm text-gray-600">{q.topic}</td>
                <td className="px-4 py-3 text-sm text-gray-500">{q.date}</td>
                <td className="px-4 py-3">
                  <div className="flex items-center justify-center gap-1">
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"><Eye className="h-4 w-4" /></button>
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"><Pencil className="h-4 w-4" /></button>
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-red-500"><Trash2 className="h-4 w-4" /></button>
                  </div>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
        <div className="flex items-center justify-between px-4 py-3 border-t border-gray-200 bg-gray-50/30">
          <div />
          <div className="flex items-center gap-1">
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100">{"<"}</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg bg-blue-600 text-white text-sm font-medium">1</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">2</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">3</button>
            <span className="px-2 text-gray-400">...</span>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">20</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100">{">"}</button>
          </div>
          <div className="text-sm text-gray-500">Hiển thị 1 - 8 trong tổng số 156 câu hỏi</div>
        </div>
      </div>
    </div>
  )
}

// Exams Tab
function ExamsTab() {
  const [searchTerm, setSearchTerm] = useState("")
  const [statusFilter, setStatusFilter] = useState("all")
  const [classFilter, setClassFilter] = useState("all")

  const filtered = exams.filter((e) => {
    const matchSearch = e.name.toLowerCase().includes(searchTerm.toLowerCase()) || e.id.toLowerCase().includes(searchTerm.toLowerCase())
    const matchStatus = statusFilter === "all" || e.status === statusFilter
    return matchSearch && matchStatus
  })

  return (
    <div className="space-y-4">
      <h3 className="text-lg font-bold text-gray-900">Danh sách đề thi</h3>
      <div className="flex items-center gap-3">
        <div className="relative flex-1 max-w-md">
          <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
          <Input placeholder="Tìm kiếm đề thi..." className="pl-10 h-10" value={searchTerm} onChange={(e) => setSearchTerm(e.target.value)} />
        </div>
        <Select className="w-[160px] h-10" value={statusFilter} onChange={(e) => setStatusFilter(e.target.value)} options={[
          { value: "all", label: "Tất cả trạng thái" },
          { value: "active", label: "Đang sử dụng" },
          { value: "ended", label: "Đã kết thúc" },
          { value: "inactive", label: "Ngừng sử dụng" },
        ]} />
        <Select className="w-[160px] h-10" value={classFilter} onChange={(e) => setClassFilter(e.target.value)} options={[
          { value: "all", label: "Tất cả lớp học" },
          ...classes.map((c) => ({ value: c.code, label: c.code })),
        ]} />
        <div className="flex-1" />
        <Button className="h-10 gap-2 border border-blue-200 text-blue-700 hover:bg-blue-50 bg-white">
          <Plus className="h-4 w-4" />
          Tạo đề thi mới
        </Button>
      </div>

      <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
        <table className="w-full">
          <thead>
            <tr className="border-b border-gray-200 bg-gray-50/50">
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">ID</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Tên đề thi</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Môn học</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Thời lượng</th>
              <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">Số câu hỏi</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Ngày tạo</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Trạng thái</th>
              <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">Thao tác</th>
            </tr>
          </thead>
          <tbody>
            {filtered.map((exam) => (
              <tr key={exam.id} className="border-b border-gray-100 hover:bg-gray-50/50">
                <td className="px-4 py-3 text-sm font-medium text-gray-900">{exam.id}</td>
                <td className="px-4 py-3 text-sm text-gray-700 max-w-[250px]">{exam.name}</td>
                <td className="px-4 py-3 text-sm text-gray-600">{subjectData.name}</td>
                <td className="px-4 py-3 text-sm text-gray-600 flex items-center gap-1"><Clock className="h-3.5 w-3.5" /> {exam.duration}</td>
                <td className="px-4 py-3 text-sm text-gray-700 text-center">{exam.questions}</td>
                <td className="px-4 py-3 text-sm text-gray-500">{exam.date}</td>
                <td className="px-4 py-3">
                  <Badge className={`text-xs px-2 py-0.5 rounded-full font-medium ${
                    exam.status === "active" ? "bg-green-50 text-green-700 border border-green-200" :
                    exam.status === "ended" ? "bg-blue-50 text-blue-700 border border-blue-200" :
                    "bg-orange-50 text-orange-700 border border-orange-200"
                  }`}>
                    {exam.status === "active" ? "Đang sử dụng" : exam.status === "ended" ? "Đã kết thúc" : "Ngừng sử dụng"}
                  </Badge>
                </td>
                <td className="px-4 py-3">
                  <div className="flex items-center justify-center gap-1">
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"><Eye className="h-4 w-4" /></button>
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"><Pencil className="h-4 w-4" /></button>
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"><Copy className="h-4 w-4" /></button>
                    <button className="p-1.5 rounded-lg hover:bg-gray-100 text-red-500"><Trash2 className="h-4 w-4" /></button>
                  </div>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
        <div className="flex items-center justify-between px-4 py-3 border-t border-gray-200 bg-gray-50/30">
          <div />
          <div className="flex items-center gap-1">
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100">{"<"}</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg bg-blue-600 text-white text-sm font-medium">1</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">2</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100">{">"}</button>
          </div>
          <div className="text-sm text-gray-500">Hiển thị 1 - 8 trong tổng số 8 đề thi</div>
        </div>
      </div>
    </div>
  )
}

// Stats Tab
function StatsTab() {
  return (
    <div className="space-y-6">
      <div className="grid grid-cols-2 gap-6">
        {/* Tỷ lệ đúng theo đề thi */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <div className="flex items-center justify-between mb-4">
            <h3 className="text-base font-bold text-gray-900">Tỷ lệ đúng trung bình theo đề thi</h3>
            <button className="text-sm text-blue-600 hover:underline flex items-center gap-1">Xem chi tiết <ArrowRight className="h-3.5 w-3.5" /></button>
          </div>
          <ResponsiveContainer width="100%" height={220}>
            <BarChart data={examPassRateData}>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#F3F4F6" />
              <XAxis dataKey="name" tick={{ fontSize: 10 }} tickLine={false} axisLine={false} />
              <YAxis tick={{ fontSize: 11 }} tickLine={false} axisLine={false} domain={[0, 100]} tickFormatter={(v) => `${v}%`} />
              <Tooltip formatter={(value) => `${value}%`} />
              <Bar dataKey="value" fill="#93C5FD" radius={[4, 4, 0, 0]} barSize={32}>
                <LabelList dataKey="value" position="top" fontSize={10} fill="#6B7280" formatter={(v) => `${v}%`} />
              </Bar>
            </BarChart>
          </ResponsiveContainer>
        </div>

        {/* Phân bố điểm số */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <div className="flex items-center justify-between mb-4">
            <h3 className="text-base font-bold text-gray-900">Phân bố điểm số</h3>
            <button className="text-sm text-blue-600 hover:underline flex items-center gap-1">Xem chi tiết <ArrowRight className="h-3.5 w-3.5" /></button>
          </div>
          <div className="flex items-center gap-4">
            <div className="relative w-40 h-40">
              <ResponsiveContainer width="100%" height="100%">
                <PieChart>
                  <Pie data={scoreDistributionData} cx="50%" cy="50%" innerRadius={45} outerRadius={70} dataKey="value" strokeWidth={0}>
                    {scoreDistributionData.map((entry, index) => (
                      <Cell key={index} fill={entry.color} />
                    ))}
                  </Pie>
                </PieChart>
              </ResponsiveContainer>
              <div className="absolute inset-0 flex flex-col items-center justify-center">
                <span className="text-2xl font-bold text-gray-900">342</span>
                <span className="text-xs text-gray-500">Lượt làm bài</span>
              </div>
            </div>
            <div className="space-y-2">
              {scoreDistributionData.map((d) => (
                <div key={d.name} className="flex items-center gap-2">
                  <div className="w-2.5 h-2.5 rounded-full" style={{ backgroundColor: d.color }} />
                  <span className="text-sm text-gray-600">{d.name}</span>
                  <span className="text-sm text-gray-400 ml-1">{d.value} ({d.percent}%)</span>
                </div>
              ))}
            </div>
          </div>
        </div>
      </div>

      <div className="grid grid-cols-2 gap-6">
        {/* Xu hướng điểm */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <div className="flex items-center justify-between mb-4">
            <h3 className="text-base font-bold text-gray-900">Xu hướng điểm trung bình theo thời gian</h3>
            <button className="text-sm text-blue-600 hover:underline flex items-center gap-1">Xem chi tiết <ArrowRight className="h-3.5 w-3.5" /></button>
          </div>
          <ResponsiveContainer width="100%" height={220}>
            <LineChart data={trendData}>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#F3F4F6" />
              <XAxis dataKey="month" tick={{ fontSize: 10 }} tickLine={false} axisLine={false} />
              <YAxis tick={{ fontSize: 11 }} tickLine={false} axisLine={false} domain={[0, 10]} />
              <Tooltip />
              <Line type="monotone" dataKey="value" stroke="#3B82F6" strokeWidth={2} dot={{ fill: "#3B82F6", r: 4 }}>
                <LabelList dataKey="value" position="top" fontSize={10} fill="#6B7280" />
              </Line>
            </LineChart>
          </ResponsiveContainer>
        </div>

        {/* Thống kê theo mức độ */}
        <div className="bg-white rounded-xl border border-gray-200 p-5">
          <div className="flex items-center justify-between mb-4">
            <h3 className="text-base font-bold text-gray-900">Thống kê theo mức độ câu hỏi</h3>
            <button className="text-sm text-blue-600 hover:underline flex items-center gap-1">Xem chi tiết <ArrowRight className="h-3.5 w-3.5" /></button>
          </div>
          <div className="flex items-center gap-4">
            <div className="relative w-40 h-40">
              <ResponsiveContainer width="100%" height="100%">
                <PieChart>
                  <Pie data={difficultyData} cx="50%" cy="50%" innerRadius={45} outerRadius={70} dataKey="value" strokeWidth={0}>
                    {difficultyData.map((entry, index) => (
                      <Cell key={index} fill={entry.color} />
                    ))}
                  </Pie>
                </PieChart>
              </ResponsiveContainer>
              <div className="absolute inset-0 flex flex-col items-center justify-center">
                <span className="text-2xl font-bold text-gray-900">156</span>
                <span className="text-xs text-gray-500">Tổng số câu hỏi</span>
              </div>
            </div>
            <div className="space-y-2">
              {difficultyData.map((d) => (
                <div key={d.name} className="flex items-center gap-2">
                  <div className="w-2.5 h-2.5 rounded-full" style={{ backgroundColor: d.color }} />
                  <span className="text-sm text-gray-600">{d.name}</span>
                  <span className="text-sm text-gray-400 ml-1">{d.value} ({d.percent}%)</span>
                </div>
              ))}
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}

// History Tab
function HistoryTab() {
  const [searchTerm, setSearchTerm] = useState("")
  const [userFilter, setUserFilter] = useState("all")
  const [actionFilter, setActionFilter] = useState("all")

  const getActionIcon = (icon: string) => {
    switch (icon) {
      case "plus": return <Plus className="h-4 w-4 text-green-600" />
      case "edit": return <Pencil className="h-4 w-4 text-orange-600" />
      case "trash": return <Trash2 className="h-4 w-4 text-red-600" />
      case "exam": return <FileText className="h-4 w-4 text-green-600" />
      case "create": return <FileText className="h-4 w-4 text-green-600" />
      case "view": return <BarChart3 className="h-4 w-4 text-blue-600" />
      case "export": return <Download className="h-4 w-4 text-blue-600" />
      default: return <Activity className="h-4 w-4 text-gray-600" />
    }
  }

  return (
    <div className="space-y-4">
      <h3 className="text-lg font-bold text-gray-900">Lịch sử hoạt động</h3>
      <div className="flex items-center gap-3">
        <div className="relative flex-1 max-w-md">
          <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
          <Input placeholder="Tìm kiếm hoạt động..." className="pl-10 h-10" value={searchTerm} onChange={(e) => setSearchTerm(e.target.value)} />
        </div>
        <Select className="w-[160px] h-10" value={userFilter} onChange={(e) => setUserFilter(e.target.value)} options={[
          { value: "all", label: "Tất cả người dùng" },
          { value: "teacher", label: "Giảng viên" },
          { value: "student", label: "Sinh viên" },
        ]} />
        <Select className="w-[160px] h-10" value={actionFilter} onChange={(e) => setActionFilter(e.target.value)} options={[
          { value: "all", label: "Tất cả hành động" },
          { value: "create", label: "Tạo mới" },
          { value: "update", label: "Cập nhật" },
          { value: "delete", label: "Xóa" },
        ]} />
        <div className="relative">
          <Calendar className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
          <Input className="pl-10 h-10 w-[200px]" placeholder="01/06/2026 - 20/06/2026" />
        </div>
        <div className="flex-1" />
        <Button variant="outline" className="h-10 gap-2">
          <Download className="h-4 w-4" />
          Xuất lịch sử
        </Button>
      </div>

      <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
        <table className="w-full">
          <thead>
            <tr className="border-b border-gray-200 bg-gray-50/50">
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Thời gian</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Người dùng</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Hành động</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Đối tượng</th>
              <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Chi tiết</th>
              <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">Thao tác</th>
            </tr>
          </thead>
          <tbody>
            {activities.map((act, i) => (
              <tr key={i} className="border-b border-gray-100 hover:bg-gray-50/50">
                <td className="px-4 py-3 text-sm text-gray-500 whitespace-nowrap">{act.time}</td>
                <td className="px-4 py-3">
                  <div className="flex items-center gap-2">
                    <div className="w-8 h-8 rounded-full bg-gray-200 flex items-center justify-center">
                      <User className="h-4 w-4 text-gray-500" />
                    </div>
                    <div>
                      <div className="text-sm font-medium text-gray-900">{act.user}</div>
                      <Badge className={`text-[10px] px-1.5 py-0 rounded-full font-medium ${
                        act.role === "Giảng viên" ? "bg-green-50 text-green-700" : "bg-blue-50 text-blue-700"
                      }`}>{act.role}</Badge>
                    </div>
                  </div>
                </td>
                <td className="px-4 py-3">
                  <div className="flex items-center gap-2">
                    {getActionIcon(act.icon)}
                    <span className="text-sm text-gray-700">{act.action}</span>
                  </div>
                </td>
                <td className="px-4 py-3 text-sm text-gray-600">{act.object}</td>
                <td className="px-4 py-3 text-sm text-gray-500 max-w-[250px] truncate">{act.detail}</td>
                <td className="px-4 py-3 text-center">
                  <button className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"><Eye className="h-4 w-4" /></button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
        <div className="flex items-center justify-between px-4 py-3 border-t border-gray-200 bg-gray-50/30">
          <div />
          <div className="flex items-center gap-1">
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100">{"<"}</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg bg-blue-600 text-white text-sm font-medium">1</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">2</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">3</button>
            <span className="px-2 text-gray-400">...</span>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-700 hover:bg-gray-100 text-sm">15</button>
            <button className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100">{">"}</button>
          </div>
          <div className="text-sm text-gray-500">Hiển thị 1 - 8 trong tổng số 112 hoạt động</div>
        </div>
      </div>
    </div>
  )
}

// Right Sidebar
function RightSidebar({ activeTab }: { activeTab: string }) {
  const getQuickActions = () => {
    switch (activeTab) {
      case "overview":
        return [
          { icon: <Plus className="h-4 w-4" />, label: "Thêm câu hỏi mới" },
          { icon: <Upload className="h-4 w-4" />, label: "Nhập câu hỏi từ Excel" },
          { icon: <FileText className="h-4 w-4" />, label: "Tạo đề thi mới" },
          { icon: <FolderOpen className="h-4 w-4" />, label: "Xem ngân hàng câu hỏi" },
          { icon: <Download className="h-4 w-4" />, label: "Xuất ngân hàng câu hỏi" },
        ]
      case "questions":
        return [
          { icon: <Plus className="h-4 w-4" />, label: "Thêm câu hỏi mới" },
          { icon: <Upload className="h-4 w-4" />, label: "Nhập câu hỏi từ Excel" },
          { icon: <FolderOpen className="h-4 w-4" />, label: "Phân loại câu hỏi" },
          { icon: <CheckSquare className="h-4 w-4" />, label: "Kiểm tra câu hỏi trùng lặp" },
          { icon: <Download className="h-4 w-4" />, label: "Xuất ngân hàng câu hỏi" },
        ]
      case "exams":
        return [
          { icon: <FileText className="h-4 w-4" />, label: "Tạo đề thi mới" },
          { icon: <Upload className="h-4 w-4" />, label: "Nhập đề thi từ file" },
          { icon: <Copy className="h-4 w-4" />, label: "Sao chép đề thi" },
          { icon: <Users className="h-4 w-4" />, label: "Phân công đề thi cho lớp" },
          { icon: <Download className="h-4 w-4" />, label: "Xuất danh sách đề thi" },
        ]
      case "stats":
        return [
          { icon: <BarChart3 className="h-4 w-4" />, label: "Xem báo cáo chi tiết" },
          { icon: <FileSpreadsheet className="h-4 w-4" />, label: "Xuất thống kê ra Excel" },
          { icon: <Printer className="h-4 w-4" />, label: "In báo cáo thống kê" },
          { icon: <Users className="h-4 w-4" />, label: "So sánh giữa các lớp" },
          { icon: <Settings className="h-4 w-4" />, label: "Thiết lập mục tiêu điểm" },
        ]
      case "history":
        return [
          { icon: <Upload className="h-4 w-4" />, label: "Nhập câu hỏi từ Excel" },
          { icon: <Upload className="h-4 w-4" />, label: "Nhập đề thi từ file" },
          { icon: <Plus className="h-4 w-4" />, label: "Tạo câu hỏi mới" },
          { icon: <FileText className="h-4 w-4" />, label: "Tạo đề thi mới" },
          { icon: <FolderOpen className="h-4 w-4" />, label: "Xem ngân hàng câu hỏi" },
          { icon: <Download className="h-4 w-4" />, label: "Xuất dữ liệu môn học" },
        ]
      default:
        return []
    }
  }

  const getRecentActivities = () => {
    switch (activeTab) {
      case "overview":
        return [
          { icon: <Plus className="h-4 w-4 text-white" />, iconBg: "bg-green-500", label: "Thêm 5 câu hỏi mới", user: "Đỗ Minh Triết", time: "19/06/2026 10:45" },
          { icon: <Pencil className="h-4 w-4 text-white" />, iconBg: "bg-orange-500", label: "Cập nhật câu hỏi ID: 1023", user: "Đỗ Minh Triết", time: "18/06/2026 15:20" },
          { icon: <FileText className="h-4 w-4 text-white" />, iconBg: "bg-purple-500", label: "Tạo đề thi \"CTDLGT - Đề 2\"", user: "Đỗ Minh Triết", time: "17/06/2026 09:15" },
          { icon: <Upload className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Nhập 20 câu hỏi từ Excel", user: "Đỗ Minh Triết", time: "16/06/2026 11:05" },
          { icon: <Trash2 className="h-4 w-4 text-white" />, iconBg: "bg-red-500", label: "Xóa câu hỏi ID: 0891", user: "Đỗ Minh Triết", time: "15/06/2026 14:30" },
        ]
      case "questions":
        return [
          { icon: <Plus className="h-4 w-4 text-white" />, iconBg: "bg-green-500", label: "Thêm 5 câu hỏi mới", user: "Đỗ Minh Triết", time: "19/06/2026 10:45" },
          { icon: <Pencil className="h-4 w-4 text-white" />, iconBg: "bg-orange-500", label: "Cập nhật câu hỏi ID: Q123", user: "Đỗ Minh Triết", time: "18/06/2026 15:20" },
          { icon: <Upload className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Nhập 20 câu hỏi từ Excel", user: "Đỗ Minh Triết", time: "17/06/2026 11:05" },
          { icon: <Trash2 className="h-4 w-4 text-white" />, iconBg: "bg-red-500", label: "Xóa câu hỏi ID: Q089", user: "Đỗ Minh Triết", time: "15/06/2026 14:30" },
          { icon: <Download className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Xuất ngân hàng câu hỏi", user: "Đỗ Minh Triết", time: "14/06/2026 09:15" },
        ]
      case "exams":
        return [
          { icon: <FileText className="h-4 w-4 text-white" />, iconBg: "bg-green-500", label: "Tạo đề thi E08", user: "Đỗ Minh Triết", time: "10/02/2025 14:30" },
          { icon: <Pencil className="h-4 w-4 text-white" />, iconBg: "bg-orange-500", label: "Cập nhật đề thi E07", user: "Đỗ Minh Triết", time: "15/10/2024 09:15" },
          { icon: <Copy className="h-4 w-4 text-white" />, iconBg: "bg-purple-500", label: "Sao chép đề thi E06", user: "Đỗ Minh Triết", time: "20/06/2024 10:00" },
          { icon: <Users className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Phân công đề thi E05 cho lớp D24CQCE01-N", user: "Đỗ Minh Triết", time: "18/06/2024 08:45" },
          { icon: <AlertCircle className="h-4 w-4 text-white" />, iconBg: "bg-red-500", label: "Ngừng sử dụng đề thi E04", user: "Đỗ Minh Triết", time: "05/05/2024 13:30" },
        ]
      case "stats":
        return [
          { icon: <FileSpreadsheet className="h-4 w-4 text-white" />, iconBg: "bg-green-500", label: "Xuất thống kê tháng 06/2026", user: "Đỗ Minh Triết", time: "20/06/2026 16:20" },
          { icon: <BarChart3 className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Xem báo cáo đề thi E08", user: "Đỗ Minh Triết", time: "20/06/2026 15:45" },
          { icon: <Users className="h-4 w-4 text-white" />, iconBg: "bg-purple-500", label: "So sánh kết quả giữa 2 lớp", user: "Đỗ Minh Triết", time: "19/06/2026 14:30" },
          { icon: <Printer className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "In báo cáo tổng kết học kỳ", user: "Đỗ Minh Triết", time: "18/06/2026 10:15" },
          { icon: <Settings className="h-4 w-4 text-white" />, iconBg: "bg-red-500", label: "Thiết lập mục tiêu điểm trung bình", user: "Đỗ Minh Triết", time: "17/06/2026 09:30" },
        ]
      case "history":
        return [
          { icon: <Plus className="h-4 w-4 text-white" />, iconBg: "bg-green-500", label: "Thêm câu hỏi Q156", user: "Đỗ Minh Triết", time: "20/06/2026 16:20" },
          { icon: <Pencil className="h-4 w-4 text-white" />, iconBg: "bg-orange-500", label: "Cập nhật đề thi E08", user: "Đỗ Minh Triết", time: "20/06/2026 15:45" },
          { icon: <FileText className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Sinh viên Nguyễn Văn An làm bài", user: "Đề thi E08", time: "20/06/2026 15:10" },
          { icon: <Trash2 className="h-4 w-4 text-white" />, iconBg: "bg-red-500", label: "Xóa câu hỏi Q148", user: "Đỗ Minh Triết", time: "20/06/2026 14:32" },
          { icon: <Download className="h-4 w-4 text-white" />, iconBg: "bg-blue-500", label: "Xuất ngân hàng câu hỏi", user: "Đỗ Minh Triết", time: "20/06/2026 11:25" },
        ]
      default:
        return []
    }
  }

  return (
    <>
      {/* Info Card */}
      <div className="bg-white rounded-xl border border-gray-200 p-5">
        <h3 className="text-base font-bold text-gray-900 mb-3">
          {activeTab === "overview" ? "Mô tả ngân hàng" :
           activeTab === "questions" ? "Ngân hàng câu hỏi" :
           activeTab === "exams" ? "Thông tin môn học" :
           activeTab === "stats" ? "Thông tin môn học" :
           "Thông tin môn học"}
        </h3>
        <p className="text-sm text-gray-600 leading-relaxed">
          {activeTab === "questions"
            ? "Ngân hàng câu hỏi của môn học chứa tất cả các câu hỏi được giảng viên tạo và quản lý. Bạn có thể thêm mới, chỉnh sửa, xóa hoặc phân loại câu hỏi theo chủ đề và mức độ."
            : subjectData.description + " Môn học cung cấp các cấu trúc dữ liệu cơ bản và các thuật toán để giải quyết các bài toán thực tế một cách hiệu quả."}
        </p>
      </div>

      {/* Quick Actions */}
      <div className="bg-white rounded-xl border border-gray-200 p-5">
        <h3 className="text-base font-bold text-gray-900 mb-3">Quick Actions</h3>
        <div className="space-y-1">
          {getQuickActions().map((action, i) => (
            <button key={i} className="w-full flex items-center justify-between py-2 px-2 rounded-lg hover:bg-gray-50 transition-colors group">
              <div className="flex items-center gap-2.5">
                <span className="text-gray-500 group-hover:text-gray-700">{action.icon}</span>
                <span className="text-sm text-gray-700">{action.label}</span>
              </div>
              <ArrowRight className="h-3.5 w-3.5 text-gray-400" />
            </button>
          ))}
        </div>
      </div>

      {/* Recent Activity */}
      <div className="bg-white rounded-xl border border-gray-200 p-5">
        <div className="flex items-center justify-between mb-3">
          <h3 className="text-base font-bold text-gray-900">Hoạt động gần đây</h3>
          <button className="text-sm text-blue-600 hover:underline">Xem tất cả</button>
        </div>
        <div className="space-y-3">
          {getRecentActivities().map((act, i) => (
            <div key={i} className="flex items-start gap-3">
              <div className={`w-7 h-7 rounded-full ${act.iconBg} flex items-center justify-center flex-shrink-0 mt-0.5`}>
                {act.icon}
              </div>
              <div className="flex-1 min-w-0">
                <div className="text-sm font-medium text-gray-900">{act.label}</div>
                <div className="text-xs text-gray-500">{act.user} &nbsp;•&nbsp; {act.time}</div>
              </div>
            </div>
          ))}
        </div>
        <button className="flex items-center gap-1 text-sm text-blue-600 hover:underline mt-3">
          Xem tất cả hoạt động <ArrowRight className="h-3.5 w-3.5" />
        </button>
      </div>
    </>
  )
}

// Info Row Component
function InfoRow({ label, value, hasAvatar }: { label: string; value: string; hasAvatar?: boolean }) {
  return (
    <div className="flex items-center justify-between">
      <span className="text-sm text-gray-500">{label}</span>
      {hasAvatar ? (
        <div className="flex items-center gap-2">
          <div className="w-6 h-6 rounded-full bg-gray-200 flex items-center justify-center">
            <User className="h-3.5 w-3.5 text-gray-500" />
          </div>
          <span className="text-sm text-gray-900 font-medium">{value}</span>
        </div>
      ) : (
        <span className="text-sm text-gray-900 font-medium">{value}</span>
      )}
    </div>
  )
}