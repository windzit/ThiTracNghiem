import TeacherLayout from "@/components/layouts/TeacherLayout"
import { useState, useEffect } from "react"
import { useParams, useNavigate } from "react-router-dom"
import {
  Users,
  BookOpen,
  Edit3,
  Plus,
  Search,
  Eye,
  Trash2,
  X,
  CheckCircle2,
} from "lucide-react"
import { classService } from "@/services/classService"
import { studentService } from "@/services/studentService"
import type { ClassItem, Student } from "@/types"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Badge } from "@/components/ui/badge"
import { useToast } from "@/context/ToastContext"
import { ApiErrorHandler } from "@/utils/ApiErrorHandler"

import { minDelay } from "@/utils/delay"

import {
  validateClassName,
  validateStudentId,
  validateStudentName,
  validateGender,
  validatePassword,
  normalizeIdentifier,
  normalizeText,
  normalizePassword,
  VALIDATION_CONSTANTS,
} from "@/utils/formValidation"

type TabId = "info" | "students"

const tabs = [
  { id: "info" as TabId, label: "Thông tin lớp", icon: BookOpen },
  { id: "students" as TabId, label: "Danh sách sinh viên", icon: Users },
]

export default function ClassDetail() {
  const { classId } = useParams<{ classId: string }>()
  const navigate = useNavigate()
  const { showSuccess, showError, confirm } = useToast()
  const [activeTab, setActiveTab] = useState<TabId>("info")
  const [classInfo, setClassInfo] = useState<ClassItem | null>(null)
  const [students, setStudents] = useState<Student[]>([])
  const [searchTerm, setSearchTerm] = useState("")
  const [selectedStudentIds, setSelectedStudentIds] = useState<string[]>([])

  const toggleSelectAllStudents = () => {
    if (selectedStudentIds.length === filteredStudents.length && filteredStudents.length > 0) {
      setSelectedStudentIds([])
    } else {
      setSelectedStudentIds(filteredStudents.map((s) => s.id))
    }
  }

  const toggleSelectStudent = (id: string) => {
    setSelectedStudentIds((prev) =>
      prev.includes(id) ? prev.filter((i) => i !== id) : [...prev, id]
    )
  }

  const handleBulkDeleteStudents = async () => {
    if (selectedStudentIds.length === 0) return
    const hasStudentWithScores = filteredStudents.some(
      (s) => selectedStudentIds.includes(s.id) && (s.examCount || 0) > 0
    )
    if (hasStudentWithScores) {
      showError("Không thể xóa sinh viên", "Có sinh viên trong danh sách đã tham gia thi, không thể xóa.", "BR-STUDENT-DELETE-01")
      return
    }
    confirm({
      title: "Xác nhận xóa hàng loạt sinh viên",
      message: `Bạn có chắc chắn muốn xóa ${selectedStudentIds.length} sinh viên đã chọn? Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: `Xóa ${selectedStudentIds.length} sinh viên`,
      onConfirm: async () => {
        try {
          await studentService.bulkDeleteStudents(selectedStudentIds)
          setSelectedStudentIds([])
          const list = await studentService.getByClass(classId || "")
          setStudents(list || [])
          if (classId) {
            const cls = await classService.getById(classId)
            if (cls) setClassInfo(cls)
          }
          showSuccess("Xóa hàng loạt sinh viên thành công")
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa sinh viên", parsed.message || "Sinh viên đã có kết quả thi, không thể xóa.", parsed.code)
        }
      },
    })
  }

  // Edit class form
  const [isEditingClass, setIsEditingClass] = useState(false)
  const [editForm, setEditForm] = useState({
    name: "",
  })
  const [isSavingClass, setIsSavingClass] = useState(false)

  // Add student drawer
  const [showAddStudent, setShowAddStudent] = useState(false)
  const [studentForm, setStudentForm] = useState({
    id: "",
    ho: "",
    ten: "",
    gender: "Nam" as "Nam" | "Nữ",
    password: "123",
  })
  const [studentErrors, setStudentErrors] = useState<{
    id?: string
    ho?: string
    ten?: string
    gender?: string
    password?: string
  }>({})
  const [isSubmittingStudent, setIsSubmittingStudent] = useState(false)

  useEffect(() => {
    if (classId) {
      classService.getById(classId).then((cls) => {
        if (cls) {
          setClassInfo(cls)
          setEditForm({
            name: cls.name,
          })
        }
      })
      studentService.getByClass(classId).then((students) => setStudents(students || []))
    }
  }, [classId])

  const filteredStudents = students.filter(
    (s) =>
      s.id.toLowerCase().includes(searchTerm.toLowerCase()) ||
      s.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
      (s.ho && s.ho.toLowerCase().includes(searchTerm.toLowerCase())) ||
      (s.ten && s.ten.toLowerCase().includes(searchTerm.toLowerCase()))
  )

  const handleSaveClass = async () => {
    if (!classId || !classInfo) return
    const nameErr = validateClassName(editForm.name)
    if (nameErr) return

    setIsSavingClass(true)
    try {
      const normalizedName = normalizeText(editForm.name)
      await minDelay(classService.updateClass(classId, {
        name: normalizedName,
      }), 400)
      setIsEditingClass(false)
      showSuccess("Cập nhật thông tin lớp học thành công", `Mã lớp: ${classId}`)
      setTimeout(() => {
        window.location.reload()
      }, 1500)
    } catch (err: any) {
      const parsed = ApiErrorHandler.handle(err)
      showError("Không thể cập nhật lớp học", parsed.message, parsed.code)
    } finally {
      setIsSavingClass(false)
    }
  }

  const validateStudentFormId = (val: string) => {
    let err = validateStudentId(val)
    if (!err) {
      const normalized = normalizeIdentifier(val)
      if (students.some((s) => s.id.toUpperCase() === normalized)) {
        err = "Mã sinh viên này đã tồn tại trong lớp."
      }
    }
    setStudentErrors((prev) => ({ ...prev, id: err || undefined }))
    return err
  }

  const validateStudentFormHo = (val: string) => {
    const err = validateStudentName(val)
    setStudentErrors((prev) => ({ ...prev, ho: err || undefined }))
    return err
  }

  const validateStudentFormTen = (val: string) => {
    const err = validateStudentName(val)
    setStudentErrors((prev) => ({ ...prev, ten: err || undefined }))
    return err
  }

  const validateStudentFormGender = (val: string) => {
    const err = validateGender(val)
    setStudentErrors((prev) => ({ ...prev, gender: err || undefined }))
    return err
  }

  const validateStudentFormPassword = (val: string) => {
    const err = validatePassword(val)
    setStudentErrors((prev) => ({ ...prev, password: err || undefined }))
    return err
  }

  const handleAddStudent = async () => {
    if (!classId) return
    const idErr = validateStudentFormId(studentForm.id)
    const hoErr = validateStudentFormHo(studentForm.ho)
    const tenErr = validateStudentFormTen(studentForm.ten)
    const genderErr = validateStudentFormGender(studentForm.gender)
    const passwordErr = validateStudentFormPassword(studentForm.password)

    if (idErr || hoErr || tenErr || genderErr || passwordErr) {
      return
    }

    setIsSubmittingStudent(true)
    try {
      const normalizedId = normalizeIdentifier(studentForm.id)
      const normalizedHo = normalizeText(studentForm.ho)
      const normalizedTen = normalizeText(studentForm.ten)
      const rawPassword = normalizePassword(studentForm.password)

      await minDelay(studentService.createStudent(classId, {
        id: normalizedId,
        ho: normalizedHo,
        ten: normalizedTen,
        name: `${normalizedHo} ${normalizedTen}`,
        gender: studentForm.gender,
        password: rawPassword,
      }), 400)

      setShowAddStudent(false)
      showSuccess("Thêm sinh viên thành công", `MSSV: ${normalizedId}`)
      setTimeout(() => {
        window.location.reload()
      }, 1500)
    } catch (err: any) {
      console.error("[ClassDetail] Add student error:", err)
      const parsed = ApiErrorHandler.handle(err)
      showError("Không thể thêm sinh viên", parsed.message, parsed.code)
    } finally {
      setIsSubmittingStudent(false)
    }
  }

  const handleDeleteStudent = async (studentId: string) => {
    confirm({
      title: "Xác nhận xóa sinh viên",
      message: `Bạn có chắc chắn muốn xóa sinh viên ${studentId}? Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: "Xóa sinh viên",
      onConfirm: async () => {
        try {
          await minDelay(studentService.deleteStudent(studentId), 400)
          showSuccess("Xóa sinh viên thành công", `Mã sinh viên: ${studentId}`)
          setTimeout(() => {
            window.location.reload()
          }, 1500)
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa sinh viên", parsed.message || "Sinh viên đã có kết quả thi, không thể xóa.", parsed.code)
        }
      },
    })
  }

  if (!classInfo) {
    return (
      <TeacherLayout
        breadcrumb={[
          { label: "Quản lý lớp học", path: "/teacher/classes" },
          { label: "Chi tiết lớp" },
        ]}
      >
        <div className="text-center py-12 text-gray-500">Đang tải...</div>
      </TeacherLayout>
    )
  }

  const isHoDisabled = !studentForm.id.trim() || !!studentErrors.id
  const isTenDisabled = isHoDisabled || !studentForm.ho.trim() || !!studentErrors.ho
  const isPasswordDisabled = isTenDisabled || !studentForm.ten.trim() || !!studentErrors.ten

  return (
    <TeacherLayout
      breadcrumb={[
        { label: "Quản lý lớp học", path: "/teacher/classes" },
        { label: classInfo.id },
      ]}
    >
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-start justify-between">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">{classInfo.name}</h1>
            <p className="text-sm text-gray-500 mt-1">Mã lớp: {classInfo.id}</p>
          </div>
        </div>

        {/* Tabs */}
        <div className="bg-white rounded-xl border border-gray-200">
          <div className="flex border-b border-gray-200">
            {tabs.map((tab) => {
              const Icon = tab.icon
              return (
                <button
                  key={tab.id}
                  onClick={() => setActiveTab(tab.id)}
                  className={`flex items-center gap-2 px-6 py-4 text-sm font-medium border-b-2 transition-colors ${
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

          <div className="p-6">
            {/* Tab: Thông tin lớp */}
            {activeTab === "info" && (
              <div className="space-y-6 max-w-2xl">
                {isEditingClass ? (
                  <div className="space-y-4">
                    <div className="space-y-1.5">
                      <Label>Tên lớp</Label>
                      <Input
                        value={editForm.name}
                        onChange={(e) => setEditForm({ ...editForm, name: e.target.value })}
                      />
                    </div>
                    <div className="flex items-center gap-3 pt-2">
                      <Button onClick={handleSaveClass} disabled={isSavingClass} className="bg-[#D9272B] hover:bg-[#C42226]">
                        {isSavingClass ? "Đang lưu..." : "Lưu thay đổi"}
                      </Button>
                      <Button variant="outline" onClick={() => setIsEditingClass(false)} disabled={isSavingClass}>Hủy</Button>
                    </div>
                  </div>
                ) : (
                  <div className="space-y-4">
                    <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                      <div className="bg-gray-50 p-4 rounded-xl border border-gray-100">
                        <div className="text-xs text-gray-500">Mã lớp</div>
                        <div className="text-base font-bold text-gray-900 mt-1">{classInfo.id}</div>
                      </div>
                      <div className="bg-gray-50 p-4 rounded-xl border border-gray-100">
                        <div className="text-xs text-gray-500">Tên lớp</div>
                        <div className="text-base font-bold text-gray-900 mt-1">{classInfo.name}</div>
                      </div>
                      <div className="bg-gray-50 p-4 rounded-xl border border-gray-100">
                        <div className="flex items-center gap-2 mb-2">
                          <div className="w-8 h-8 rounded-lg bg-blue-50 flex items-center justify-center">
                            <Users className="h-4 w-4 text-[#3B82F6]" />
                          </div>
                        </div>
                        <div className="text-xs text-gray-500">Tổng sinh viên</div>
                        <div className="text-xl font-bold text-gray-900">{students.length}</div>
                      </div>
                    </div>
                  </div>
                )}
                {!isEditingClass && (
                  <Button onClick={() => setIsEditingClass(true)} className="bg-[#D9272B] hover:bg-[#C42226]">
                    <Edit3 className="h-4 w-4 mr-2" /> Chỉnh sửa thông tin lớp
                  </Button>
                )}
              </div>
            )}

            {/* Tab: Danh sách sinh viên */}
            {activeTab === "students" && (
              <div className="space-y-4">
                <div className="flex items-center justify-between">
                  <div className="flex items-center gap-3">
                    <div className="relative w-64">
                      <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
                      <Input
                        placeholder="Tìm kiếm sinh viên..."
                        className="pl-10 h-10"
                        value={searchTerm}
                        onChange={(e) => setSearchTerm(e.target.value)}
                      />
                    </div>
                    {selectedStudentIds.length > 0 && (
                      <Button
                        variant="outline"
                        onClick={handleBulkDeleteStudents}
                        className="h-10 text-red-600 border-red-200 hover:bg-red-50 hover:border-red-300"
                      >
                        <Trash2 className="h-4 w-4 mr-2" /> Xóa {selectedStudentIds.length} SV đã chọn
                      </Button>
                    )}
                  </div>
                  <Button onClick={() => setShowAddStudent(true)} className="bg-[#D9272B] hover:bg-[#C42226]">
                    <Plus className="h-4 w-4 mr-2" /> Thêm sinh viên
                  </Button>
                </div>

                <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
                  <table className="w-full">
                    <thead>
                      <tr className="border-b border-gray-200 bg-gray-50/50">
                        <th className="w-12 text-center px-4 py-3">
                          <input
                            type="checkbox"
                            className="rounded border-gray-300 text-[#D9272B] focus:ring-[#D9272B] cursor-pointer"
                            checked={filteredStudents.length > 0 && selectedStudentIds.length === filteredStudents.length}
                            onChange={toggleSelectAllStudents}
                          />
                        </th>
                        <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">MASV</th>
                        <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">HỌ</th>
                        <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">TÊN</th>
                        <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">PHÁI</th>
                        <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Số môn đã thi</th>
                        <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">Thao tác</th>
                      </tr>
                    </thead>
                    <tbody>
                      {filteredStudents.map((student) => (
                        <tr key={student.id} className="border-b border-gray-100 hover:bg-gray-50/50">
                          <td className="text-center px-4 py-3">
                            <input
                              type="checkbox"
                              className="rounded border-gray-300 text-[#D9272B] focus:ring-[#D9272B] cursor-pointer"
                              checked={selectedStudentIds.includes(student.id)}
                              onChange={() => toggleSelectStudent(student.id)}
                            />
                          </td>
                          <td className="px-4 py-3 text-sm font-medium text-gray-900">{student.id}</td>
                          <td className="px-4 py-3 text-sm text-gray-700">{student.ho || student.name.split(" ").slice(0, -1).join(" ") || "-"}</td>
                          <td className="px-4 py-3 text-sm font-semibold text-gray-900">{student.ten || student.name.split(" ").slice(-1)[0] || student.name}</td>
                          <td className="px-4 py-3 text-sm text-gray-700">{student.gender || "Nam"}</td>
                          <td className="px-4 py-3">
                            {(student.examCount || 0) === 0 ? (
                              <Badge className="bg-gray-100 text-gray-600 border border-gray-200">
                                Chưa thi (0 môn)
                              </Badge>
                            ) : (
                              <Badge className="bg-blue-50 text-blue-700 border border-blue-200">
                                Đã thi ({student.examCount} môn)
                              </Badge>
                            )}
                          </td>
                          <td className="px-4 py-3">
                            <div className="flex items-center justify-center gap-1">
                              <button
                                className="p-1.5 rounded-lg hover:bg-gray-100 text-gray-500"
                                onClick={() => navigate(`/teacher/students/${student.id}`)}
                                title="Xem chi tiết sinh viên"
                              >
                                <Eye className="h-4 w-4" />
                              </button>
                              <button
                                className={`p-1.5 rounded-lg transition-colors ${
                                  (student.examCount || 0) > 0
                                    ? "opacity-40 cursor-not-allowed text-gray-300"
                                    : "hover:bg-red-50 hover:text-red-600 text-gray-500"
                                }`}
                                onClick={() => handleDeleteStudent(student.id)}
                                disabled={(student.examCount || 0) > 0}
                                title={
                                  (student.examCount || 0) > 0
                                    ? "Không thể xóa sinh viên đã có kết quả thi"
                                    : "Xóa sinh viên"
                                }
                              >
                                <Trash2 className="h-4 w-4" />
                              </button>
                            </div>
                          </td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                  {filteredStudents.length === 0 && (
                    <div className="text-center py-8 text-gray-500">Không có sinh viên nào trong lớp</div>
                  )}
                </div>
              </div>
            )}
          </div>
        </div>
      </div>

      {/* Add Student Drawer */}
      {showAddStudent && (
        <>
          <div className="fixed inset-0 bg-black/30 z-40" onClick={() => setShowAddStudent(false)} />
          <div className="fixed right-0 top-0 h-full w-[420px] bg-white shadow-2xl z-50 flex flex-col overflow-y-auto">
            <div className="flex items-center justify-between px-6 py-5 border-b border-gray-200">
              <h2 className="text-lg font-bold text-gray-900">Thêm sinh viên</h2>
              <button className="p-1 rounded-lg hover:bg-gray-100 text-gray-500" onClick={() => setShowAddStudent(false)}>
                <X className="h-5 w-5" />
              </button>
            </div>

            <form onSubmit={(e) => { e.preventDefault(); handleAddStudent(); }} className="flex-1 px-6 py-5 space-y-4">
              <div className="space-y-1.5">
                <Label className="text-sm font-medium text-gray-700">
                  Mã sinh viên (MASV) <span className="text-red-500">*</span>
                </Label>
                <Input
                  placeholder="Nhập mã sinh viên (ví dụ: N24DECE001)"
                  value={studentForm.id}
                  maxLength={VALIDATION_CONSTANTS.MAX_CODE_LENGTH}
                  onChange={(e) => {
                    const val = normalizeIdentifier(e.target.value)
                    setStudentForm({ ...studentForm, id: val })
                    validateStudentFormId(val)
                  }}
                  onBlur={() => validateStudentFormId(studentForm.id)}
                  className="h-10"
                />
                {studentErrors.id && <p className="text-xs text-red-500">{studentErrors.id}</p>}
              </div>

              <div className="space-y-1.5">
                <Label className="text-sm font-medium text-gray-700">
                  Họ <span className="text-red-500">*</span>
                </Label>
                <Input
                  placeholder="Nhập họ và tên đệm (ví dụ: Nguyễn Văn)"
                  value={studentForm.ho}
                  disabled={isHoDisabled}
                  onChange={(e) => {
                    const val = e.target.value
                    setStudentForm({ ...studentForm, ho: val })
                    validateStudentFormHo(val)
                  }}
                  onBlur={(e) => {
                    const normalized = normalizeText(e.target.value)
                    setStudentForm({ ...studentForm, ho: normalized })
                    validateStudentFormHo(normalized)
                  }}
                  className="h-10"
                />
                {studentErrors.ho && <p className="text-xs text-red-500">{studentErrors.ho}</p>}
              </div>

              <div className="space-y-1.5">
                <Label className="text-sm font-medium text-gray-700">
                  Tên <span className="text-red-500">*</span>
                </Label>
                <Input
                  placeholder="Nhập tên (ví dụ: An)"
                  value={studentForm.ten}
                  disabled={isTenDisabled}
                  onChange={(e) => {
                    const val = e.target.value
                    setStudentForm({ ...studentForm, ten: val })
                    validateStudentFormTen(val)
                  }}
                  onBlur={(e) => {
                    const normalized = normalizeText(e.target.value)
                    setStudentForm({ ...studentForm, ten: normalized })
                    validateStudentFormTen(normalized)
                  }}
                  className="h-10"
                />
                {studentErrors.ten && <p className="text-xs text-red-500">{studentErrors.ten}</p>}
              </div>

              <div className="space-y-1.5">
                <Label className="text-sm font-medium text-gray-700">
                  Giới tính <span className="text-red-500">*</span>
                </Label>
                <div className="flex items-center gap-6 pt-1">
                  <label className="flex items-center gap-2 cursor-pointer text-sm">
                    <input
                      type="radio"
                      name="gender"
                      value="Nam"
                      checked={studentForm.gender === "Nam"}
                      onChange={() => {
                        setStudentForm({ ...studentForm, gender: "Nam" })
                        validateStudentFormGender("Nam")
                      }}
                      className="text-[#D9272B] focus:ring-[#D9272B]"
                    />
                    Nam
                  </label>
                  <label className="flex items-center gap-2 cursor-pointer text-sm">
                    <input
                      type="radio"
                      name="gender"
                      value="Nữ"
                      checked={studentForm.gender === "Nữ"}
                      onChange={() => {
                        setStudentForm({ ...studentForm, gender: "Nữ" })
                        validateStudentFormGender("Nữ")
                      }}
                      className="text-[#D9272B] focus:ring-[#D9272B]"
                    />
                    Nữ
                  </label>
                </div>
                {studentErrors.gender && <p className="text-xs text-red-500">{studentErrors.gender}</p>}
              </div>

              <div className="space-y-1.5">
                <Label className="text-sm font-medium text-gray-700">Mật khẩu <span className="text-red-500">*</span></Label>
                <Input
                  type="password"
                  placeholder="Nhập mật khẩu (mặc định: 123)"
                  value={studentForm.password}
                  disabled={isPasswordDisabled}
                  onChange={(e) => {
                    const val = normalizePassword(e.target.value)
                    setStudentForm({ ...studentForm, password: val })
                    validateStudentFormPassword(val)
                  }}
                  onBlur={() => validateStudentFormPassword(studentForm.password)}
                  className="h-10"
                />
                {studentErrors.password && <p className="text-xs text-red-500">{studentErrors.password}</p>}
                <p className="text-xs text-gray-400">Mặc định: 123</p>
              </div>
            </form>

            <div className="px-6 py-4 border-t border-gray-200 flex items-center gap-3">
              <Button variant="outline" className="flex-1 h-11" onClick={() => setShowAddStudent(false)} disabled={isSubmittingStudent}>
                Hủy
              </Button>
              <Button
                className="flex-1 h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
                onClick={handleAddStudent}
                disabled={isSubmittingStudent || !!studentErrors.id || !!studentErrors.ho || !!studentErrors.ten || !!studentErrors.gender || !!studentErrors.password}
              >
                {isSubmittingStudent ? "Đang lưu..." : "Lưu"}
              </Button>
            </div>
          </div>
        </>
      )}
    </TeacherLayout>
  )

}