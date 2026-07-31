import { useState, useEffect } from "react"
import { useNavigate, useParams } from "react-router-dom"
import {
  ArrowLeft,
  CheckCircle2,
  BookOpen,
  FileText,
  AlertCircle,
  HelpCircle,
  Loader2,
  Tag,
} from "lucide-react"
import TeacherLayout from "@/widgets/layouts/TeacherLayout"
import { Button } from "@/shared/ui/button"
import { Badge } from "@/shared/ui/badge"
import { EmptyState } from "@/shared/components"
import { questionService } from "@/entities/question/questionService"
import type { Question } from "@/shared/types"

export default function QuestionDetail() {
  const navigate = useNavigate()
  const { id } = useParams<{ id: string }>()
  const [question, setQuestion] = useState<Question | null>(null)
  const [loading, setLoading] = useState(true)
  const [notFound, setNotFound] = useState(false)

  useEffect(() => {
    if (!id) {
      setNotFound(true)
      setLoading(false)
      return
    }

    setLoading(true)
    setNotFound(false)

    questionService
      .getQuestionById(id)
      .then((q) => {
        if (q) {
          setQuestion(q)
        } else {
          setNotFound(true)
        }
      })
      .catch((err) => {
        console.error("[QuestionDetail] Failed to load question:", err)
        setNotFound(true)
      })
      .finally(() => {
        setLoading(false)
      })
  }, [id])

  const breadcrumb = [
    { label: "Học vụ", path: "/teacher/dashboard" },
    { label: "Câu hỏi", path: "/teacher/questions" },
    { label: "Chi tiết câu hỏi" },
  ]

  if (loading) {
    return (
      <TeacherLayout breadcrumb={breadcrumb} pageTitle="Chi tiết câu hỏi">
        <div className="flex flex-col items-center justify-center py-24 space-y-4">
          <Loader2 className="h-10 w-10 animate-spin text-[#D9272B]" />
          <p className="text-sm font-medium text-gray-500">Đang tải dữ liệu câu hỏi #{id}...</p>
        </div>
      </TeacherLayout>
    )
  }

  if (notFound || !question) {
    return (
      <TeacherLayout breadcrumb={breadcrumb} pageTitle="Chi tiết câu hỏi">
        <div className="py-12">
          <EmptyState
            title={`Không tìm thấy câu hỏi ID #${id}`}
            description="Câu hỏi bạn tìm kiếm không tồn tại hoặc đã bị xóa khỏi ngân hàng câu hỏi."
            action={
              <Button
                className="bg-[#D9272B] hover:bg-[#C42226] text-white gap-2 font-medium"
                onClick={() => navigate("/teacher/questions")}
              >
                <ArrowLeft className="h-4 w-4" />
                Quay lại danh sách câu hỏi
              </Button>
            }
          />
        </div>
      </TeacherLayout>
    )
  }

  const isUsed = question.status === "used" || (question as any).used === true

  return (
    <TeacherLayout breadcrumb={breadcrumb} pageTitle={`Chi tiết câu hỏi #${question.id}`}>
      <div className="space-y-6">
        {/* Header Navigation & Action Bar */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-2xl font-bold text-gray-900">Chi tiết câu hỏi #{question.id}</h1>
            <button
              onClick={() => navigate(-1)}
              className="flex items-center gap-1.5 text-sm text-gray-600 hover:text-[#D9272B] hover:underline mt-1 transition-colors"
            >
              <ArrowLeft className="h-4 w-4" />
              Quay lại danh sách câu hỏi
            </button>
          </div>
          <div className="flex items-center gap-2">
            <Button
              variant="outline"
              className="h-10 gap-2 border-gray-200"
              onClick={() => navigate(`/teacher/questions?subject=${question.subject}`)}
            >
              <BookOpen className="h-4 w-4 text-gray-500" />
              Xem môn học {question.subject}
            </Button>
          </div>
        </div>

        {/* 2-Column Layout */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          {/* Left Main Card (~67%) */}
          <div className="lg:col-span-2 space-y-6">
            <div className="bg-white rounded-xl border border-gray-200 p-6 shadow-sm">
              {/* Question Metadata Header */}
              <div className="flex items-center justify-between gap-3 mb-6 pb-4 border-b border-gray-100">
                <Badge className="bg-purple-50 text-purple-700 border border-purple-200 font-semibold text-sm px-3 py-1 rounded-md">
                  Mã câu hỏi: #{question.id}
                </Badge>
                <Badge
                  className={`font-semibold text-xs px-3 py-1 rounded-full border ${
                    isUsed
                      ? "bg-emerald-50 text-emerald-700 border-emerald-200"
                      : "bg-gray-100 text-gray-700 border-gray-200"
                  }`}
                >
                  {isUsed ? "Đã được chọn thi" : "Chưa từng sử dụng"}
                </Badge>
              </div>

              {/* Question Content */}
              <h2 className="text-xs font-bold text-gray-400 uppercase tracking-wider mb-2">Nội dung câu hỏi</h2>
              <div className="bg-gray-50/70 border border-gray-200/80 rounded-xl p-5 mb-6">
                <p className="text-base font-semibold text-gray-900 leading-relaxed whitespace-pre-wrap">
                  {question.content}
                </p>
              </div>

              {/* Answer Options */}
              <h2 className="text-xs font-bold text-gray-400 uppercase tracking-wider mb-3">Các phương án trả lời</h2>
              <div className="space-y-3">
                {question.options.map((option, idx) => {
                  const isCorrect = idx === question.correctAnswer
                  return (
                    <div
                      key={option.label || idx}
                      className={`flex items-start gap-3.5 px-4 py-3.5 rounded-xl border transition-all ${
                        isCorrect
                          ? "bg-emerald-50/90 border-emerald-300 shadow-sm"
                          : "bg-white border-gray-200 hover:bg-gray-50/50"
                      }`}
                    >
                      <div
                        className={`w-6 h-6 rounded-full flex items-center justify-center text-xs font-bold flex-shrink-0 mt-0.5 ${
                          isCorrect
                            ? "bg-emerald-600 text-white shadow-sm"
                            : "bg-gray-100 text-gray-600 border border-gray-200"
                        }`}
                      >
                        {option.label || String.fromCharCode(65 + idx)}
                      </div>

                      <div className="flex-1 text-sm pt-0.5">
                        <span className={isCorrect ? "text-emerald-950 font-semibold" : "text-gray-800"}>
                          {option.text}
                        </span>
                      </div>

                      {isCorrect && (
                        <Badge className="bg-emerald-600 text-white border-none font-semibold text-xs px-2.5 py-0.5 rounded-full flex items-center gap-1">
                          <CheckCircle2 className="h-3 w-3" />
                          Đáp án đúng
                        </Badge>
                      )}
                    </div>
                  )
                })}
              </div>
            </div>
          </div>

          {/* Right Information Card (~33%) */}
          <div className="space-y-6">
            <div className="bg-white rounded-xl border border-gray-200 p-5 shadow-sm space-y-4">
              <h2 className="text-base font-bold text-gray-900 border-b border-gray-100 pb-3 flex items-center gap-2">
                <HelpCircle className="h-4 w-4 text-[#D9272B]" />
                Thông tin câu hỏi
              </h2>

              <div className="space-y-4">
                <div className="flex items-start gap-3">
                  <BookOpen className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Môn học</div>
                    <div className="text-sm font-semibold text-gray-900 mt-0.5">{question.subject}</div>
                  </div>
                </div>

                <div className="flex items-start gap-3">
                  <Tag className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Mã câu hỏi</div>
                    <div className="text-sm font-semibold text-purple-700 mt-0.5">#{question.id}</div>
                  </div>
                </div>

                <div className="flex items-start gap-3">
                  <FileText className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Loại câu hỏi</div>
                    <div className="text-sm font-medium text-gray-800 mt-0.5">{question.type || "Trắc nghiệm"}</div>
                  </div>
                </div>

                <div className="flex items-start gap-3">
                  <CheckCircle2 className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Trạng thái sử dụng</div>
                    <div className="mt-1">
                      <Badge
                        className={`font-semibold text-xs px-2.5 py-0.5 rounded-full border ${
                          isUsed
                            ? "bg-emerald-50 text-emerald-700 border-emerald-200"
                            : "bg-gray-100 text-gray-700 border-gray-200"
                        }`}
                      >
                        {isUsed ? "Đã được chọn thi" : "Chưa từng sử dụng"}
                      </Badge>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </TeacherLayout>
  )
}