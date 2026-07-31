import TeacherLayout from "@/widgets/layouts/TeacherLayout"
import { useState, useEffect, useRef } from "react"
import { useNavigate } from "react-router-dom"
import {
  Search,
  Plus,
  Eye,
  Edit3,
  Trash2,
  Users,
  GraduationCap,
  ArrowUpDown,
} from "lucide-react"
import { classService } from "@/entities/class/classService"
import type { ClassItem } from "@/shared/types"
import { Button } from "@/shared/ui/button"
import { Input } from "@/shared/ui/input"
import { Select } from "@/shared/ui/select"
import { Label } from "@/shared/ui/label"
import { StatCard, Pagination, Drawer } from "@/shared/components"
import { useToast } from "@/app/providers/ToastContext"
import { ApiErrorHandler } from "@/shared/api/ApiErrorHandler"
import { minDelay } from "@/shared/lib/delay"

import {
  validateClassCode,
  validateClassName,
  normalizeIdentifier,
  normalizeText,
  VALIDATION_CONSTANTS,
} from "@/shared/lib/formValidation"

type DrawerMode = "create" | "edit"

export default function ClassManagement() {
  const navigate = useNavigate()
  const { showSuccess, showError, showWarning, confirm } = useToast()
  const editingInitialRef = useRef<{ name: string } | null>(null)
  const [searchTerm, setSearchTerm] = useState("")
  const [sortOrder, setSortOrder] = useState<"none" | "students-asc" | "students-desc">("none")
  const [currentPage, setCurrentPage] = useState(1)
  const [itemsPerPage] = useState(10)
  const [classes, setClasses] = useState<ClassItem[]>([])
  const [selectedIds, setSelectedIds] = useState<string[]>([])
  const [showDrawer, setShowDrawer] = useState(false)
  const [drawerMode, setDrawerMode] = useState<DrawerMode>("create")
  const [editingId, setEditingId] = useState<string | null>(null)
  const [isSubmitting, setIsSubmitting] = useState(false)

  // Form state (Only real Backend fields: MALOP, TENLOP)
  const [formCode, setFormCode] = useState("")
  const [formName, setFormName] = useState("")

  // Validation errors
  const [errors, setErrors] = useState<{ code?: string; name?: string }>({})

  const refreshData = async () => {
    try {
      const data = await classService.getAll()
      setClasses(data)
    } catch (err) {
      console.error("[ClassManagement] Failed to load classes:", err)
    }
  }

  useEffect(() => {
    refreshData()
  }, [])

  const filteredClasses = classes.filter(
    (c) => c.id.toLowerCase().includes(searchTerm.toLowerCase()) || c.name.toLowerCase().includes(searchTerm.toLowerCase())
  )

  if (sortOrder === "students-asc") {
    filteredClasses.sort((a, b) => (a.total || 0) - (b.total || 0))
  } else if (sortOrder === "students-desc") {
    filteredClasses.sort((a, b) => (b.total || 0) - (a.total || 0))
  }

  const paginatedClasses = filteredClasses.slice((currentPage - 1) * itemsPerPage, currentPage * itemsPerPage)

  const toggleSelectAll = () => {
    if (selectedIds.length === paginatedClasses.length) {
      setSelectedIds([])
    } else {
      setSelectedIds(paginatedClasses.map((c) => c.id))
    }
  }

  const toggleSelect = (id: string) => {
    setSelectedIds((prev) =>
      prev.includes(id) ? prev.filter((i) => i !== id) : [...prev, id]
    )
  }

  const validateCode = (val: string, mode: DrawerMode = drawerMode) => {
    let err = validateClassCode(val)
    if (!err && mode === "create") {
      const normalized = normalizeIdentifier(val)
      if (classes.some((c) => c.id.toUpperCase() === normalized)) {
        err = "Mã lớp này đã tồn tại trong hệ thống."
      }
    }
    setErrors((prev) => ({ ...prev, code: err || undefined }))
    return err
  }

  const validateName = (val: string) => {
    const err = validateClassName(val)
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

  const handleOpenEdit = (cls: ClassItem) => {
    setFormCode(cls.id)
    setFormName(cls.name)
    setErrors({})
    setEditingId(cls.id)
    setDrawerMode("edit")
    editingInitialRef.current = {
      name: cls.name.trim(),
    }
    setShowDrawer(true)
  }

  const handleDeleteClass = async (id: string) => {
    confirm({
      title: "Xác nhận xóa lớp học",
      message: `Bạn có chắc chắn muốn xóa lớp ${id}? Chỉ có thể xóa lớp chưa có sinh viên.`,
      severity: "danger",
      confirmText: "Xóa lớp học",
      onConfirm: async () => {
        try {
          await minDelay(classService.deleteClass(id), 400)
          showSuccess("Xóa lớp học thành công", `Mã lớp: ${id}`)
          setTimeout(() => {
            window.location.reload()
          }, 1500)
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa lớp học", parsed.message || "Lớp vẫn còn sinh viên, không thể xóa.", parsed.code)
        }
      },
    })
  }

  const handleBulkDelete = async () => {
    if (selectedIds.length === 0) return

    confirm({
      title: "Xác nhận xóa hàng loạt",
      message: `Bạn có chắc chắn muốn xóa ${selectedIds.length} lớp học đã chọn? Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: `Xóa ${selectedIds.length} lớp`,
      onConfirm: async () => {
        try {
          await minDelay((async () => {
            for (const id of selectedIds) {
              await classService.deleteClass(id)
            }
          })(), 400)
          showSuccess("Xóa hàng loạt lớp học thành công")
          setTimeout(() => {
            window.location.reload()
          }, 1500)
        } catch (err: any) {
          const parsed = ApiErrorHandler.handle(err)
          showError("Không thể xóa lớp học", parsed.message || "Có lớp vẫn còn sinh viên, không thể xóa.", parsed.code)
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

      if (drawerMode === "edit" && editingInitialRef.current) {
        const isDirty = normalizedName !== editingInitialRef.current.name

        if (!isDirty) {
          setShowDrawer(false)
          showWarning("Không có thay đổi nào để lưu")
          return
        }
      }

      const classData: ClassItem = {
        id: normalizedCode,
        name: normalizedName,
        dept: "",
        current: 0,
        total: 0,
        status: "active",
      }

      if (drawerMode === "create") {
        await minDelay(classService.createClass(classData), 400)
        showSuccess("Tạo lớp học thành công", `Mã lớp: ${normalizedCode}`)
      } else if (editingId) {
        await minDelay(classService.updateClass(editingId, {
          name: normalizedName,
        }), 400)
        showSuccess("Cập nhật lớp học thành công", `Mã lớp: ${editingId}`)
      }
      setShowDrawer(false)
      handleResetForm()
      setTimeout(() => {
        window.location.reload()
      }, 1500)
    } catch (err: any) {
      console.error("[ClassManagement] Submit error:", err)
      const parsed = ApiErrorHandler.handle(err)
      showError(drawerMode === "create" ? "Không thể tạo lớp học" : "Không thể cập nhật lớp học", parsed.message, parsed.code)
    } finally {
      setIsSubmitting(false)
    }
  }

  const isNameDisabled = drawerMode === "create" && (!formCode.trim() || !!errors.code)

  return (
    <TeacherLayout>
      <div className="space-y-6">
        {/* Breadcrumb + Title + Actions */}
        <div className="flex items-start justify-between">
          <div>
            <div className="text-[13px] text-gray-400 mb-2">{"Học vụ > Lớp học"}</div>
            <h1 className="text-2xl font-bold text-gray-900">Quản lý lớp học</h1>
            <p className="text-[13px] text-gray-500 mt-1">Danh sách tất cả các lớp học trong hệ thống</p>
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
            <Button
              className="flex items-center gap-2 px-4 py-2.5 rounded-lg bg-[#D9272B] text-white text-sm font-medium hover:bg-[#c4222a] transition-colors"
              onClick={handleOpenCreate}
            >
              <Plus className="h-4 w-4" /> Thêm lớp
            </Button>
          </div>
        </div>

        {/* Stat Cards */}
        <div className="grid grid-cols-2 lg:grid-cols-2 gap-4">
          <StatCard
            icon={Users}
            value={classes.length}
            label="Tổng số lớp"
            sublabel="Lớp học"
            iconBg="bg-blue-50"
            iconColor="text-[#3B82F6]"
          />
          <StatCard
            icon={GraduationCap}
            value={classes.reduce((sum, c) => sum + (c.total || 0), 0)}
            label="Tổng sinh viên"
            sublabel="Sinh viên"
            iconBg="bg-green-50"
            iconColor="text-[#10B981]"
          />
        </div>

        {/* Toolbar */}
        <div className="bg-white rounded-xl border border-gray-200 p-4 flex items-center justify-between gap-3">
          <div className="relative w-72">
            <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
            <input
              type="text"
              placeholder="Tìm kiếm lớp..."
              value={searchTerm}
              onChange={(e) => {
                setSearchTerm(e.target.value)
                setCurrentPage(1)
              }}
              className="w-full h-10 pl-10 pr-4 rounded-lg border border-gray-200 text-sm text-gray-700 placeholder:text-gray-400 focus:outline-none focus:ring-2 focus:ring-[#D9272B]/20 focus:border-[#D9272B]"
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
              { value: "students-asc", label: "Số sinh viên: Tăng dần ↑" },
              { value: "students-desc", label: "Số sinh viên: Giảm dần ↓" },
            ]}
          />
        </div>

        {/* Table */}
        <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead>
                <tr className="bg-[#F9FAFB] border-b border-gray-200">
                  <th className="w-10 py-3 px-4">
                    <input
                      type="checkbox"
                      className="w-4 h-4 rounded border-gray-300"
                      checked={selectedIds.length === paginatedClasses.length && paginatedClasses.length > 0}
                      onChange={toggleSelectAll}
                    />
                  </th>
                  <th className="text-left text-[13px] font-medium text-gray-500 py-3 px-4">Mã lớp</th>
                  <th className="text-left text-[13px] font-medium text-gray-500 py-3 px-4">Tên lớp</th>
                  <th
                    className="text-center text-[13px] font-semibold text-gray-700 py-3 px-4 cursor-pointer select-none hover:text-[#D9272B] transition-colors"
                    onClick={() => {
                      if (sortOrder === "students-asc") setSortOrder("students-desc")
                      else setSortOrder("students-asc")
                      setCurrentPage(1)
                    }}
                    title="Bấm để đổi hướng sắp xếp số lượng sinh viên"
                  >
                    <div className="flex items-center justify-center gap-1">
                      <span>Sĩ số</span>
                      <ArrowUpDown className="h-3.5 w-3.5" />
                      {sortOrder === "students-asc" && <span className="text-[#D9272B] font-bold">↑</span>}
                      {sortOrder === "students-desc" && <span className="text-[#D9272B] font-bold">↓</span>}
                    </div>
                  </th>
                  <th className="text-center text-[13px] font-medium text-gray-500 py-3 px-4 w-28">Thao tác</th>
                </tr>
              </thead>
              <tbody>
                {paginatedClasses.map((cls) => (
                  <tr key={cls.id} className="border-b border-gray-100 last:border-0 hover:bg-gray-50/50">
                    <td className="py-3 px-4">
                      <input
                        type="checkbox"
                        className="w-4 h-4 rounded border-gray-300"
                        checked={selectedIds.includes(cls.id)}
                        onChange={() => toggleSelect(cls.id)}
                      />
                    </td>
                    <td className="py-3 px-4 text-[13px] font-medium text-gray-900">{cls.id}</td>
                    <td className="py-3 px-4 text-[13px] text-gray-700">{cls.name}</td>
                    <td className="py-3 px-4 text-center">
                      <div className="flex items-center justify-center gap-3">
                        <span className="text-[13px] font-semibold text-gray-700 min-w-[48px]">
                          {cls.total || 0} SV
                        </span>
                      </div>
                    </td>
                    <td className="py-3 px-4 text-center">
                      <div className="flex items-center justify-center gap-1">
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors"
                          title="Xem sinh viên trong lớp"
                          onClick={() => navigate(`/teacher/classes/${cls.id}`)}
                        >
                          <Eye className="h-4 w-4 text-gray-400 hover:text-gray-600" />
                        </button>
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors"
                          title="Chỉnh sửa lớp"
                          onClick={() => handleOpenEdit(cls)}
                        >
                          <Edit3 className="h-4 w-4 text-gray-400 hover:text-gray-600" />
                        </button>
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors text-red-500 hover:text-red-700"
                          title="Xóa lớp"
                          onClick={() => handleDeleteClass(cls.id)}
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

          <Pagination
            currentPage={currentPage}
            totalItems={filteredClasses.length}
            itemsPerPage={itemsPerPage}
            onPageChange={setCurrentPage}
            itemLabel="lớp"
          />
        </div>
      </div>

      {/* Drawer - Thêm/Sửa lớp */}
      <Drawer
        open={showDrawer}
        onClose={() => setShowDrawer(false)}
        title={drawerMode === "create" ? "Thêm lớp" : "Chỉnh sửa lớp"}
        footer={
          <>
            <Button variant="outline" className="flex-1 h-11" onClick={() => setShowDrawer(false)} disabled={isSubmitting}>
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
        <form onSubmit={(e) => { e.preventDefault(); handleSubmit(); }} className="space-y-4">
          <h3 className="text-base font-bold text-gray-900">Thông tin lớp học</h3>

          {/* Mã lớp */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">
              Mã lớp <span className="text-[#D9272B]">*</span>
            </Label>
            <Input
              placeholder="Nhập mã lớp (ví dụ: D24CQCE01-N)"
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
              <p className="text-xs text-gray-400">Mã lớp là khóa chính, không thể thay đổi khi sửa</p>
            )}
            {errors.code && <p className="text-xs text-red-500">{errors.code}</p>}
          </div>

          {/* Tên lớp */}
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">
              Tên lớp <span className="text-[#D9272B]">*</span>
            </Label>
            <Input
              placeholder="Nhập tên lớp"
              value={formName}
              disabled={isNameDisabled}
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
        </form>
      </Drawer>
    </TeacherLayout>
  )
}