import { useState, useEffect } from "react"
import { useNavigate } from "react-router-dom"
import {
  Search,
  Eye,
  Pencil,
  Trash2,
  BookOpen,
  ArrowUpDown,
} from "lucide-react"
import TeacherLayout from "@/widgets/layouts/TeacherLayout"
import { Button } from "@/shared/ui/button"
import { Input } from "@/shared/ui/input"
import { Select } from "@/shared/ui/select"
import { Label } from "@/shared/ui/label"
import { subjectService } from "@/entities/subject/subjectService"
import type { Subject } from "@/shared/types"
import { StatCard, Pagination, Drawer, SubjectAutocomplete } from "@/shared/components"
import { useToast } from "@/app/providers/ToastContext"
import { ApiErrorHandler } from "@/shared/api/ApiErrorHandler"
import { minDelay } from "@/shared/lib/delay"
import {
  validateSubjectCode,
  validateSubjectName,
  normalizeIdentifier,
  normalizeText,
  VALIDATION_CONSTANTS,
} from "@/shared/lib/formValidation"

type DrawerMode = "create" | "edit"

export default function SubjectManagement() {
  const navigate = useNavigate()
  const { showSuccess, showError, confirm } = useToast()
  const [searchTerm, setSearchTerm] = useState("")
  const [sortOrder, setSortOrder] = useState<"none" | "questions-asc" | "questions-desc">("none")
  const [currentPage, setCurrentPage] = useState(1)
  const [itemsPerPage, setItemsPerPage] = useState(10)
  const [showDrawer, setShowDrawer] = useState(false)
  const [drawerMode, setDrawerMode] = useState<DrawerMode>("create")
  const [editingId, setEditingId] = useState<string | null>(null)
  const [selectedIds, setSelectedIds] = useState<string[]>([])
  const [subjects, setSubjects] = useState<Subject[]>([])
  const [isSubmitting, setIsSubmitting] = useState(false)

  // Validation errors state
  const [errors, setErrors] = useState<{ code?: string; name?: string }>({})

  const refreshData = async () => {
    try {
      const data = await subjectService.getAll()
      setSubjects(data)
    } catch (err) {
      console.error("[SubjectManagement] Failed to load subjects:", err)
    }
  }

  useEffect(() => {
    refreshData()
  }, [])

  // Form state (Only MAMH & TENMH)
  const [formCode, setFormCode] = useState("")
  const [formName, setFormName] = useState("")

  const filteredSubjects = subjects.filter((s) => {
    return (
      s.code.toLowerCase().includes(searchTerm.toLowerCase()) ||
      s.name.toLowerCase().includes(searchTerm.toLowerCase())
    )
  })

  if (sortOrder === "questions-asc") {
    filteredSubjects.sort((a, b) => (a.questionCount || 0) - (b.questionCount || 0))
  } else if (sortOrder === "questions-desc") {
    filteredSubjects.sort((a, b) => (b.questionCount || 0) - (a.questionCount || 0))
  }

  const startIndex = (currentPage - 1) * itemsPerPage
  const paginatedSubjects = filteredSubjects.slice(startIndex, startIndex + itemsPerPage)

  const totalSubjects = subjects.length
  const totalQuestions = subjects.reduce((sum, s) => sum + (s.questionCount || 0), 0)

  const toggleSelectAll = () => {
    if (selectedIds.length === paginatedSubjects.length) {
      setSelectedIds([])
    } else {
      setSelectedIds(paginatedSubjects.map((s) => s.id))
    }
  }

  const toggleSelect = (id: string) => {
    setSelectedIds((prev) =>
      prev.includes(id) ? prev.filter((i) => i !== id) : [...prev, id]
    )
  }

  const validateCode = (val: string, mode: DrawerMode = drawerMode) => {
    let err = validateSubjectCode(val)
    if (!err && mode === "create") {
      const normalized = normalizeIdentifier(val)
      if (subjects.some((s) => s.code.toUpperCase() === normalized)) {
        err = "Mã môn học này đã tồn tại trong hệ thống."
      }
    }
    setErrors((prev) => ({ ...prev, code: err || undefined }))
    return err
  }

  const validateName = (val: string) => {
    const err = validateSubjectName(val)
    setErrors((prev) => ({ ...prev, name: err || undefined }))
    return err
  }

  const handleResetForm = () => {
    setFormCode("")
    setFormName("")
    setErrors({})
    setEditingId(null)
  }

  const handleOpenCreate = () => {
    handleResetForm()
    setDrawerMode("create")
    setShowDrawer(true)
  }

  const handleOpenEdit = (subject: Subject) => {
    setFormCode(subject.code)
    setFormName(subject.name)
    setErrors({})
    setEditingId(subject.id)
    setDrawerMode("edit")
    setShowDrawer(true)
  }

  const handleDeleteSubject = async (id: string) => {
    const sub = subjects.find((s) => s.id === id)
    if (sub && sub.questionCount > 0) {
      showError("Không thể xóa môn học", "Môn học đang chứa câu hỏi, vui lòng xóa câu hỏi trước.", "BR-SUBJECT-DELETE-01")
      return
    }

    confirm({
      title: "Xác nhận xóa môn học",
      message: `Bạn có chắc chắn muốn xóa môn học ${id}? Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: "Xóa môn học",
      onConfirm: async () => {
        try {
          await minDelay(subjectService.deleteSubject(id), 400)
          showSuccess("Xóa môn học thành công", `Mã môn: ${id}`)
          setTimeout(() => {
            window.location.reload()
          }, 1500)
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa môn học", parsed.message, parsed.code)
        }
      },
    })
  }

  const handleBulkDelete = async () => {
    if (selectedIds.length === 0) return

    const subsWithQuestions = subjects.filter((s) => selectedIds.includes(s.id) && s.questionCount > 0)
    if (subsWithQuestions.length > 0) {
      showError("Không thể xóa môn học", `Có ${subsWithQuestions.length} môn học vẫn còn câu hỏi. Vui lòng xóa câu hỏi trước.`, "BR-SUBJECT-DELETE-01")
      return
    }

    confirm({
      title: "Xác nhận xóa hàng loạt",
      message: `Bạn có chắc chắn muốn xóa ${selectedIds.length} môn học đã chọn? Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: `Xóa ${selectedIds.length} môn học`,
      onConfirm: async () => {
        try {
          await minDelay((async () => {
            for (const id of selectedIds) {
              await subjectService.deleteSubject(id)
            }
          })(), 400)
          showSuccess("Xóa hàng loạt môn học thành công")
          setTimeout(() => {
            window.location.reload()
          }, 1500)
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa môn học", parsed.message, parsed.code)
        }
      },
    })
  }

  const handleSubmit = async () => {
    const codeErr = validateCode(formCode, drawerMode)
    const nameErr = validateName(formName)
    if (codeErr || nameErr) {
      return
    }

    setIsSubmitting(true)
    try {
      const normalizedCode = normalizeIdentifier(formCode)
      const normalizedName = normalizeText(formName)

      if (drawerMode === "create") {
        await minDelay(subjectService.createSubject({
          mamh: normalizedCode,
          tenmh: normalizedName,
        }), 400)
        showSuccess("Thêm môn học thành công", `Mã môn: ${normalizedCode}`)
      } else if (editingId) {
        await minDelay(subjectService.updateSubject(editingId, {
          tenmh: normalizedName,
        }), 400)
        showSuccess("Cập nhật môn học thành công", `Mã môn: ${editingId}`)
      }

      setShowDrawer(false)
      handleResetForm()
      setTimeout(() => {
        window.location.reload()
      }, 1500)
    } catch (err: any) {
      console.error("[SubjectManagement] Submit error:", err)
      const parsed = ApiErrorHandler.handle(err)
      showError(drawerMode === "create" ? "Không thể thêm môn học" : "Không thể cập nhật môn học", parsed.message, parsed.code)
    } finally {
      setIsSubmitting(false)
    }
  }

  return (
    <TeacherLayout>
      <div className="space-y-6">
        {/* Breadcrumb */}
        <div className="flex items-center gap-2 text-sm text-gray-500">
          <span>Học vụ</span>
          <span className="text-gray-300">›</span>
          <span className="text-gray-700 font-medium">Môn học</span>
        </div>

        {/* Title */}
        <div className="flex items-start justify-between">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Quản lý môn học</h1>
            <p className="text-sm text-gray-500 mt-1">Danh sách tất cả môn học trong hệ thống</p>
          </div>
          <div className="flex items-center gap-3">
            {selectedIds.length > 0 && (
              <Button
                variant="outline"
                className="flex items-center gap-2 px-4 py-2.5 rounded-lg border-red-200 text-red-600 hover:bg-red-50"
                onClick={handleBulkDelete}
              >
                <Trash2 className="h-4 w-4" /> Xóa đã chọn ({selectedIds.length})
              </Button>
            )}
            <Button className="h-10 gap-2 bg-[#D9272B] hover:bg-[#C42226]" onClick={handleOpenCreate}>
              <BookOpen className="h-4 w-4" />
              Thêm môn học
            </Button>
          </div>
        </div>

        {/* Stat Cards */}
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          <StatCard
            icon={BookOpen}
            value={totalSubjects}
            label="Tổng môn học"
            sublabel="Môn học"
            iconBg="bg-blue-50"
            iconColor="text-blue-500"
          />
          <StatCard
            icon={BookOpen}
            value={totalQuestions}
            label="Tổng số câu hỏi"
            sublabel="Câu hỏi"
            iconBg="bg-green-50"
            iconColor="text-green-500"
          />
        </div>

        {/* Toolbar */}
        <div className="flex items-center gap-3 bg-white rounded-xl border border-gray-200 p-4">
          <div className="flex-1 max-w-md">
            <SubjectAutocomplete
              items={subjects}
              value={subjects.find((s) => s.code === searchTerm || s.name === searchTerm) || null}
              onChange={(selectedSubject) => {
                setSearchTerm(selectedSubject ? selectedSubject.code : "")
                setCurrentPage(1)
              }}
              placeholder="Tìm theo mã môn, tên môn..."
            />
          </div>
          <Select
            className="w-[220px] h-10"
            value={sortOrder}
            onChange={(e) => {
              setSortOrder(e.target.value as any)
              setCurrentPage(1)
            }}
            options={[
              { value: "none", label: "Sắp xếp: Mặc định" },
              { value: "questions-asc", label: "Số câu hỏi: Tăng dần ↑" },
              { value: "questions-desc", label: "Số câu hỏi: Giảm dần ↓" },
            ]}
          />
        </div>

        {/* Table */}
        <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead>
                <tr className="border-b border-gray-200 bg-gray-50/50">
                  <th className="w-12 px-4 py-3">
                    <input
                      type="checkbox"
                      className="w-4 h-4 rounded border-gray-300 text-[#D9272B] focus:ring-[#D9272B]/20 cursor-pointer"
                      checked={selectedIds.length === paginatedSubjects.length && paginatedSubjects.length > 0}
                      onChange={toggleSelectAll}
                    />
                  </th>
                  <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Mã môn học</th>
                  <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Tên môn học</th>
                  <th
                    className="text-center px-4 py-3 text-sm font-semibold text-gray-700 cursor-pointer select-none hover:text-[#D9272B] transition-colors"
                    onClick={() => {
                      if (sortOrder === "questions-asc") setSortOrder("questions-desc")
                      else setSortOrder("questions-asc")
                      setCurrentPage(1)
                    }}
                    title="Bấm để đổi hướng sắp xếp số câu hỏi"
                  >
                    <div className="flex items-center justify-center gap-1">
                      <span>Số câu hỏi</span>
                      <ArrowUpDown className="h-3.5 w-3.5" />
                      {sortOrder === "questions-asc" && <span className="text-[#D9272B] font-bold">↑</span>}
                      {sortOrder === "questions-desc" && <span className="text-[#D9272B] font-bold">↓</span>}
                    </div>
                  </th>
                  <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700 w-28">Thao tác</th>
                </tr>
              </thead>
              <tbody>
                {paginatedSubjects.map((subject) => (
                  <tr key={subject.id} className="border-b border-gray-100 hover:bg-gray-50/50 transition-colors">
                    <td className="px-4 py-3">
                      <input
                        type="checkbox"
                        className="w-4 h-4 rounded border-gray-300 text-[#D9272B] focus:ring-[#D9272B]/20 cursor-pointer"
                        checked={selectedIds.includes(subject.id)}
                        onChange={() => toggleSelect(subject.id)}
                      />
                    </td>
                    <td className="px-4 py-3 text-sm font-semibold text-gray-900">{subject.code}</td>
                    <td className="px-4 py-3 text-sm text-gray-700">{subject.name}</td>
                    <td className="px-4 py-3 text-sm text-gray-700 text-center font-semibold">{subject.questionCount}</td>
                    <td className="px-4 py-3 text-center">
                      <div className="flex items-center justify-center gap-1">
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors text-gray-500 hover:text-gray-700"
                          title="Xem ngân hàng câu hỏi môn học"
                          onClick={() => navigate(`/teacher/questions?subject=${subject.code}`)}
                        >
                          <Eye className="h-4 w-4" />
                        </button>
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors text-gray-500 hover:text-gray-700"
                          title="Chỉnh sửa môn học"
                          onClick={() => handleOpenEdit(subject)}
                        >
                          <Pencil className="h-4 w-4" />
                        </button>
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors text-red-500 hover:text-red-700"
                          title="Xóa môn học"
                          onClick={() => handleDeleteSubject(subject.id)}
                        >
                          <Trash2 className="h-4 w-4" />
                        </button>
                      </div>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>

          {/* Pagination */}
          <Pagination
            currentPage={currentPage}
            totalItems={filteredSubjects.length}
            itemsPerPage={itemsPerPage}
            onPageChange={setCurrentPage}
            onItemsPerPageChange={(size) => { setItemsPerPage(size); setCurrentPage(1) }}
            itemLabel="môn học"
            pageSizeOptions={[5, 10, 20, 50]}
          />
        </div>
      </div>

      {/* Drawer - Thêm / Sửa môn học */}
      <Drawer
        open={showDrawer}
        onClose={() => setShowDrawer(false)}
        title={drawerMode === "create" ? "Thêm môn học" : "Chỉnh sửa môn học"}
        footer={
          <>
            <Button
              variant="outline"
              className="flex-1 h-11"
              onClick={() => setShowDrawer(false)}
              disabled={isSubmitting}
            >
              Hủy
            </Button>
            <Button
              className="flex-1 h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
              onClick={handleSubmit}
              disabled={isSubmitting || !!errors.code || !!errors.name}
            >
              {isSubmitting ? "Đang lưu..." : "Lưu"}
            </Button>
          </>
        }
      >
        <h3 className="text-base font-bold text-gray-900">Thông tin môn học</h3>

        {/* Mã môn học */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            Mã môn học <span className="text-[#D9272B]">*</span>
          </Label>
          <Input
            placeholder="Nhập mã môn học (ví dụ: CTDLGT)"
            value={formCode}
            maxLength={VALIDATION_CONSTANTS.MAX_CODE_LENGTH}
            disabled={drawerMode === "edit"}
            onChange={(e) => {
              const val = normalizeIdentifier(e.target.value)
              setFormCode(val)
              validateCode(val)
            }}
            onBlur={() => validateCode(formCode)}
            className={`h-10 ${drawerMode === "edit" ? "bg-gray-100 font-semibold" : ""}`}
          />
          {drawerMode === "edit" && (
            <p className="text-xs text-gray-400">Mã môn học là khóa chính, không thể thay đổi khi sửa</p>
          )}
          {errors.code && <p className="text-xs text-red-500">{errors.code}</p>}
        </div>

        {/* Tên môn học */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            Tên môn học <span className="text-[#D9272B]">*</span>
          </Label>
          <Input
            placeholder="Nhập tên môn học"
            value={formName}
            onChange={(e) => {
              const val = e.target.value
              setFormName(val)
              validateName(val)
            }}
            onBlur={(e) => {
              const normalized = normalizeText(e.target.value)
              setFormName(normalized)
              validateName(normalized)
            }}
            className="h-10"
          />
          {errors.name && <p className="text-xs text-red-500">{errors.name}</p>}
        </div>
      </Drawer>
    </TeacherLayout>
  )
}