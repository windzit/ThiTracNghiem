import { useState, useEffect } from "react"
import { useNavigate } from "react-router-dom"
import { BookOpen, Clock, Hash, AlertTriangle, AlertCircle } from "lucide-react"
import StudentLayout from "@/widgets/layouts/StudentLayout"
import { Button } from "@/shared/ui/button"
import { Label } from "@/shared/ui/label"
import { Input } from "@/shared/ui/input"
import { Select } from "@/shared/ui/select"
import { Card, CardContent, CardHeader, CardTitle } from "@/shared/ui/card"
import { SubjectAutocomplete } from "@/shared/components"
import { subjectService } from "@/entities/subject/subjectService"
import { authService } from "@/entities/session/authService"
import { examService } from "@/entities/exam/examService"
import type { Subject } from "@/shared/types"

const timeOptions = [
  { value: "20s", label: "20 giây (Test auto-submit)" },
  { value: "5", label: "5 phút" },
  { value: "15", label: "15 phút" },
  { value: "30", label: "30 phút" },
  { value: "45", label: "45 phút" },
  { value: "60", label: "60 phút" },
  { value: "90", label: "90 phút" },
  { value: "120", label: "120 phút" },
]

export default function TakeExam() {
  const navigate = useNavigate()
  const [subjects, setSubjects] = useState<Subject[]>([])
  const [selectedSubject, setSelectedSubject] = useState<Subject | null>(null)
  const [availableQuestionCount, setAvailableQuestionCount] = useState<number | null>(null)
  const [subjectError, setSubjectError] = useState("")
  const [numQuestions, setNumQuestions] = useState("")
  const [timeLimit, setTimeLimit] = useState("")

  // Check active resume session on mount
  useEffect(() => {
    const user = authService.getCurrentUser()
    if (!user || !user.id) return

    examService.getResumeSession(user.id).then((session) => {
      if (session && session.remainingSeconds > 0) {
        const answersMap: Record<number, number> = {}
        const mappedQs = session.questions.map((q) => {
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
              subject: session.tenmh || session.mamh,
              subjectId: session.mamh,
              numQuestions: session.questions.length,
              timeLimit: session.thoiGianPhut,
              remainingSeconds: session.remainingSeconds,
              questions: mappedQs,
              answersMap,
              fullscreenRequired: !!session.fullscreenRequired,
            },
          },
        })
      }
    })
  }, [navigate])

  // Fetch all subjects on mount (Phase H: always display all available subjects returned by Backend)
  useEffect(() => {
    subjectService
      .getAll()
      .then((res) => {
        setSubjects(res)
      })
      .catch((err) => {
        console.error("[TakeExam] Failed to load subjects:", err)
      })
  }, [])

  // Set real question count directly from selectedSubject (no extra API call)
  useEffect(() => {
    setNumQuestions("")
    setSubjectError("")

    if (!selectedSubject) {
      setAvailableQuestionCount(null)
      return
    }

    const count = selectedSubject.questionCount ?? 0
    setAvailableQuestionCount(count)
    if (count === 0) {
      setSubjectError("Môn học này chưa có câu hỏi nào trong ngân hàng đề thi.")
    }
  }, [selectedSubject])

  // Validate number of questions input
  const getQuestionCountValidationError = (): string => {
    if (!selectedSubject || availableQuestionCount === null) return ""
    if (availableQuestionCount === 0) return "Môn học này chưa có câu hỏi nào trong ngân hàng đề thi."
    if (!numQuestions) return ""
    const val = Number(numQuestions)
    if (isNaN(val) || !Number.isInteger(val)) return "Số câu hỏi phải là số nguyên."
    if (val <= 0) return "Số câu hỏi phải lớn hơn 0."
    if (val > availableQuestionCount)
      return `Số câu hỏi không được vượt quá số câu hiện có trong ngân hàng (${availableQuestionCount} câu).`
    return ""
  }

  const questionValidationError = getQuestionCountValidationError()

  const canContinue =
    selectedSubject !== null &&
    availableQuestionCount !== null &&
    availableQuestionCount > 0 &&
    numQuestions !== "" &&
    questionValidationError === "" &&
    timeLimit !== ""

  const handleContinue = () => {
    if (!canContinue || !selectedSubject) return
    const isSec = timeLimit === "20s"
    const parsedMin = isSec ? 1 : parseInt(timeLimit, 10)
    const timeLimitSec = isSec ? 20 : parsedMin * 60
    const timeLimitLabel = isSec ? "20 giây (Test auto-submit)" : `${parsedMin} phút`

    navigate("/student/exam-confirm", {
      state: {
        subject: selectedSubject.name,
        subjectId: selectedSubject.id,
        numQuestions: parseInt(numQuestions, 10),
        timeLimit: parsedMin,
        timeLimitSec,
        timeLimitLabel,
      },
    })
  }

  return (
    <StudentLayout>
      <div className="max-w-2xl mx-auto space-y-6">
        {/* Title */}
        <div>
          <h1 className="text-2xl font-bold text-gray-900">Thi trắc nghiệm</h1>
          <p className="text-sm text-gray-500 mt-1">Chọn môn học và cấu hình bài thi của bạn</p>
        </div>

        {/* Form Card */}
        <Card>
          <CardHeader>
            <CardTitle className="text-lg">Cấu hình bài thi</CardTitle>
          </CardHeader>
          <CardContent className="space-y-5">
            {/* Môn học */}
            <div className="space-y-1.5">
              <Label className="text-sm font-medium text-gray-700 flex items-center gap-2">
                <BookOpen className="h-4 w-4 text-blue-500" />
                Môn học <span className="text-red-500">*</span>
              </Label>
              <SubjectAutocomplete
                items={subjects}
                value={selectedSubject}
                onChange={setSelectedSubject}
                placeholder="-- Chọn môn học --"
              />
            </div>

            {/* Error State */}
            {subjectError && (
              <div className="bg-red-50 border border-red-200 rounded-lg p-3 flex items-center gap-2 text-xs text-red-700">
                <AlertCircle className="h-4 w-4 text-red-500 flex-shrink-0" />
                <span>{subjectError}</span>
              </div>
            )}

            {/* Helper Text: Ngân hàng câu hỏi hiện có */}
            {selectedSubject && availableQuestionCount !== null && (
              <div className="text-xs font-medium text-gray-600 bg-gray-50 border border-gray-200 rounded-lg p-3 flex items-center justify-between">
                <span>Ngân hàng câu hỏi hiện có:</span>
                <strong className={availableQuestionCount > 0 ? "text-blue-600 text-sm" : "text-red-500 text-sm"}>
                  {availableQuestionCount} câu
                </strong>
              </div>
            )}

            {/* Số câu hỏi (Numeric Input) */}
            <div className="space-y-1.5">
              <Label className="text-sm font-medium text-gray-700 flex items-center gap-2">
                <Hash className="h-4 w-4 text-blue-500" />
                Số câu hỏi <span className="text-red-500">*</span>
              </Label>
              <Input
                type="number"
                min={1}
                max={availableQuestionCount || undefined}
                step={1}
                placeholder={
                  !selectedSubject
                    ? "-- Vui lòng chọn môn học trước --"
                    : availableQuestionCount === 0
                    ? "-- Môn học không có câu hỏi --"
                    : `Nhập số câu (tối đa ${availableQuestionCount} câu)`
                }
                value={numQuestions}
                onChange={(e) => setNumQuestions(e.target.value)}
                disabled={!selectedSubject || availableQuestionCount === 0}
                className={`h-10 ${
                  questionValidationError
                    ? "border-red-500 focus-visible:ring-red-500 bg-red-50/40"
                    : ""
                }`}
              />

              {/* Inline Validation Error */}
              {questionValidationError && (
                <p className="text-xs font-medium text-red-600 flex items-center gap-1 mt-1">
                  <AlertCircle className="h-3.5 w-3.5 text-red-500 flex-shrink-0" />
                  {questionValidationError}
                </p>
              )}
            </div>

            {/* Thời gian làm bài */}
            <div className="space-y-1.5">
              <Label className="text-sm font-medium text-gray-700 flex items-center gap-2">
                <Clock className="h-4 w-4 text-blue-500" />
                Thời gian làm bài (phút) <span className="text-red-500">*</span>
              </Label>
              <Select
                value={timeLimit}
                onChange={(e) => setTimeLimit(e.target.value)}
                disabled={!selectedSubject || availableQuestionCount === 0}
                options={[
                  { value: "", label: "-- Chọn thời gian --" },
                  ...timeOptions,
                ]}
              />
            </div>

            {/* Warning */}
            <div className="bg-amber-50 border border-amber-200 rounded-lg p-3 flex items-start gap-2">
              <AlertTriangle className="h-4 w-4 text-amber-500 mt-0.5 flex-shrink-0" />
              <div className="text-xs text-amber-700">
                <p className="font-medium mb-1">Lưu ý quan trọng:</p>
                <ul className="list-disc list-inside space-y-0.5">
                  <li>Câu hỏi sẽ được ngẫu nhiên lấy từ ngân hàng câu hỏi của môn học</li>
                  <li>Thời gian tính từ khi bạn nhấn "Bắt đầu làm bài"</li>
                  <li>Không được chuyển tab hoặc rời khỏi màn hình làm bài</li>
                  <li>Bài thi sẽ tự động nộp nếu vi phạm quá 3 lần</li>
                </ul>
              </div>
            </div>

            {/* Button */}
            <Button
              className="w-full h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
              disabled={!canContinue}
              onClick={handleContinue}
            >
              Tiếp tục
            </Button>
          </CardContent>
        </Card>
      </div>
    </StudentLayout>
  )
}