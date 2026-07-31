import { useState, useEffect, useCallback } from "react"
import { useNavigate } from "react-router-dom"
import {
  Search,
  Users,
  ClipboardCheck,
  ClipboardX,
  TrendingUp,
  Star,
  Award,
  Eye,
  Trash2,
  AlertTriangle,
  X,
  BookOpen,
} from "lucide-react"
import TeacherLayout from "@/widgets/layouts/TeacherLayout"
import { Button } from "@/shared/ui/button"
import { Input } from "@/shared/ui/input"
import { Select } from "@/shared/ui/select"
import { StatCard, Pagination, SubjectAutocomplete } from "@/shared/components"
import { useToast } from "@/app/providers/ToastContext"
import { classService } from "@/entities/class/classService"
import { subjectService } from "@/entities/subject/subjectService"
import { reportService, type ScoreboardStudent } from "@/entities/report/reportService"
import type { ClassItem, Subject } from "@/shared/types"
import { formatSubjectLabel } from "@/shared/lib/formatSubject"

export default function Gradebook() {
  const navigate = useNavigate()
  const [classes, setClasses] = useState<ClassItem[]>([])
  const [selectedClass, setSelectedClass] = useState<string>("")
  const [students, setStudents] = useState<ScoreboardStudent[]>([])
  const [subjectsMap, setSubjectsMap] = useState<Record<string, string>>({})
  const [selectedSubjectFilter, setSelectedSubjectFilter] = useState<Subject | null>(null)

  const [loading, setLoading] = useState(false)
  const [errorMsg, setErrorMsg] = useState("")
  const [searchTerm, setSearchTerm] = useState("")
  const [currentPage, setCurrentPage] = useState(1)
  const [itemsPerPage, setItemsPerPage] = useState(10)

  // Delete modal state
  const [deleteTarget, setDeleteTarget] = useState<{
    masv: string
    name: string
    mamh: string
  } | null>(null)
  const { showSuccess } = useToast()
  const [deleting, setDeleting] = useState(false)

  // Load class list and subject metadata on mount
  useEffect(() => {
    classService
      .getAll()
      .then((res) => {
        setClasses(res)
        if (res.length > 0) {
          setSelectedClass(res[0].id)
        }
      })
      .catch((err: any) => {
        setErrorMsg(err?.message || "Không thể tải danh sách lớp học")
      })

    subjectService
      .getAll()
      .then((subList) => {
        const map: Record<string, string> = {}
        subList.forEach((s) => {
          const code = s.code || s.id
          const name = s.name || s.id
          map[code] = name
        })
        setSubjectsMap(map)
      })
      .catch((err: any) => {
        console.error("[Gradebook] subjectService error:", err)
      })
  }, [])

  // Load scoreboard whenever selectedClass changes
  const fetchScoreboard = useCallback(() => {
    if (!selectedClass) return
    setLoading(true)
    setErrorMsg("")
    reportService
      .getScoreboard(selectedClass)
      .then((res) => {
        setStudents(res.students || [])
        setLoading(false)
      })
      .catch((err: any) => {
        console.error("[Gradebook] getScoreboard error:", err)
        setErrorMsg(err?.response?.data?.message || err?.message || "Không thể tải bảng điểm cho lớp này")
        setStudents([])
        setLoading(false)
      })
  }, [selectedClass])

  useEffect(() => {
    fetchScoreboard()
    setSelectedSubjectFilter(null)
    setCurrentPage(1)
  }, [fetchScoreboard])

  const handleDeleteScore = async () => {
    if (!deleteTarget) return
    setDeleting(true)
    try {
      await reportService.deleteScore(deleteTarget.masv, deleteTarget.mamh, selectedClass)
      setDeleteTarget(null)
      setDeleting(false)
      showSuccess("Đã xóa kết quả thi.")
      setTimeout(() => window.location.reload(), 2500)
    } catch (err: any) {
      console.error("[Gradebook] deleteScore error:", err)
      setErrorMsg(err?.response?.data?.message || err?.message || "Không thể xóa điểm thi")
      setDeleting(false)
    }
  }

  // Get unique subject codes across all students in current class
  const allSubjectCodes = Array.from(
    new Set(students.flatMap((s) => Object.keys(s.scores || {})))
  )

  // Filter subject codes: ONLY subjects where >= 1 student in current class has a non-null score
  const activeSubjectCodes = allSubjectCodes.filter((code) =>
    students.some((s) => s.scores[code] !== null && s.scores[code] !== undefined)
  )

  // Prepare Autocomplete suggestions ONLY for subjects with >= 1 taken score in current class
  const autocompleteSubjects: Subject[] = activeSubjectCodes.map((code) => {
    const name = subjectsMap[code] || code
    return {
      id: code,
      code: code,
      name: name,
      questionCount: 0,
      createdAt: "",
      status: "active",
    }
  })

  // Filter student list by search term
  const filteredStudents = students.filter(
    (s) =>
      s.masv.toLowerCase().includes(searchTerm.toLowerCase()) ||
      `${s.ho} ${s.ten}`.toLowerCase().includes(searchTerm.toLowerCase())
  )

  const startIndex = (currentPage - 1) * itemsPerPage
  const paginatedStudents = filteredStudents.slice(startIndex, startIndex + itemsPerPage)

  const totalStudents = students.length
  const takenStudentsCount = students.filter((s) =>
    Object.values(s.scores || {}).some((v) => v !== null && v !== undefined)
  ).length
  const notTakenStudentsCount = totalStudents - takenStudentsCount

  // Check if selected subject has any scores in current class
  const isSelectedSubjectTaken = selectedSubjectFilter
    ? activeSubjectCodes.includes(selectedSubjectFilter.code)
    : false

  return (
    <TeacherLayout
      breadcrumb={[
        { label: "Báo cáo", path: "/teacher/gradebook" },
        { label: "Bảng điểm" },
      ]}
    >
      <div className="space-y-6">
        {/* Header + Class Selector */}
        <div className="flex flex-col sm:flex-row items-start sm:items-center justify-between gap-4">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Bảng điểm lớp học</h1>
            <p className="text-sm text-gray-500 mt-1">
              Xem điểm chi tiết và danh sách môn thi của từng sinh viên theo lớp.
            </p>
          </div>
          <div className="flex items-center gap-3">
            <label className="text-sm font-medium text-gray-700">Chọn lớp:</label>
            <Select
              className="w-[200px] h-10"
              value={selectedClass}
              onChange={(e) => {
                setSelectedClass(e.target.value)
              }}
              options={
                classes.length > 0
                  ? classes.map((c) => ({ value: c.id, label: `${c.id} - ${c.name}` }))
                  : [{ value: "", label: "-- Không có lớp --" }]
              }
            />
          </div>
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

        {/* Stat Cards */}
        <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
          <StatCard icon={Users} value={totalStudents} label="Sĩ số lớp" sublabel="Sinh viên" iconBg="bg-purple-50" iconColor="text-purple-500" size="sm" />
          <StatCard icon={ClipboardCheck} value={takenStudentsCount} label="Đã dự thi" sublabel="Sinh viên" iconBg="bg-green-50" iconColor="text-green-500" size="sm" />
          <StatCard icon={ClipboardX} value={notTakenStudentsCount} label="Chưa dự thi" sublabel="Sinh viên" iconBg="bg-orange-50" iconColor="text-orange-500" size="sm" />
        </div>

        {/* Main Table */}
        <div className="bg-white rounded-xl border border-gray-200 overflow-hidden">
          {/* Toolbar with Search + Subject Autocomplete Filter */}
          <div className="flex flex-col sm:flex-row items-center gap-3 px-4 py-3 border-b border-gray-200">
            {/* Student Search */}
            <div className="relative flex-1 w-full max-w-md">
              <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
              <Input
                placeholder="Tìm kiếm theo mã SV, họ tên..."
                className="pl-10 h-10"
                value={searchTerm}
                onChange={(e) => {
                  setSearchTerm(e.target.value)
                  setCurrentPage(1)
                }}
              />
            </div>

            {/* Subject Autocomplete Filter (Only suggests subjects with >= 1 taken score) */}
            <div className="w-full sm:w-[320px] relative">
              <SubjectAutocomplete
                items={autocompleteSubjects}
                value={selectedSubjectFilter}
                onChange={(sub) => {
                  setSelectedSubjectFilter(sub)
                  setCurrentPage(1)
                }}
                placeholder="Lọc theo môn học..."
              />
              {selectedSubjectFilter && (
                <button
                  onClick={() => setSelectedSubjectFilter(null)}
                  className="absolute right-8 top-1/2 -translate-y-1/2 text-gray-400 hover:text-gray-600 p-1"
                  title="Bỏ lọc môn"
                >
                  <X className="h-4 w-4" />
                </button>
              )}
            </div>
          </div>

          {/* Table or Empty State */}
          {selectedSubjectFilter && !isSelectedSubjectTaken ? (
            <div className="p-12 text-center space-y-3">
              <div className="w-12 h-12 rounded-full bg-amber-50 text-amber-500 flex items-center justify-center mx-auto text-xl">
                <BookOpen className="h-6 w-6 text-amber-500" />
              </div>
              <div className="text-base font-bold text-gray-900">
                Lớp hiện chưa có sinh viên thi môn học này.
              </div>
              <p className="text-sm text-gray-500 max-w-md mx-auto">
                Môn <strong>{formatSubjectLabel(selectedSubjectFilter)}</strong> chưa có dữ liệu điểm trong lớp {selectedClass}.
              </p>
              <Button
                variant="outline"
                size="sm"
                onClick={() => setSelectedSubjectFilter(null)}
                className="gap-2 mt-2"
              >
                <X className="h-4 w-4" /> Xem tất cả danh sách lớp
              </Button>
            </div>
          ) : (
            <div className="overflow-x-auto">
              <table className="w-full">
                <thead>
                  <tr className="border-b border-gray-200 bg-gray-50/50">
                    <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700 w-12">STT</th>
                    <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Mã sinh viên</th>
                    <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Họ và tên</th>
                    <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700 w-24">Đã thi</th>

                    {/* Render score column ONLY when a subject is selected */}
                    {selectedSubjectFilter && (
                      <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">
                        {formatSubjectLabel(selectedSubjectFilter)}
                      </th>
                    )}

                    <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700 w-28">Thao tác</th>
                  </tr>
                </thead>
                <tbody>
                  {loading ? (
                    <tr>
                      <td colSpan={selectedSubjectFilter ? 6 : 5} className="text-center py-8 text-gray-500">
                        Đang tải bảng điểm...
                      </td>
                    </tr>
                  ) : paginatedStudents.length === 0 ? (
                    <tr>
                      <td colSpan={selectedSubjectFilter ? 6 : 5} className="text-center py-8 text-gray-500">
                        Không tìm thấy sinh viên nào.
                      </td>
                    </tr>
                  ) : (
                    paginatedStudents.map((student, idx) => {
                      const score = selectedSubjectFilter
                        ? student.scores[selectedSubjectFilter.code]
                        : null

                      return (
                        <tr key={student.masv} className="border-b border-gray-100 hover:bg-gray-50/50 transition-colors">
                          <td className="px-4 py-3 text-sm text-gray-500">{startIndex + idx + 1}</td>
                          <td className="px-4 py-3 text-sm font-semibold text-gray-900">{student.masv}</td>
                          <td className="px-4 py-3 text-sm text-gray-700">{`${student.ho} ${student.ten}`}</td>
                          <td className="px-4 py-3 text-sm text-center text-gray-700">{student.examCount ?? 0}</td>

                          {/* Render score cell ONLY in selected subject mode */}
                          {selectedSubjectFilter && (
                            <td className="px-4 py-3 text-sm text-center">
                              {score === null || score === undefined ? (
                                <span className="text-xs text-gray-400 italic">Chưa thi</span>
                              ) : (
                                <span className={`font-semibold ${score >= 5.0 ? "text-green-600" : "text-red-500"}`}>
                                  {score.toFixed(2)}
                                </span>
                              )}
                            </td>
                          )}

                          {/* Action column */}
                          <td className="px-4 py-3">
                            <div className="flex items-center justify-center gap-2">
                              {/* Xem bài thi button (ONLY present when subject is selected and student has a score) */}
                              {selectedSubjectFilter && score !== null && score !== undefined && (
                                <button
                                  className="p-1.5 rounded-lg hover:bg-blue-50 transition-colors text-blue-600 hover:text-blue-800 flex items-center gap-1 text-xs font-medium"
                                  title={`Xem chi tiết bài thi môn ${selectedSubjectFilter.code}`}
                                  onClick={() => {
                                    navigate("/teacher/exam-review", {
                                      state: {
                                        malop: selectedClass,
                                        masv: student.masv,
                                        mamh: selectedSubjectFilter.code,
                                      },
                                    })
                                  }}
                                >
                                  <Eye className="h-4 w-4" />
                                  <span className="hidden sm:inline">Xem bài</span>
                                </button>
                              )}

                              {/* Xem SV button */}
                              <button
                                className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors text-gray-500 hover:text-gray-700 flex items-center gap-1 text-xs font-medium"
                                title="Xem thông tin sinh viên"
                                onClick={() => navigate(`/teacher/students/${student.masv}`)}
                              >
                                <Users className="h-4 w-4" />
                                <span className="hidden sm:inline">Hồ sơ</span>
                              </button>

                              {/* Xóa button (ONLY present when subject is selected and student has a score) */}
                              {selectedSubjectFilter && score !== null && score !== undefined && (
                                <button
                                  className="p-1.5 rounded-lg hover:bg-red-50 transition-colors text-red-500 hover:text-red-700 flex items-center gap-1 text-xs font-medium"
                                  title={`Xóa điểm môn ${selectedSubjectFilter.code}`}
                                  onClick={() => {
                                    setDeleteTarget({
                                      masv: student.masv,
                                      name: `${student.ho} ${student.ten}`,
                                      mamh: selectedSubjectFilter.code,
                                    })
                                  }}
                                >
                                  <Trash2 className="h-4 w-4" />
                                  <span className="hidden sm:inline">Xóa</span>
                                </button>
                              )}
                            </div>
                          </td>
                        </tr>
                      )
                    })
                  )}
                </tbody>
              </table>
            </div>
          )}

          {/* Pagination */}
          <Pagination
            currentPage={currentPage}
            totalItems={filteredStudents.length}
            itemsPerPage={itemsPerPage}
            onPageChange={setCurrentPage}
            onItemsPerPageChange={(size) => {
              setItemsPerPage(size)
              setCurrentPage(1)
            }}
            itemLabel="sinh viên"
            pageSizeOptions={[10, 20, 50]}
          />
        </div>
      </div>

      {/* Delete Score Confirm Modal */}
      {deleteTarget && (
        <div className="fixed inset-0 bg-black/50 z-50 flex items-center justify-center p-4">
          <div className="bg-white rounded-xl p-6 max-w-md w-full shadow-2xl space-y-4">
            <div className="flex items-center gap-3">
              <div className="w-10 h-10 rounded-full bg-red-100 flex items-center justify-center flex-shrink-0">
                <AlertTriangle className="h-5 w-5 text-red-600" />
              </div>
              <h3 className="text-lg font-bold text-gray-900">Xác nhận xóa điểm thi?</h3>
            </div>
            <p className="text-sm text-gray-600">
              Bạn có chắc chắn muốn xóa điểm thi môn <strong>{deleteTarget.mamh}</strong> của sinh viên{" "}
              <strong>{deleteTarget.name}</strong> ({deleteTarget.masv})?
            </p>
            <p className="text-xs text-amber-600 bg-amber-50 p-2.5 rounded-lg border border-amber-200">
              Lưu ý: Sau khi xóa điểm, sinh viên sẽ có thể làm lại bài thi môn này.
            </p>
            <div className="flex items-center justify-end gap-3 pt-2">
              <Button variant="outline" onClick={() => setDeleteTarget(null)} disabled={deleting}>
                Hủy
              </Button>
              <Button
                className="bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
                onClick={handleDeleteScore}
                disabled={deleting}
              >
                {deleting ? "Đang xóa..." : "Xóa điểm thi"}
              </Button>
            </div>
          </div>
        </div>
      )}
    </TeacherLayout>
  )
}