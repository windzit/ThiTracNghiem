import TeacherLayout from "@/widgets/layouts/TeacherLayout"
import { useState, useEffect } from "react"
import { useParams, useNavigate } from "react-router-dom"
import {
  User,
  FileText,
  Edit3,
  BookOpen,
  ArrowLeft,
  GraduationCap,
  Award,
  TrendingUp,
  TrendingDown,
  Mail,
  Trash2,
} from "lucide-react"
import { studentService } from "@/entities/student/studentService"
import { classService } from "@/entities/class/classService"
import { reportService } from "@/entities/report/reportService"
import type { Student, ClassItem } from "@/shared/types"
import { Button } from "@/shared/ui/button"
import { Input } from "@/shared/ui/input"
import { Label } from "@/shared/ui/label"
import { Drawer, EmptyState } from "@/shared/components"
import { validateStudentName, splitStudentName, formatStudentNameFields, normalizeText } from "@/shared/lib/formValidation"
import { useToast } from "@/app/providers/ToastContext"
import { ApiErrorHandler } from "@/shared/api/ApiErrorHandler"

export default function StudentDetail() {

  const { id } = useParams<{ id: string }>()
  const navigate = useNavigate()
  const { showSuccess, showError, confirm } = useToast()
  const [student, setStudent] = useState<Student | null>(null)
  const [classData, setClassData] = useState<ClassItem | null>(null)
  const [loading, setLoading] = useState(true)
  const [scores, setScores] = useState<Array<{ subjectCode: string; subjectName: string; score: number }>>([])
  
  // Edit Drawer state
  const [showEditDrawer, setShowEditDrawer] = useState(false)
  const [formHo, setFormHo] = useState("")
  const [formTen, setFormTen] = useState("")
  const [formPassword, setFormPassword] = useState("")
  const [isSubmitting, setIsSubmitting] = useState(false)
  const [errors, setErrors] = useState<{ ho?: string; ten?: string; name?: string }>({})

  const fetchStudentData = async () => {
    if (!id) return
    setLoading(true)
    try {
      const studentData = await studentService.getById(id)
      if (studentData) {
        setStudent(studentData)
        // Split name for edit form
        const parts = studentData.name.trim().split(" ")
        const ten = parts.pop() || ""
        const ho = parts.join(" ")
        setFormHo(ho)
        setFormTen(ten)

        if (studentData.scores && studentData.scores.length > 0) {
          setScores(studentData.scores)
        }

        // Lazy execution for Class and Scoreboard requests
        if (studentData.classCode) {
          const clsPromise = classService.getById(studentData.classCode).catch(() => null)
          const scoreboardPromise = (!studentData.scores || studentData.scores.length === 0) && (studentData.examCount || 0) > 0
            ? reportService.getScoreboard(studentData.classCode).catch(() => null)
            : Promise.resolve(null)

          const [clsResult, scoreboardResult] = await Promise.all([clsPromise, scoreboardPromise])

          if (clsResult) setClassData(clsResult)

          if (scoreboardResult && scoreboardResult.students && (!studentData.scores || studentData.scores.length === 0)) {
            const studentScoreRow = scoreboardResult.students.find((s) => s.masv === id)
            if (studentScoreRow) {
              const scoreList: Array<{ subjectCode: string; subjectName: string; score: number }> = []
              Object.entries(studentScoreRow.scores).forEach(([subCode, val]) => {
                if (val !== null && val !== undefined) {
                  const subObj = Array.isArray((scoreboardResult as any).subjects)
                    ? (scoreboardResult as any).subjects.find((sb: any) => sb.mamh === subCode)
                    : null
                  scoreList.push({
                    subjectCode: subCode,
                    subjectName: subObj ? subObj.tenmh : subCode,
                    score: typeof val === "number" ? val : parseFloat(val),
                  })
                }
              })
              setScores(scoreList)
            }
          }
        }
      }
    } catch (err) {
      console.error("[StudentDetail] Error fetching student data:", err)
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    fetchStudentData()
  }, [id])

  const handleDeleteScore = (subjectCode: string, subjectName: string) => {
    if (!student || !id) return

    confirm({
      title: "Xóa kết quả thi",
      message: `Bạn có chắc muốn xóa kết quả thi môn: ${subjectCode} - ${subjectName}?

Sau khi xác nhận:
• Điểm thi sẽ bị xóa.
• Bài thi sẽ bị loại khỏi lịch sử.
• Nếu đây là bài thi cuối cùng sử dụng một số câu hỏi, hệ thống sẽ tự động cập nhật lại trạng thái sử dụng của các câu hỏi đó.

Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: "Xóa",
      cancelText: "Hủy",
      onConfirm: async () => {
        try {
          await reportService.deleteScore(id, subjectCode, student.classCode)
          showSuccess("Xóa kết quả thi thành công", `Môn: ${subjectCode} - ${subjectName}`)
          await fetchStudentData()
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa kết quả thi", parsed.message || "Đã xảy ra lỗi khi xóa điểm thi", parsed.code)
        }
      },
    })
  }

  const handleOpenEdit = () => {
    if (!student) return
    let ho = student.ho || ""
    let ten = student.ten || ""
    if (!ho && !ten && student.name) {
      const split = splitStudentName("", student.name)
      ho = split.ho
      ten = split.ten
    }
    setFormHo(ho)
    setFormTen(ten)
    setFormPassword("")
    setErrors({})
    setShowEditDrawer(true)
  }

  const handleSaveStudent = async () => {
    if (!student || !id) return
    const rawHo = normalizeText(formHo)
    const rawTen = normalizeText(formTen)
    const { ho: formattedHo, ten: formattedTen } = formatStudentNameFields(rawHo, rawTen)

    const hoErr = validateStudentName(formattedHo)
    const tenErr = validateStudentName(formattedTen)
    if (hoErr || tenErr) {
      setErrors({
        ho: hoErr || undefined,
        ten: tenErr || undefined,
        name: hoErr || tenErr || undefined,
      })
      return
    }

    setIsSubmitting(true)
    try {
      await studentService.updateStudent(id, {
        ho: formattedHo,
        ten: formattedTen,
        name: `${formattedHo} ${formattedTen}`.trim(),
        password: formPassword ? formPassword : undefined,
      })
      setShowEditDrawer(false)
      await fetchStudentData()
      showSuccess("Cập nhật thông tin sinh viên thành công", `Mã SV: ${id}`)
    } catch (err: any) {
      console.error("[StudentDetail] Update error:", err)
      const parsed = ApiErrorHandler.handle(err)
      showError("Không thể cập nhật thông tin sinh viên", parsed.message, parsed.code)
    } finally {
      setIsSubmitting(false)
    }
  }


  if (loading) {
    return (
      <TeacherLayout>
        <div className="p-8 text-center text-gray-500">Đang tải thông tin sinh viên...</div>
      </TeacherLayout>
    )
  }

  if (!student) {
    return (
      <TeacherLayout>
        <div className="p-8 text-center space-y-4">
          <div className="text-red-500 font-medium">Nội dung không tồn tại.</div>
          <Button variant="outline" onClick={() => navigate("/teacher/classes")}>
            <ArrowLeft className="h-4 w-4 mr-2" /> Quay lại danh sách lớp
          </Button>
        </div>
      </TeacherLayout>
    )
  }

  // Calculate Statistics
  const totalExams = scores.length
  const avgScore = totalExams > 0 ? (scores.reduce((sum, s) => sum + s.score, 0) / totalExams).toFixed(2) : "-"
  const highestScore = totalExams > 0 ? Math.max(...scores.map((s) => s.score)).toFixed(2) : "-"
  const lowestScore = totalExams > 0 ? Math.min(...scores.map((s) => s.score)).toFixed(2) : "-"
  const passedCount = scores.filter((s) => s.score >= 5.0).length
  const failedCount = scores.filter((s) => s.score < 5.0).length

  // Calculate Pie Chart Proportions
  const passedPercent = totalExams > 0 ? Math.round((passedCount / totalExams) * 100) : 0
  const failedPercent = totalExams > 0 ? 100 - passedPercent : 0

  const emailAddress = `${student.id}@student.edu.vn`
  const classDisplayName = classData ? `${classData.id} - ${classData.name}` : student.classCode || "N/A"

  // Breadcrumb with Fallback logic
  const breadcrumbItems = student.classCode
    ? [
        { label: "Quản lý lớp học", path: "/teacher/classes" },
        { label: `Lớp ${student.classCode}`, path: `/teacher/classes/${student.classCode}` },
        { label: student.name },
      ]
    : [
        { label: "Quản lý lớp học", path: "/teacher/classes" },
        { label: "Sinh viên", path: "/teacher/classes" },
        { label: student.name },
      ]

  return (
    <TeacherLayout breadcrumb={breadcrumbItems}>
      <div className="space-y-6">
        {/* Profile Header Banner */}
        <div className="bg-white rounded-xl border border-gray-200 p-6 flex items-start justify-between">
          <div className="flex items-center gap-5">
            <div className="w-16 h-16 rounded-full bg-red-50 text-[#D9272B] font-bold text-xl flex items-center justify-center border border-red-100">
              {student.name.split(" ").map((n) => n[0]).slice(-2).join("")}
            </div>
            <div>
              <div className="flex items-center gap-3">
                <h1 className="text-2xl font-bold text-gray-900">{student.name}</h1>
                <span className="text-xs font-medium px-2.5 py-0.5 rounded-full bg-green-50 text-green-700 border border-green-200">
                  Đang học
                </span>
              </div>
              <div className="text-sm text-gray-500 mt-1 flex flex-wrap items-center gap-4">
                <span>MSSV: <strong className="text-gray-800">{student.id}</strong></span>
                <span>•</span>
                <span>Lớp: <strong className="text-gray-800">{classDisplayName}</strong></span>
                <span>•</span>
                <span>Email: <strong className="text-gray-800">{emailAddress}</strong></span>
              </div>
            </div>
          </div>
          <Button className="bg-[#D9272B] hover:bg-[#C42226] text-white gap-2" onClick={handleOpenEdit}>
            <Edit3 className="h-4 w-4" /> Chỉnh sửa thông tin
          </Button>
        </div>

        {/* Content Section */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          {/* Left Column: Personal Info & Exam History Table */}
          <div className="lg:col-span-2 space-y-6">
            {/* Personal Info Card */}
            <div className="bg-white rounded-xl border border-gray-200 p-6">
              <h3 className="text-base font-bold text-gray-900 mb-4 flex items-center gap-2">
                <User className="h-5 w-5 text-[#D9272B]" /> Thông tin cá nhân
              </h3>
              <div className="grid grid-cols-2 gap-4">
                <div className="bg-gray-50 p-3 rounded-lg border border-gray-100">
                  <label className="text-xs text-gray-400 block mb-1">Mã sinh viên (MSSV)</label>
                  <span className="text-sm font-bold text-gray-900">{student.id}</span>
                </div>
                <div className="bg-gray-50 p-3 rounded-lg border border-gray-100">
                  <label className="text-xs text-gray-400 block mb-1">Họ và tên</label>
                  <span className="text-sm font-bold text-gray-900">{student.name}</span>
                </div>
                <div className="bg-gray-50 p-3 rounded-lg border border-gray-100">
                  <label className="text-xs text-gray-400 block mb-1">Lớp học</label>
                  <span className="text-sm font-bold text-gray-900">{classDisplayName}</span>
                </div>
                <div className="bg-gray-50 p-3 rounded-lg border border-gray-100">
                  <label className="text-xs text-gray-400 block mb-1">Email sinh viên</label>
                  <span className="text-sm font-bold text-gray-900">{emailAddress}</span>
                </div>
              </div>
            </div>

            {/* Exam History Table */}
            <div className="bg-white rounded-xl border border-gray-200 p-6">
              <h3 className="text-base font-bold text-gray-900 mb-4 flex items-center gap-2">
                <FileText className="h-5 w-5 text-[#D9272B]" /> Kết quả thi các môn học
              </h3>
              {scores.length === 0 ? (
                <EmptyState
                  title="Sinh viên chưa tham gia kỳ thi nào"
                  description="Dữ liệu điểm thi và biểu đồ sẽ xuất hiện khi sinh viên hoàn thành ít nhất một bài thi."
                />
              ) : (
                <div className="overflow-x-auto">
                  <table className="w-full">
                    <thead>
                      <tr className="border-b border-gray-200 bg-gray-50">
                        <th className="text-left py-3 px-4 text-xs font-semibold text-gray-600">STT</th>
                        <th className="text-left py-3 px-4 text-xs font-semibold text-gray-600">Mã môn</th>
                        <th className="text-left py-3 px-4 text-xs font-semibold text-gray-600">Tên môn học</th>
                        <th className="text-center py-3 px-4 text-xs font-semibold text-gray-600">Điểm thi</th>
                        <th className="text-center py-3 px-4 text-xs font-semibold text-gray-600">Kết quả</th>
                        <th className="text-center py-3 px-4 text-xs font-semibold text-gray-600">Thao tác</th>
                      </tr>
                    </thead>
                    <tbody>
                      {scores.map((sc, idx) => (
                        <tr key={sc.subjectCode} className="border-b border-gray-100 last:border-0 hover:bg-gray-50">
                          <td className="py-3 px-4 text-sm text-gray-500">{idx + 1}</td>
                          <td className="py-3 px-4 text-sm font-bold text-gray-900">{sc.subjectCode}</td>
                          <td className="py-3 px-4 text-sm text-gray-700">{sc.subjectName}</td>
                          <td className="py-3 px-4 text-center">
                            <span className="text-sm font-bold px-3 py-1 rounded-lg bg-gray-100 text-gray-900 border border-gray-200">
                              {sc.score.toFixed(2)}
                            </span>
                          </td>
                          <td className="py-3 px-4 text-center">
                            {sc.score >= 5.0 ? (
                              <span className="text-xs font-semibold px-2.5 py-1 rounded-full bg-green-50 text-green-700 border border-green-200">
                                Đạt
                              </span>
                            ) : (
                              <span className="text-xs font-semibold px-2.5 py-1 rounded-full bg-red-50 text-red-700 border border-red-200">
                                Không đạt
                              </span>
                            )}
                          </td>
                          <td className="py-3 px-4 text-center">
                            <button
                              className="p-1.5 rounded-lg hover:bg-red-50 text-red-500 hover:text-red-700 transition-colors inline-flex items-center gap-1 text-xs font-medium"
                              title={`Xóa kết quả thi môn ${sc.subjectCode}`}
                              onClick={() => handleDeleteScore(sc.subjectCode, sc.subjectName)}
                            >
                              <Trash2 className="h-4 w-4" />
                              <span>Xóa</span>
                            </button>
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                </div>
              )}
            </div>
          </div>

          {/* Right Column: Statistics & Pie Chart */}
          <div className="space-y-6">
            {/* Statistics Card */}
            <div className="bg-white rounded-xl border border-gray-200 p-6">
              <h3 className="text-base font-bold text-gray-900 mb-4 flex items-center gap-2">
                <BookOpen className="h-5 w-5 text-[#D9272B]" /> Thống kê điểm thi
              </h3>
              <div className="grid grid-cols-2 gap-3 mb-4">
                <div className="bg-gray-50 p-3 rounded-xl border border-gray-100">
                  <span className="text-xs text-gray-500 font-medium block">Số môn đã thi</span>
                  <span className="text-lg font-bold text-gray-900">{totalExams}</span>
                </div>
                <div className="bg-gray-50 p-3 rounded-xl border border-gray-100">
                  <span className="text-xs text-gray-500 font-medium block">Điểm TB (ĐTB)</span>
                  <span className="text-lg font-bold text-[#D9272B]">{avgScore}</span>
                </div>
                <div className="bg-gray-50 p-3 rounded-xl border border-gray-100">
                  <span className="text-xs text-gray-500 font-medium block">Cao nhất</span>
                  <span className="text-lg font-bold text-green-600">{highestScore}</span>
                </div>
                <div className="bg-gray-50 p-3 rounded-xl border border-gray-100">
                  <span className="text-xs text-gray-500 font-medium block">Thấp nhất</span>
                  <span className="text-lg font-bold text-red-600">{lowestScore}</span>
                </div>
              </div>

              {/* Passed / Failed Counter */}
              <div className="pt-3 border-t border-gray-100 flex items-center justify-between text-sm">
                <span className="flex items-center gap-1.5 text-green-700 font-medium">
                  <TrendingUp className="h-4 w-4" /> Số môn Đạt (≥5.0): <strong>{passedCount}</strong>
                </span>
                <span className="flex items-center gap-1.5 text-red-700 font-medium">
                  <TrendingDown className="h-4 w-4" /> Số môn Trượt: <strong>{failedCount}</strong>
                </span>
              </div>
            </div>

            {/* SVG Pie Chart Card (Only render if totalExams > 0) */}
            {totalExams > 0 && (
              <div className="bg-white rounded-xl border border-gray-200 p-6 text-center">
                <h3 className="text-base font-bold text-gray-900 mb-4 flex items-center gap-2 text-left">
                  <Award className="h-5 w-5 text-[#D9272B]" /> Tỷ lệ Đạt / Trượt
                </h3>
                <div className="flex flex-col items-center justify-center">
                  <div className="relative w-36 h-36">
                    <svg viewBox="0 0 36 36" className="w-full h-full transform -rotate-90">
                      <path
                        d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831"
                        fill="none"
                        stroke="#EF4444"
                        strokeWidth="3.8"
                      />
                      <path
                        d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831"
                        fill="none"
                        stroke="#10B981"
                        strokeWidth="3.8"
                        strokeDasharray={`${passedPercent}, 100`}
                      />
                    </svg>
                    <div className="absolute inset-0 flex flex-col items-center justify-center">
                      <span className="text-xl font-bold text-gray-900">{passedPercent}%</span>
                      <span className="text-[10px] text-gray-400 font-medium">Tỷ lệ đạt</span>
                    </div>
                  </div>

                  <div className="flex items-center justify-center gap-6 mt-4 text-xs font-medium">
                    <div className="flex items-center gap-2">
                      <span className="w-3 h-3 rounded-full bg-green-500 inline-block" />
                      <span>Đạt ({passedCount})</span>
                    </div>
                    <div className="flex items-center gap-2">
                      <span className="w-3 h-3 rounded-full bg-red-500 inline-block" />
                      <span>Trượt ({failedCount})</span>
                    </div>
                  </div>
                </div>
              </div>
            )}
          </div>
        </div>
      </div>

      {/* Edit Student Drawer */}
      <Drawer
        open={showEditDrawer}
        onClose={() => setShowEditDrawer(false)}
        title="Chỉnh sửa thông tin sinh viên"
        footer={
          <>
            <Button variant="outline" className="flex-1 h-11" onClick={() => setShowEditDrawer(false)} disabled={isSubmitting}>
              Hủy
            </Button>
            <Button
              className="flex-1 h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
              onClick={handleSaveStudent}
              disabled={isSubmitting}
            >
              {isSubmitting ? "Đang lưu..." : "Lưu"}
            </Button>
          </>
        }
      >
        <div className="space-y-4">
          {/* MSSV (Read Only) */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">Mã sinh viên (MSSV)</Label>
            <Input value={student.id} disabled className="h-10 bg-gray-100 font-semibold" />
            <p className="text-xs text-gray-400">MSSV là khóa chính, không thể thay đổi</p>
          </div>

          {/* Email (Read Only) */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">Email sinh viên (Tự động)</Label>
            <Input value={emailAddress} disabled className="h-10 bg-gray-100 text-gray-600" />
          </div>

          {/* Class (Read Only) */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">Lớp học</Label>
            <Input value={classDisplayName} disabled className="h-10 bg-gray-100 font-semibold" />
          </div>

          {/* Họ và tên đệm */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">
              Họ và tên đệm <span className="text-[#D9272B]">*</span>
            </Label>
            <Input
              placeholder="Nhập họ và tên đệm"
              value={formHo}
              onChange={(e) => setFormHo(e.target.value)}
              onBlur={() => {
                const err = validateStudentName(formHo)
                setErrors((prev) => ({ ...prev, ho: err || undefined }))
              }}
              className="h-10"
            />
            {errors.ho && <p className="text-xs text-red-500">{errors.ho}</p>}
          </div>

          {/* Tên */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">
              Tên <span className="text-[#D9272B]">*</span>
            </Label>
            <Input
              placeholder="Nhập tên"
              value={formTen}
              onChange={(e) => setFormTen(e.target.value)}
              onBlur={() => {
                const err = validateStudentName(formTen)
                setErrors((prev) => ({ ...prev, ten: err || undefined }))
              }}
              className="h-10"
            />
            {errors.ten && <p className="text-xs text-red-500">{errors.ten}</p>}
            {errors.name && !errors.ho && !errors.ten && <p className="text-xs text-red-500">{errors.name}</p>}
          </div>


          {/* Mật khẩu */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">Mật khẩu mới (Để trống nếu không đổi)</Label>
            <Input
              type="password"
              placeholder="Nhập mật khẩu mới..."
              value={formPassword}
              onChange={(e) => setFormPassword(e.target.value)}
              className="h-10"
            />
          </div>
        </div>
      </Drawer>
    </TeacherLayout>
  )
}