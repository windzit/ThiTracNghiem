import { useState, useEffect } from "react"
import { useNavigate, useSearchParams } from "react-router-dom"
import {
  Search,
  Eye,
  Pencil,
  Trash2,
  Upload,
  Plus,
  HelpCircle,
  CheckCircle2,
  Archive,
  Star,
  ArrowUpDown,
} from "lucide-react"
import TeacherLayout from "@/components/layouts/TeacherLayout"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Select } from "@/components/ui/select"
import { Textarea } from "@/components/ui/textarea"
import { RadioGroup, RadioGroupItem } from "@/components/ui/radio-group"
import { Label } from "@/components/ui/label"
import { questionService } from "@/services/questionService"
import { subjectService } from "@/services/subjectService"
import { useToast } from "@/context/ToastContext"
import type { Subject, Question } from "@/types"
import { Difficulty } from "@/data/difficulty"
import { StatCard, Pagination, Drawer, StatusBadge, SubjectAutocomplete } from "@/components/shared"
import { ApiErrorHandler } from "@/utils/ApiErrorHandler"
import { minDelay } from "@/utils/delay"
import {
  validateQuestionContent,
  validateQuestionOption,
  normalizeText,
} from "@/utils/formValidation"

export default function QuestionManagement() {
  const navigate = useNavigate()
  const [searchParams, setSearchParams] = useSearchParams()
  const { showSuccess, showError, confirm } = useToast()

  const initialSubject = searchParams.get("subject") || "all"
  const initialStatus = searchParams.get("status") || "all"
  const initialSearch = searchParams.get("q") || ""
  const initialPage = parseInt(searchParams.get("page") || "1", 10) || 1

  const [searchTerm, setSearchTerm] = useState(initialSearch)
  const [subjectFilter, setSubjectFilter] = useState(initialSubject)
  const [statusFilter, setStatusFilter] = useState(initialStatus)
  const [sortDir, setSortDir] = useState<"asc" | "desc">("asc")
  const [currentPage, setCurrentPage] = useState(initialPage)
  const [itemsPerPage, setItemsPerPage] = useState(10)
  const [showDrawer, setShowDrawer] = useState(false)
  const [editingQuestion, setEditingQuestion] = useState<Question | null>(null)
  const [selectedIds, setSelectedIds] = useState<string[]>([])
  const [questions, setQuestions] = useState<Question[]>([])
  const [subjects, setSubjects] = useState<Subject[]>([])
  const [isLoadingSubjects, setIsLoadingSubjects] = useState(false)
  const [isSubmitting, setIsSubmitting] = useState(false)

  // Validation errors state
  const [formErrors, setFormErrors] = useState<{
    subject?: string
    content?: string
    a?: string
    b?: string
    c?: string
    d?: string
  }>({})

  const updateQueryParams = (sub?: string, status?: string, q?: string, page?: number) => {
    const params = new URLSearchParams()
    const finalSubject = sub !== undefined ? sub : subjectFilter
    const finalStatus = status !== undefined ? status : statusFilter
    const finalSearch = q !== undefined ? q : searchTerm
    const finalPage = page !== undefined ? page : currentPage

    if (finalSubject && finalSubject !== "all") params.set("subject", finalSubject)
    if (finalStatus && finalStatus !== "all") params.set("status", finalStatus)
    if (finalSearch) params.set("q", finalSearch)
    if (finalPage > 1) params.set("page", finalPage.toString())

    setSearchParams(params, { replace: true })
  }

  const refreshData = async () => {
    try {
      const qs = await questionService.getQuestions()
      setQuestions(qs)
      const subs = await subjectService.getAll()
      setSubjects(subs)
    } catch (err) {
      console.error("[QuestionManagement] Failed to refresh questions or subjects:", err)
    }
  }

  useEffect(() => {
    refreshData()
    setIsLoadingSubjects(true)
    subjectService
      .getAll()
      .then(setSubjects)
      .catch((err) => {
        console.error("[QuestionManagement] Failed to load subjects:", err)
        setSubjects([])
      })
      .finally(() => setIsLoadingSubjects(false))
  }, [])

  useEffect(() => {
    const urlSubject = searchParams.get("subject") || "all"
    const urlStatus = searchParams.get("status") || "all"
    const urlSearch = searchParams.get("q") || ""
    const urlPage = parseInt(searchParams.get("page") || "1", 10) || 1

    if (urlSubject !== subjectFilter) {
      setSubjectFilter(urlSubject)
      setSortDir("asc") // Reset sort to ID Ascending when subject changes
    }
    if (urlStatus !== statusFilter) setStatusFilter(urlStatus)
    if (urlSearch !== searchTerm) setSearchTerm(urlSearch)
    if (urlPage !== currentPage) setCurrentPage(urlPage)
  }, [searchParams])

  // Form state
  const [formSubject, setFormSubject] = useState("")
  const [formContent, setFormContent] = useState("")
  const [formA, setFormA] = useState("")
  const [formB, setFormB] = useState("")
  const [formC, setFormC] = useState("")
  const [formD, setFormD] = useState("")
  const [formAnswer, setFormAnswer] = useState("A")

  const selectedSubjectObj = subjects.find((s) => s.code === subjectFilter)
  
  // Stat cards scope to selected subject if filtered
  const subjectScopedQuestions = subjectFilter === "all"
    ? questions
    : questions.filter((q) => q.subject === subjectFilter || (selectedSubjectObj && q.subject === selectedSubjectObj.name))

  // RULE: Business logic uses only used/deleted. status is presentation only.
  const filteredQuestions = questions.filter((q) => {
    const matchSearch = q.content.toLowerCase().includes(searchTerm.toLowerCase()) || q.id.includes(searchTerm)
    const matchSubject =
      subjectFilter === "all" ||
      q.subject === subjectFilter ||
      (selectedSubjectObj && q.subject === selectedSubjectObj.name)
    
    // Business rule filter
    let matchStatus = true
    if (statusFilter === "used") {
      matchStatus = q.used
    } else if (statusFilter === "unused") {
      matchStatus = !q.used && !q.deleted
    } else if (statusFilter === "deleted") {
      matchStatus = q.deleted
    }
    // "all" → no filter

    return matchSearch && matchSubject && matchStatus
  })

  // Phase E: Sort by ID (numeric)
  filteredQuestions.sort((a, b) => {
    const numA = Number(a.id) || 0
    const numB = Number(b.id) || 0
    return sortDir === "asc" ? numA - numB : numB - numA
  })

  const startIndex = (currentPage - 1) * itemsPerPage
  const paginatedQuestions = filteredQuestions.slice(startIndex, startIndex + itemsPerPage)

  // Statistics — computed independently, never derived by subtraction
  const totalQuestions = subjectScopedQuestions.length
  const usedCount = subjectScopedQuestions.filter(q => q.used).length
  const deletedCount = subjectScopedQuestions.filter(q => q.deleted).length
  const unusedCount = subjectScopedQuestions.filter(q => !q.used && !q.deleted).length

  const toggleSelectAll = () => {
    if (selectedIds.length === paginatedQuestions.length) {
      setSelectedIds([])
    } else {
      setSelectedIds(paginatedQuestions.map((q) => q.id))
    }
  }

  const toggleSelect = (id: string) => {
    setSelectedIds((prev) =>
      prev.includes(id) ? prev.filter((i) => i !== id) : [...prev, id]
    )
  }

  const validateFormContent = (val: string) => {
    const err = validateQuestionContent(val)
    setFormErrors((prev) => ({ ...prev, content: err || undefined }))
    return err
  }

  const validateFormOption = (val: string, key: "a" | "b" | "c" | "d", label: string) => {
    const err = validateQuestionOption(val, label)
    setFormErrors((prev) => ({ ...prev, [key]: err || undefined }))
    return err
  }

  const handleOpenCreateDrawer = () => {
    setEditingQuestion(null)
    setFormSubject(subjectFilter !== "all" ? subjectFilter : "")
    setFormContent("")
    setFormA("")
    setFormB("")
    setFormC("")
    setFormD("")
    setFormAnswer("A")
    setFormErrors({})
    setShowDrawer(true)
  }

  const handleOpenEditDrawer = (q: Question) => {
    // BUSINESS RULE: Cannot edit used or deleted questions
    if (q.used) {
      showError("Không thể chỉnh sửa", "Không thể chỉnh sửa câu hỏi đã được sử dụng trong bài thi.")
      return
    }
    if (q.deleted) {
      showError("Không thể chỉnh sửa", "Không thể chỉnh sửa câu hỏi đã bị vô hiệu hóa.")
      return
    }
    setEditingQuestion(q)
    const foundSubj = subjects.find(s => s.name === q.subject || s.code === q.subject)
    setFormSubject(foundSubj ? foundSubj.code : q.subject)
    setFormContent(q.content)
    setFormA(q.options[0]?.text || "")
    setFormB(q.options[1]?.text || "")
    setFormC(q.options[2]?.text || "")
    setFormD(q.options[3]?.text || "")
    const indexMap: Record<number, string> = { 0: "A", 1: "B", 2: "C", 3: "D" }
    setFormAnswer(indexMap[q.correctAnswer] || "A")
    setFormErrors({})
    setShowDrawer(true)
  }

  const handleSaveQuestion = async () => {
    if (!formSubject.trim()) {
      showError("Thiếu thông tin", "Vui lòng chọn môn học")
      return
    }

    const contentErr = validateFormContent(formContent)
    const aErr = validateFormOption(formA, "a", "A")
    const bErr = validateFormOption(formB, "b", "B")
    const cErr = validateFormOption(formC, "c", "C")
    const dErr = validateFormOption(formD, "d", "D")

    if (contentErr || aErr || bErr || cErr || dErr) {
      return
    }

    setIsSubmitting(true)
    try {
      const answerIndexMap: Record<string, number> = { "A": 0, "B": 1, "C": 2, "D": 3 }
      const ansIdx = answerIndexMap[formAnswer] ?? 0
      const normContent = normalizeText(formContent)
      const normA = normalizeText(formA)
      const normB = normalizeText(formB)
      const normC = normalizeText(formC)
      const normD = normalizeText(formD)

      if (editingQuestion) {
        // Update question
        const res = await minDelay(questionService.updateQuestion(editingQuestion.id, {
          mamh: formSubject,
          content: normContent,
          options: [
            { label: 'A', text: normA },
            { label: 'B', text: normB },
            { label: 'C', text: normC },
            { label: 'D', text: normD },
          ],
          correctAnswer: ansIdx,
          subject: formSubject,
        }), 400)

        if (!res.success) {
          showError("Không thể cập nhật câu hỏi", res.message || "Có lỗi xảy ra")
        } else {
          showSuccess("Cập nhật câu hỏi thành công!", `Mã câu hỏi: #${editingQuestion.id}`)
          setShowDrawer(false)
          setEditingQuestion(null)
          // CRUD UX: delay 2.5s → full reload
          setTimeout(() => {
            window.location.reload()
          }, 2500)
        }
      } else {
        // Create question
        await minDelay(questionService.createQuestion({
          mamh: formSubject,
          content: normContent,
          options: [
            { label: 'A', text: normA },
            { label: 'B', text: normB },
            { label: 'C', text: normC },
            { label: 'D', text: normD },
          ],
          correctAnswer: ansIdx,
          type: 'Trắc nghiệm',
          status: 'unused',
          subject: formSubject,
          difficulty: Difficulty.EASY,
          used: false,
          deleted: false,
        }), 400)
        showSuccess("Thêm câu hỏi mới thành công!")
        setShowDrawer(false)
        // CRUD UX: delay 2.5s → full reload
        setTimeout(() => {
          window.location.reload()
        }, 2500)
      }
    } catch (err: any) {
      const parsed = ApiErrorHandler.handle(err)
      showError("Không thể lưu câu hỏi", parsed.message, parsed.code)
    } finally {
      setIsSubmitting(false)
    }
  }

  // Hybrid delete: different dialogs depending on used/deleted state
  const handleSingleDelete = (q: Question) => {
    if (q.deleted) {
      showError("Không thể thực hiện", "Câu hỏi này đã bị vô hiệu hóa.")
      return
    }

    if (!q.used) {
      // CASE A: Unused → Hard Delete (permanent removal)
      confirm({
        title: "Xóa câu hỏi",
        message: "Bạn có chắc muốn xóa câu hỏi này?\n\nCâu hỏi chưa từng được sử dụng nên sẽ bị xóa vĩnh viễn khỏi hệ thống.",
        severity: "danger",
        confirmText: "Xóa",
        onConfirm: async () => {
          const res = await minDelay(questionService.deleteQuestion(q.id), 400)
          if (!res.success) {
            showError("Không thể xóa câu hỏi", res.message)
          } else {
            showSuccess("Đã xóa câu hỏi.")
            setTimeout(() => window.location.reload(), 2500)
          }
        }
      })
    } else {
      // CASE B: Used → Soft Delete (disable, preserve history)
      confirm({
        title: "Ngừng sử dụng câu hỏi",
        message: "Câu hỏi này đã từng xuất hiện trong bài thi.\n\nĐể đảm bảo tính toàn vẹn lịch sử thi,\nhệ thống sẽ KHÔNG xóa vật lý.\n\nSau khi xác nhận:\n- câu hỏi bị vô hiệu hóa\n- không thể chỉnh sửa\n- không xuất hiện trong đề thi mới\n- lịch sử bài thi vẫn xem được",
        severity: "warning",
        confirmText: "Đồng ý",
        onConfirm: async () => {
          const res = await minDelay(questionService.deleteQuestion(q.id), 400)
          if (!res.success) {
            showError("Không thể vô hiệu hóa câu hỏi", res.message)
          } else {
            showSuccess("Câu hỏi đã được chuyển sang trạng thái \"Vô hiệu hóa\".")
            setTimeout(() => window.location.reload(), 2500)
          }
        }
      })
    }
  }

  const handleBulkDelete = () => {
    if (selectedIds.length === 0) return
    const items = selectedIds.map(id => {
      const q = questions.find(item => item.id === id)
      const mamh = subjects.find(s => s.name === q?.subject || s.code === q?.subject)?.code || q?.subject || ''
      return { id, mamh }
    })

    confirm({
      title: "Xác nhận xóa hàng loạt",
      message: `Bạn có chắc chắn muốn xóa ${selectedIds.length} câu hỏi đã chọn? Thao tác này không thể hoàn tác.`,
      severity: "danger",
      confirmText: `Xóa ${selectedIds.length} câu hỏi`,
      onConfirm: async () => {
        const res = await minDelay(questionService.bulkDeleteQuestions(items), 400)
        if (!res.success) {
          showError("Không thể xóa hàng loạt câu hỏi", res.message)
        } else {
          showSuccess("Xóa hàng loạt câu hỏi thành công", `Đã xóa ${res.deletedCount || selectedIds.length} câu hỏi`)
          setTimeout(() => {
            window.location.reload()
          }, 2500)
        }
      }
    })
  }

  const breadcrumb = [
    { label: "Học vụ", path: "/teacher/dashboard" },
    { label: "Câu hỏi" },
  ]

  return (
    <TeacherLayout breadcrumb={breadcrumb} pageTitle="Câu hỏi">
      <div className="space-y-6">
        {/* Title Row */}
        <div className="flex items-start justify-between">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Quản lý câu hỏi</h1>
            <p className="text-sm text-gray-500 mt-1">Quản lý ngân hàng câu hỏi theo môn học</p>
          </div>
          <div className="flex items-center gap-2">
            <Button className="h-10 gap-2 bg-[#D9272B] hover:bg-[#C42226]" onClick={handleOpenCreateDrawer}>
              <Plus className="h-4 w-4" />
              Thêm câu hỏi
            </Button>
          </div>
        </div>

        {/* Stat Cards — independent counts */}
        <div className="grid grid-cols-2 lg:grid-cols-4 gap-3 lg:gap-4">
          <StatCard
            icon={HelpCircle}
            value={totalQuestions.toLocaleString()}
            label="Tổng câu hỏi"
            sublabel="Câu hỏi"
            iconBg="bg-blue-50"
            iconColor="text-blue-500"
            shape="rounded-full"
          />
          <StatCard
            icon={CheckCircle2}
            value={usedCount.toLocaleString()}
            label="Đã sử dụng"
            sublabel="Đã đưa vào thi"
            iconBg="bg-green-50"
            iconColor="text-green-500"
            shape="rounded-full"
          />
          <StatCard
            icon={Archive}
            value={unusedCount.toLocaleString()}
            label="Chưa sử dụng"
            sublabel="Trong ngân hàng"
            iconBg="bg-orange-50"
            iconColor="text-orange-500"
            shape="rounded-full"
          />
          <StatCard
            icon={Star}
            value={deletedCount.toLocaleString()}
            label="Vô hiệu hóa"
            sublabel="Không còn sử dụng"
            iconBg="bg-gray-100"
            iconColor="text-gray-500"
            shape="rounded-full"
          />
        </div>

        {/* Toolbar */}
        <div className="flex items-center gap-3 flex-wrap">
          <div className="relative flex-1 min-w-[200px] max-w-md">
            <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
            <Input
              placeholder="Tìm kiếm theo nội dung câu hỏi..."
              className="pl-10 h-10"
              value={searchTerm}
              onChange={(e) => {
                setSearchTerm(e.target.value)
                setCurrentPage(1)
                updateQueryParams(undefined, undefined, e.target.value, 1)
              }}
            />
          </div>
          <div className="w-[260px]">
            <SubjectAutocomplete
              items={subjects}
              value={subjects.find((s) => s.code === subjectFilter) || null}
              onChange={(selectedSubject) => {
                const code = selectedSubject ? selectedSubject.code : "all"
                setSubjectFilter(code)
                setSortDir("asc")
                setCurrentPage(1)
                updateQueryParams(code, undefined, undefined, 1)
              }}
              placeholder="Tất cả môn học"
            />
          </div>
          {/* 4 filter options */}
          <Select
            className="w-[160px] h-10"
            value={statusFilter}
            onChange={(e) => {
              setStatusFilter(e.target.value)
              setCurrentPage(1)
              updateQueryParams(undefined, e.target.value, undefined, 1)
            }}
            options={[
              { value: "all", label: "Tất cả" },
              { value: "used", label: "Đã sử dụng" },
              { value: "unused", label: "Chưa sử dụng" },
              { value: "deleted", label: "Vô hiệu hóa" },
            ]}
          />

          {/* Sort Selector (ID ↑ / ID ↓) */}
          <div className="flex items-center gap-1 bg-gray-100 p-1 rounded-lg border border-gray-200">
            <button
              onClick={() => setSortDir("asc")}
              className={`px-2.5 py-1 text-xs font-semibold rounded transition-colors flex items-center gap-1 ${
                sortDir === "asc"
                  ? "bg-white text-[#D9272B] shadow-sm"
                  : "text-gray-600 hover:text-gray-900"
              }`}
              title="Sắp xếp theo ID tăng dần"
            >
              <ArrowUpDown className="h-3 w-3" /> ID ↑
            </button>
            <button
              onClick={() => setSortDir("desc")}
              className={`px-2.5 py-1 text-xs font-semibold rounded transition-colors flex items-center gap-1 ${
                sortDir === "desc"
                  ? "bg-white text-[#D9272B] shadow-sm"
                  : "text-gray-600 hover:text-gray-900"
              }`}
              title="Sắp xếp theo ID giảm dần"
            >
              <ArrowUpDown className="h-3 w-3" /> ID ↓
            </button>
          </div>

          {selectedIds.length > 0 && (
            <Button
              className="h-10 gap-2 bg-[#D9272B] hover:bg-[#C42226] text-white font-medium"
              onClick={handleBulkDelete}
            >
              <Trash2 className="h-4 w-4" />
              Xóa đã chọn ({selectedIds.length})
            </Button>
          )}
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
                      checked={selectedIds.length === paginatedQuestions.length && paginatedQuestions.length > 0}
                      onChange={toggleSelectAll}
                    />
                  </th>
                  <th
                    className="text-left px-4 py-3 text-sm font-semibold text-gray-700 cursor-pointer select-none hover:text-[#D9272B]"
                    onClick={() => setSortDir((prev) => (prev === "asc" ? "desc" : "asc"))}
                    title="Bấm để đổi chiều sắp xếp ID"
                  >
                    ID {sortDir === "asc" ? "↑" : "↓"}
                  </th>
                  <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Nội dung câu hỏi</th>
                  <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Môn học</th>
                  <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Loại</th>
                  <th className="text-left px-4 py-3 text-sm font-semibold text-gray-700">Trạng thái</th>
                  <th className="text-center px-4 py-3 text-sm font-semibold text-gray-700">Thao tác</th>
                </tr>
              </thead>
              <tbody>
                {paginatedQuestions.map((q) => (
                  <tr key={q.id} className={`border-b border-gray-100 transition-colors ${
                    q.deleted ? 'bg-gray-50 text-gray-400' : 'hover:bg-gray-50/50'
                  }`}>
                    <td className="px-4 py-3">
                      <input
                        type="checkbox"
                        className="w-4 h-4 rounded border-gray-300 text-[#D9272B] focus:ring-[#D9272B]/20 cursor-pointer"
                        checked={selectedIds.includes(q.id)}
                        onChange={() => toggleSelect(q.id)}
                      />
                    </td>
                    <td className="px-4 py-3 text-sm font-medium">{q.id}</td>
                    <td className="px-4 py-3">
                      <div className="text-sm font-medium">{q.content}</div>
                      <div className="text-xs mt-0.5">{q.subText}</div>
                    </td>
                    <td className="px-4 py-3 text-sm">{q.subject}</td>
                    <td className="px-4 py-3 text-sm">{q.type}</td>
                    {/* Single status badge — priority: deleted > used > unused */}
                    <td className="px-4 py-3">
                      <StatusBadge status={q.deleted ? "deleted" : (q.used ? "used" : "unused")} />
                    </td>
                    <td className="px-4 py-3">
                      <div className="flex items-center justify-center gap-1">
                        {/* View — always enabled */}
                        <button
                          className="p-1.5 rounded-lg hover:bg-gray-100 transition-colors text-gray-500 hover:text-gray-700"
                          title="Xem chi tiết"
                          onClick={() => navigate(`/teacher/questions/${q.id}`)}
                        >
                          <Eye className="h-4 w-4" />
                        </button>
                        {/* Edit — disabled if deleted */}
                        <button
                          className={`p-1.5 rounded-lg transition-colors ${
                            q.deleted
                              ? 'text-gray-300 cursor-not-allowed'
                              : 'text-gray-500 hover:text-gray-700 hover:bg-gray-100'
                          }`}
                          title={q.deleted ? "Câu hỏi đã bị vô hiệu hóa." : "Chỉnh sửa"}
                          disabled={q.deleted}
                          onClick={() => !q.deleted && handleOpenEditDrawer(q)}
                        >
                          <Pencil className="h-4 w-4" />
                        </button>
                        {/* Delete — shows toast if deleted, otherwise confirmation */}
                        <button
                          className={`p-1.5 rounded-lg transition-colors ${
                            q.deleted
                              ? 'text-gray-300 cursor-not-allowed'
                              : 'text-gray-500 hover:text-red-600 hover:bg-gray-100'
                          }`}
                          title={q.deleted ? "Câu hỏi đã bị vô hiệu hóa." : "Xóa"}
                          disabled={q.deleted}
                          onClick={() => handleSingleDelete(q)}
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
            totalItems={filteredQuestions.length}
            itemsPerPage={itemsPerPage}
            onPageChange={(page) => {
              setCurrentPage(page)
              updateQueryParams(undefined, undefined, undefined, page)
            }}
            onItemsPerPageChange={(size) => { setItemsPerPage(size); setCurrentPage(1); updateQueryParams(undefined, undefined, undefined, 1) }}
            itemLabel="câu hỏi"
            pageSizeOptions={[10, 20, 50]}
          />
        </div>
      </div>

      {/* Drawer - Thêm / Chỉnh sửa câu hỏi */}
      <Drawer
        open={showDrawer}
        onClose={() => setShowDrawer(false)}
        title={editingQuestion ? `Chỉnh sửa câu hỏi #${editingQuestion.id}` : "Thêm câu hỏi mới"}
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
              onClick={handleSaveQuestion}
              disabled={isSubmitting || !!formErrors.content || !!formErrors.a || !!formErrors.b || !!formErrors.c || !!formErrors.d}
            >
              {isSubmitting ? "Đang lưu..." : "Lưu"}
            </Button>
          </>
        }
      >
        {editingQuestion && (
          <div className="space-y-1.5">
            <Label className="text-sm font-medium text-gray-700">Mã câu hỏi (ID)</Label>
            <Input value={editingQuestion.id} disabled className="h-10 bg-gray-100 font-semibold" />
          </div>
        )}

        {/* Chọn môn học */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            Chọn môn học <span className="text-[#D9272B]">*</span>
          </Label>
          <div className={editingQuestion ? "pointer-events-none opacity-60" : ""}>
            <SubjectAutocomplete
              items={subjects}
              value={subjects.find((s) => s.code === formSubject) || null}
              onChange={(selectedSubject) => setFormSubject(selectedSubject ? selectedSubject.code : "")}
              placeholder="-- Chọn môn học --"
            />
          </div>
          {editingQuestion && (
            <p className="text-xs text-gray-400">Không thể đổi môn học của câu hỏi đã tạo</p>
          )}
        </div>

        {/* Nội dung câu hỏi */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            Nội dung câu hỏi <span className="text-[#D9272B]">*</span>
          </Label>
          <Textarea
            placeholder="Nhập nội dung câu hỏi..."
            value={formContent}
            onChange={(e) => {
              const val = e.target.value.slice(0, 1000)
              setFormContent(val)
              validateFormContent(val)
            }}
            onBlur={(e) => {
              const normalized = normalizeText(e.target.value)
              setFormContent(normalized)
              validateFormContent(normalized)
            }}
            className="min-h-[80px] resize-y"
          />
          {formErrors.content && <p className="text-xs text-red-500">{formErrors.content}</p>}
          <p className="text-xs text-gray-400 text-right">{formContent.length}/1000</p>
        </div>

        {/* Đáp án A */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            A <span className="text-[#D9272B]">*</span>
          </Label>
          <Input
            placeholder="Nhập đáp án A..."
            value={formA}
            onChange={(e) => {
              setFormA(e.target.value)
              validateFormOption(e.target.value, "a", "A")
            }}
            onBlur={(e) => {
              const normalized = normalizeText(e.target.value)
              setFormA(normalized)
              validateFormOption(normalized, "a", "A")
            }}
            className="h-10"
          />
          {formErrors.a && <p className="text-xs text-red-500">{formErrors.a}</p>}
        </div>

        {/* Đáp án B */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            B <span className="text-[#D9272B]">*</span>
          </Label>
          <Input
            placeholder="Nhập đáp án B..."
            value={formB}
            onChange={(e) => {
              setFormB(e.target.value)
              validateFormOption(e.target.value, "b", "B")
            }}
            onBlur={(e) => {
              const normalized = normalizeText(e.target.value)
              setFormB(normalized)
              validateFormOption(normalized, "b", "B")
            }}
            className="h-10"
          />
          {formErrors.b && <p className="text-xs text-red-500">{formErrors.b}</p>}
        </div>

        {/* Đáp án C */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            C <span className="text-[#D9272B]">*</span>
          </Label>
          <Input
            placeholder="Nhập đáp án C..."
            value={formC}
            onChange={(e) => {
              setFormC(e.target.value)
              validateFormOption(e.target.value, "c", "C")
            }}
            onBlur={(e) => {
              const normalized = normalizeText(e.target.value)
              setFormC(normalized)
              validateFormOption(normalized, "c", "C")
            }}
            className="h-10"
          />
          {formErrors.c && <p className="text-xs text-red-500">{formErrors.c}</p>}
        </div>

        {/* Đáp án D */}
        <div className="space-y-1.5">
          <Label className="text-sm font-medium text-gray-700">
            D <span className="text-[#D9272B]">*</span>
          </Label>
          <Input
            placeholder="Nhập đáp án D..."
            value={formD}
            onChange={(e) => {
              setFormD(e.target.value)
              validateFormOption(e.target.value, "d", "D")
            }}
            onBlur={(e) => {
              const normalized = normalizeText(e.target.value)
              setFormD(normalized)
              validateFormOption(normalized, "d", "D")
            }}
            className="h-10"
          />
          {formErrors.d && <p className="text-xs text-red-500">{formErrors.d}</p>}
        </div>

        {/* Đáp án đúng */}
        <div className="space-y-2">
          <Label className="text-sm font-medium text-gray-700">
            Đáp án đúng <span className="text-[#D9272B]">*</span>
          </Label>
          <RadioGroup value={formAnswer} onValueChange={setFormAnswer} className="flex items-center gap-6">
            {["A", "B", "C", "D"].map((opt) => (
              <div key={opt} className="flex items-center gap-2 cursor-pointer" onClick={() => setFormAnswer(opt)}>
                <RadioGroupItem value={opt} id={`answer-${opt}`} />
                <Label htmlFor={`answer-${opt}`} className="text-sm text-gray-700 cursor-pointer font-medium">{opt}</Label>
              </div>
            ))}
          </RadioGroup>
        </div>
      </Drawer>
    </TeacherLayout>
  )
}