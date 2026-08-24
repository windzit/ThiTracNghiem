import { useState, useEffect, useCallback, useRef } from "react"
import { useNavigate, useLocation } from "react-router-dom"
import {
  Clock,
  AlertTriangle,
  AlertCircle,
  ChevronLeft,
  ChevronRight,
  Flag,
  Send,
  FileText,
  Calendar,
  ListChecks,
  Timer,
  Grid3X3,
  Info,
  BookOpen,
  Lock,
  RefreshCw,
} from "lucide-react"
import { Button } from "@/shared/ui/button"
import { Badge } from "@/shared/ui/badge"
import { RadioGroup, RadioGroupItem } from "@/shared/ui/radio-group"
import { Label } from "@/shared/ui/label"
import { Card, CardContent, CardHeader, CardTitle } from "@/shared/ui/card"
import { authService } from "@/entities/session/authService"
import { examService } from "@/entities/exam/examService"
import { examSessionService } from "@/entities/exam/examSessionService"
import { useConnection } from "@/app/providers/ConnectionContext"
import { useExamSession } from "@/app/providers/ExamSessionContext"

const MAX_VIOLATIONS = 4

interface DisplayQuestion {
  id: string
  numericId: number
  content: string
  options: string[]
  type: string
  description?: string
}

export default function TakingExam() {
  const navigate = useNavigate()
  const location = useLocation()

  const locationState = location.state as {
    resumedSession?: {
      subject: string
      subjectId: string
      numQuestions: number
      timeLimit: number
      remainingSeconds: number
      questions: DisplayQuestion[]
      answersMap: Record<number, number>
      fullscreenRequired?: boolean
    }
    subject?: string
    subjectId?: string
    numQuestions?: number
    timeLimit?: number
    timeLimitSec?: number
  } | null

  const resumed = locationState?.resumedSession

  const subject = resumed ? resumed.subject : locationState?.subject || ""
  const subjectId = resumed ? resumed.subjectId : locationState?.subjectId || ""
  const numQuestions = resumed ? resumed.numQuestions : locationState?.numQuestions || 0
  const timeLimit = resumed ? resumed.timeLimit : locationState?.timeLimit || 15

  const [questions, setQuestions] = useState<DisplayQuestion[]>(resumed ? resumed.questions : [])
  const [loading, setLoading] = useState(!resumed)
  const [errorMsg, setErrorMsg] = useState("")
  const [currentQ, setCurrentQ] = useState(0)
  const [answers, setAnswers] = useState<Record<number, number>>(resumed ? resumed.answersMap : {})
  const [flagged, setFlagged] = useState<Set<string>>(new Set())
  const [timeLeft, setTimeLeft] = useState(resumed ? resumed.remainingSeconds : 0)
  const [violations, setViolations] = useState(0)
  const [showWarning, setShowWarning] = useState(false)
  const [showSubmitConfirm, setShowSubmitConfirm] = useState(false)
  const [isReady, setIsReady] = useState(false)
  const [showQuestionList, setShowQuestionList] = useState(false)
  const [fullscreenRequired, setFullscreenRequired] = useState(resumed ? !!resumed.fullscreenRequired : false)
  const [fullscreenDenied, setFullscreenDenied] = useState(false)
  const timerRef = useRef<ReturnType<typeof setInterval> | null>(null)
  const handleSubmitRef = useRef<() => void>(() => {})
  const pendingUpdatesRef = useRef<Record<number, string>>({})
  const debounceTimeoutRef = useRef<ReturnType<typeof setTimeout> | null>(null)

  const retryFullscreen = useCallback(() => {
    const el = document.documentElement
    if (el.requestFullscreen) {
      el.requestFullscreen().then(() => {
        setFullscreenDenied(false)
      }).catch((err) => {
        console.warn("[TakingExam] Retry fullscreen denied:", err)
        setFullscreenDenied(true)
      })
    }
  }, [])

  // Initialize exam from API or Resumed session
  useEffect(() => {
    if (!locationState) {
      navigate("/student/take-exam")
      return
    }

    const user = authService.getCurrentUser()
    const studentId = user?.id || "GV"

    if (resumed) {
      // Resumed Session
      examSessionService.startSession()
      setQuestions(resumed.questions)
      setAnswers(resumed.answersMap)
      setTimeLeft(resumed.remainingSeconds)
      const isFsReq = !!resumed.fullscreenRequired
      setFullscreenRequired(isFsReq)

      if (isFsReq && !document.fullscreenElement) {
        document.documentElement.requestFullscreen().catch(() => setFullscreenDenied(true))
      } else if (!isFsReq && document.fullscreenElement) {
        document.exitFullscreen().catch(() => {})
      }

      // Always fetch fresh remainingSeconds directly from backend to prevent stale state
      examService.getResumeSession(studentId).then((freshSess) => {
        if (freshSess && freshSess.remainingSeconds > 0) {
          setTimeLeft(freshSess.remainingSeconds)
        }
      }).catch(() => {})

      setLoading(false)

      if (resumed.remainingSeconds <= 0) {
        // Time expired during disconnect -> auto submit
        setTimeout(() => handleSubmitRef.current(), 500)
        return
      }

      const graceTimer = setTimeout(() => setIsReady(true), 1000)
      return () => clearTimeout(graceTimer)
    } else {
      // Start New Exam
      setLoading(true)
      examSessionService.startSession()
      examService
        .startExam(studentId, subjectId, numQuestions, timeLimit, locationState?.timeLimitSec)
        .then((res) => {
          const mappedQs: DisplayQuestion[] = res.questions.map((q) => ({
            id: String(q.id),
            numericId: q.id,
            content: q.noidung,
            options: [q.A, q.B, q.C, q.D],
            type: "Trắc nghiệm 1 đáp án",
          }))
          setQuestions(mappedQs)
          setTimeLeft(res.remainingSeconds || timeLimit * 60)
          const isFsReq = !!res.fullscreenRequired
          setFullscreenRequired(isFsReq)

          if (isFsReq && !document.fullscreenElement) {
            document.documentElement.requestFullscreen().catch(() => setFullscreenDenied(true))
          } else if (!isFsReq && document.fullscreenElement) {
            document.exitFullscreen().catch(() => {})
          }
          setLoading(false)
          const graceTimer = setTimeout(() => setIsReady(true), 1000)
          return () => clearTimeout(graceTimer)
        })
        .catch((err: any) => {
          console.error("[TakingExam] startExam error:", err)
          if (document.fullscreenElement) {
            document.exitFullscreen().catch(() => {})
          }
          examSessionService.endSession()
          const msg = err?.response?.data?.message || err?.message || "Không thể bắt đầu bài thi"
          setErrorMsg(msg)
          setLoading(false)
        })
    }
  }, [locationState, resumed, subjectId, numQuestions, timeLimit, navigate])

  const { isDisconnected } = useConnection()
  const prevDisconnectedRef = useRef(false)

  // Flush queued pending answers to backend
  const flushPendingAnswers = useCallback(async () => {
    if (isDisconnected) return

    const pending = pendingUpdatesRef.current
    const keys = Object.keys(pending).map(Number)
    if (keys.length === 0) return

    const user = authService.getCurrentUser()
    const studentId = user?.id || "GV"
    if (!studentId || !subjectId) return

    // Take snapshot and clear queue immediately
    const updatesToFlush = { ...pending }
    pendingUpdatesRef.current = {}

    for (const qNumericId of keys) {
      const optionLetter = updatesToFlush[qNumericId]
      try {
        await examService.submitAnswer(studentId, subjectId, qNumericId, optionLetter)
      } catch (err) {
        console.error(`[TakingExam] Failed to flush answer for question ${qNumericId}:`, err)
        // Push back to queue on error
        pendingUpdatesRef.current[qNumericId] = optionLetter
      }
    }
  }, [isDisconnected, subjectId])

  // Reconnect Resync: When server connection is restored, flush pending answers and fetch time
  useEffect(() => {
    if (prevDisconnectedRef.current && !isDisconnected) {
      flushPendingAnswers()
      const user = authService.getCurrentUser()
      if (user && user.id) {
        examService.getResumeSession(user.id).then((sess) => {
          if (sess && sess.remainingSeconds > 0) {
            setTimeLeft(sess.remainingSeconds)
          }
        }).catch((err) => {
          console.warn("[TakingExam] Reconnect resync warning:", err)
        })
      }
    }
    prevDisconnectedRef.current = isDisconnected
  }, [isDisconnected, flushPendingAnswers])

  // Timer (only after grace period / isReady AND when connected)
  useEffect(() => {
    if (!isReady || timeLeft <= 0 || isDisconnected) {
      if (isReady && timeLeft <= 0 && !isDisconnected) {
        handleSubmitRef.current()
      }
      return
    }
    timerRef.current = setInterval(() => {
      setTimeLeft(prev => prev - 1)
    }, 1000)
    return () => {
      if (timerRef.current) clearInterval(timerRef.current)
    }
  }, [isReady, timeLeft, isDisconnected])

  const handleViolation = useCallback(() => {
    if (!fullscreenRequired || fullscreenDenied) return
    if (isDisconnected) return
    examSessionService.recordViolation("EXIT_FULLSCREEN")
    setViolations(prev => {
      const newCount = prev + 1
      if (newCount >= MAX_VIOLATIONS) {
        setShowWarning(true)
        setTimeout(() => handleSubmitRef.current(), 2000)
      } else {
        setShowWarning(true)
      }
      return newCount
    })
  }, [isDisconnected, fullscreenRequired, fullscreenDenied])

  // Fullscreen change detection
  useEffect(() => {
    if (!isReady || !fullscreenRequired || fullscreenDenied) return
    const handleFullscreenChange = () => {
      if (!document.fullscreenElement && examSessionService.isSessionActive()) {
        handleViolation()
        setTimeout(() => {
          const el = document.documentElement
          if (el.requestFullscreen) {
            el.requestFullscreen().catch(() => setFullscreenDenied(true))
          }
        }, 500)
      }
    }
    document.addEventListener("fullscreenchange", handleFullscreenChange)
    return () => document.removeEventListener("fullscreenchange", handleFullscreenChange)
  }, [isReady, fullscreenRequired, fullscreenDenied, handleViolation])

  const { clearSession } = useExamSession()

  const handleSubmit = useCallback(async () => {
    // Clear any active debounce timer and flush pending answers before submit
    if (debounceTimeoutRef.current) {
      clearTimeout(debounceTimeoutRef.current)
    }
    await flushPendingAnswers()

    clearSession()
    examSessionService.endSession()
    setIsReady(false)
    if (timerRef.current) clearInterval(timerRef.current)
    if (document.fullscreenElement) {
      document.exitFullscreen().catch(() => {})
    }
    if (!subjectId) return

    const user = authService.getCurrentUser()
    const studentId = user?.id || "GV"

    const answersPayload = questions.map((q) => {
      const optionIdx = answers[q.numericId]
      const optionLetter = optionIdx !== undefined ? ["A", "B", "C", "D"][optionIdx] : ""
      return { id: q.numericId, answer: optionLetter }
    })

    try {
      const result = await examService.submitExam(studentId, subjectId, answersPayload)
      navigate("/student/exam-submitted", {
        state: {
          subject,
          subjectId,
          numQuestions: questions.length,
          timeLimit,
          soDung: result.soDung,
          total: result.total,
          diem: result.diem,
        },
      })
    } catch (err: any) {
      console.error("[TakingExam] submitExam error:", err)
      navigate("/student/exam-submitted", {
        state: {
          subject,
          subjectId,
          numQuestions: questions.length,
          timeLimit,
        },
      })
    }
  }, [subject, subjectId, questions, answers, timeLimit, navigate, flushPendingAnswers])

  useEffect(() => {
    handleSubmitRef.current = handleSubmit
  }, [handleSubmit])

  // Select option & Sync with debounced save & offline queue
  const handleSelectOption = (qNumericId: number, optionIdx: number) => {
    // 1. Update local state immediately for high responsiveness (Optimistic UI)
    setAnswers(prev => ({ ...prev, [qNumericId]: optionIdx }))
    const optionLetter = ["A", "B", "C", "D"][optionIdx]
    
    // 2. Put in pending queue
    pendingUpdatesRef.current[qNumericId] = optionLetter

    // 3. Clear existing timeout
    if (debounceTimeoutRef.current) {
      clearTimeout(debounceTimeoutRef.current)
    }

    // 4. Setup 200ms debounce
    debounceTimeoutRef.current = setTimeout(() => {
      flushPendingAnswers()
    }, 200)
  }

  // Visibility detection
  useEffect(() => {
    if (!isReady) return
    const handleVisibility = () => {
      if (document.hidden && questions.length > 0) handleViolation()
    }
    document.addEventListener("visibilitychange", handleVisibility)
    return () => document.removeEventListener("visibilitychange", handleVisibility)
  }, [isReady, questions.length, handleViolation])

  // Blur detection
  useEffect(() => {
    if (!isReady) return
    const handleBlur = () => {
      if (questions.length > 0) handleViolation()
    }
    window.addEventListener("blur", handleBlur)
    return () => window.removeEventListener("blur", handleBlur)
  }, [isReady, questions.length, handleViolation])

  // BeforeUnload Warning
  useEffect(() => {
    if (!isReady) return
    const handleBeforeUnload = (e: BeforeUnloadEvent) => {
      e.preventDefault()
      e.returnValue = "Bài thi đang diễn ra. Rời khỏi trang có thể làm gián đoạn thời gian làm bài!"
      return e.returnValue
    }
    window.addEventListener("beforeunload", handleBeforeUnload)
    return () => window.removeEventListener("beforeunload", handleBeforeUnload)
  }, [isReady])

  const formatTime = (seconds: number) => {
    const h = Math.floor(seconds / 3600)
    const m = Math.floor((seconds % 3600) / 60)
    const s = seconds % 60
    if (h > 0) return `${h.toString().padStart(2, "0")}:${m.toString().padStart(2, "0")}:${s.toString().padStart(2, "0")}`
    return `${m.toString().padStart(2, "0")}:${s.toString().padStart(2, "0")}`
  }

  const toggleFlag = () => {
    const qId = questions[currentQ]?.id
    if (!qId) return
    setFlagged(prev => {
      const next = new Set(prev)
      if (next.has(qId)) next.delete(qId)
      else next.add(qId)
      return next
    })
  }

  const getQuestionStatus = (q: DisplayQuestion, idx: number) => {
    if (idx === currentQ) return "current"
    if (flagged.has(q.id)) return "flagged"
    if (answers[q.numericId] !== undefined) return "answered"
    return "unanswered"
  }

  const getStatusColor = (status: string) => {
    switch (status) {
      case "current": return "border-2 border-blue-500 bg-blue-50 text-blue-700 font-bold"
      case "answered": return "bg-green-100 text-green-700 border border-green-300"
      case "flagged": return "bg-orange-50 text-orange-700 border-2 border-orange-400"
      default: return "bg-white text-gray-600 border border-gray-200 hover:bg-gray-50"
    }
  }

  if (!locationState) return null

  if (loading) {
    return (
      <div className="min-h-screen bg-[#F9FAFB] flex flex-col items-center justify-center p-6">
        <div className="bg-white rounded-2xl border border-gray-200 p-8 max-w-md w-full text-center space-y-4 shadow-sm">
          <div className="w-12 h-12 border-4 border-blue-600 border-t-transparent rounded-full animate-spin mx-auto" />
          <h2 className="text-lg font-bold text-gray-900">Đang chuẩn bị đề thi...</h2>
          <p className="text-sm text-gray-500">
            Hệ thống đang xáo trộn và khởi tạo bộ câu hỏi ngẫu nhiên cho môn <strong>{subject}</strong>.
          </p>
        </div>
      </div>
    )
  }

  if (errorMsg || questions.length === 0) {
    return (
      <div className="min-h-screen bg-[#F9FAFB] flex flex-col items-center justify-center p-6">
        <div className="bg-white rounded-2xl border border-gray-200 p-8 max-w-md w-full text-center space-y-4 shadow-sm">
          <div className="w-12 h-12 rounded-full bg-red-50 text-red-500 flex items-center justify-center mx-auto text-xl font-bold">
            !
          </div>
          <h2 className="text-lg font-bold text-gray-900">Không thể tạo bài thi</h2>
          <p className="text-sm text-red-600 font-medium">
            {errorMsg || "Không tải được danh sách câu hỏi cho môn thi này."}
          </p>
          <Button
            className="w-full bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
            onClick={() => navigate("/student/take-exam")}
          >
            Quay lại cấu hình bài thi
          </Button>
        </div>
      </div>
    )
  }

  const answeredCount = Object.keys(answers).length
  const progress = (answeredCount / questions.length) * 100
  const currentQuestion = questions[currentQ]
  const isFlagged = flagged.has(currentQuestion?.id)

  return (
    <div className="min-h-screen bg-[#F9FAFB] p-4 lg:p-6">
      <div className="max-w-7xl mx-auto space-y-6">

        {/* 4 Stat Cards */}
        <div className="grid grid-cols-4 gap-4">
          <div className="bg-white rounded-xl border border-gray-200 p-4 flex items-center gap-3">
            <div className="w-12 h-12 rounded-xl bg-blue-50 flex items-center justify-center flex-shrink-0">
              <FileText className="h-6 w-6 text-blue-500" />
            </div>
            <div>
              <div className="text-xs text-gray-500">Bài thi</div>
              <div className="text-sm font-bold text-gray-900">{subject}</div>
            </div>
          </div>
          <div className="bg-white rounded-xl border border-gray-200 p-4 flex items-center gap-3">
            <div className="w-12 h-12 rounded-xl bg-green-50 flex items-center justify-center flex-shrink-0">
              <Calendar className="h-6 w-6 text-green-500" />
            </div>
            <div>
              <div className="text-xs text-gray-500">Thời gian làm bài</div>
              <div className="text-sm font-bold text-gray-900">{timeLimit} phút</div>
            </div>
          </div>
          <div className="bg-white rounded-xl border border-gray-200 p-4 flex items-center gap-3">
            <div className="w-12 h-12 rounded-xl bg-orange-50 flex items-center justify-center flex-shrink-0">
              <ListChecks className="h-6 w-6 text-orange-500" />
            </div>
            <div>
              <div className="text-xs text-gray-500">Số câu hỏi</div>
              <div className="text-sm font-bold text-gray-900">{questions.length} câu</div>
            </div>
          </div>
          <div className="bg-white rounded-xl border border-gray-200 p-4 flex items-center gap-3">
            <div className="w-12 h-12 rounded-xl bg-red-50 flex items-center justify-center flex-shrink-0">
              <Timer className="h-6 w-6 text-red-500" />
            </div>
            <div>
              <div className="text-xs text-gray-500">Thời gian còn lại</div>
              <div className={`text-lg font-bold font-mono ${timeLeft < 300 ? "text-red-600 animate-pulse" : "text-red-600"}`}>
                {formatTime(timeLeft)}
              </div>
            </div>
          </div>
        </div>

        {/* Main Content + Sidebar */}
        <div className="flex gap-6">
          {/* Left — Question Card */}
          <div className="flex-1 min-w-0">
            <Card>
              <CardContent className="p-6">
                {/* Question Header */}
                <div className="flex items-center justify-between mb-4">
                  <div className="flex items-center gap-3">
                    <span className="text-lg font-bold text-blue-600">Câu {currentQ + 1}/{questions.length}</span>
                    <Badge className="bg-purple-50 text-purple-700 border border-purple-200 hover:bg-purple-50 text-xs px-2 py-0.5 rounded-full">
                      ID: {currentQuestion.id}
                    </Badge>
                    <Badge className="bg-blue-50 text-blue-700 border border-blue-200 hover:bg-blue-50 text-xs px-2 py-0.5 rounded-full">
                      {currentQuestion.type}
                    </Badge>
                  </div>
                  <button
                    onClick={toggleFlag}
                    className={`flex items-center gap-1 text-sm px-3 py-1.5 rounded-lg border transition-colors ${
                      isFlagged ? "border-orange-300 bg-orange-50 text-orange-700" : "border-gray-200 text-gray-500 hover:bg-gray-50"
                    }`}
                  >
                    <Flag className={`h-4 w-4 ${isFlagged ? "fill-orange-500" : ""}`} />
                    Đánh dấu
                  </button>
                </div>

                {/* Question Content */}
                <h2 className="text-lg font-bold text-gray-900 mb-2">{currentQuestion.content}</h2>
                {currentQuestion.description && (
                  <p className="text-sm text-gray-500 mb-6">{currentQuestion.description}</p>
                )}

                {/* Answer Options */}
                <RadioGroup
                  value={answers[currentQuestion.numericId]?.toString()}
                  onValueChange={(v) => handleSelectOption(currentQuestion.numericId, parseInt(v, 10))}
                  className="space-y-3 mb-6"
                >
                  {currentQuestion.options.map((option, idx) => {
                    const isSelected = answers[currentQuestion.numericId] === idx
                    return (
                      <div
                        key={idx}
                        className={`flex items-center gap-3 p-4 rounded-lg border-2 cursor-pointer transition-colors ${
                          isSelected
                            ? "border-blue-400 bg-blue-50"
                            : "border-gray-200 hover:bg-gray-50"
                        }`}
                        onClick={() => handleSelectOption(currentQuestion.numericId, idx)}
                      >
                        <RadioGroupItem
                          value={idx.toString()}
                          id={`opt-${currentQuestion.id}-${idx}`}
                          className="sr-only"
                        />
                        <div className={`w-5 h-5 rounded-full border-2 flex items-center justify-center flex-shrink-0 ${
                          isSelected ? "border-blue-600 bg-blue-600" : "border-gray-300"
                        }`}>
                          {isSelected && <div className="w-2 h-2 rounded-full bg-white" />}
                        </div>
                        <Label
                          htmlFor={`opt-${currentQuestion.id}-${idx}`}
                          className="flex-1 cursor-pointer text-sm font-medium"
                        >
                          {["A", "B", "C", "D"][idx]}. {option}
                        </Label>
                      </div>
                    )
                  })}
                </RadioGroup>

                {/* Navigation */}
                <div className="flex items-center justify-between pt-4 border-t border-gray-100">
                  <Button
                    variant="outline"
                    className="gap-2"
                    disabled={currentQ === 0}
                    onClick={() => setCurrentQ(c => c - 1)}
                  >
                    <ChevronLeft className="h-4 w-4" />
                    Câu trước
                  </Button>
                  {currentQ < questions.length - 1 ? (
                    <Button
                      className="gap-2 bg-blue-600 hover:bg-blue-700 text-white"
                      onClick={() => setCurrentQ(c => c + 1)}
                    >
                      Câu tiếp theo
                      <ChevronRight className="h-4 w-4" />
                    </Button>
                  ) : (
                    <Button
                      className="gap-2 bg-[#D9272B] hover:bg-[#C42226] text-white"
                      onClick={() => setShowSubmitConfirm(true)}
                    >
                      <Send className="h-4 w-4" />
                      Nộp bài
                    </Button>
                  )}
                </div>
              </CardContent>
            </Card>

            {/* Footer note */}
            <div className="flex items-center gap-2 mt-4 text-sm text-gray-500">
              <Info className="h-4 w-4" />
              Lưu ý: Bài thi sẽ được tự động nộp khi hết thời gian.
            </div>
          </div>

          {/* Right Sidebar */}
          <div className="w-[300px] flex-shrink-0 space-y-4">
            {/* Question Navigator */}
            <Card>
              <CardHeader className="pb-3">
                <CardTitle className="text-base">Danh sách câu hỏi</CardTitle>
              </CardHeader>
              <CardContent>
                {/* Legend */}
                <div className="grid grid-cols-2 gap-2 mb-4 text-xs">
                  <div className="flex items-center gap-1.5">
                    <div className="w-3 h-3 rounded bg-green-200 border border-green-400" />
                    <span className="text-gray-600">Đã trả lời</span>
                  </div>
                  <div className="flex items-center gap-1.5">
                    <div className="w-3 h-3 rounded bg-blue-50 border-2 border-blue-500" />
                    <span className="text-gray-600">Đang làm</span>
                  </div>
                  <div className="flex items-center gap-1.5">
                    <div className="w-3 h-3 rounded bg-white border border-gray-200" />
                    <span className="text-gray-600">Chưa làm</span>
                  </div>
                  <div className="flex items-center gap-1.5">
                    <div className="w-3 h-3 rounded bg-orange-50 border-2 border-orange-400" />
                    <span className="text-gray-600">Đã đánh dấu</span>
                  </div>
                </div>

                {/* Grid */}
                <div className="grid grid-cols-6 gap-1.5">
                  {questions.map((q, i) => {
                    const status = getQuestionStatus(q, i)
                    return (
                      <button
                        key={q.id}
                        onClick={() => setCurrentQ(i)}
                        className={`w-full h-9 flex items-center justify-center rounded-lg text-xs font-medium transition-colors relative ${getStatusColor(status)}`}
                      >
                        {i + 1}
                        {flagged.has(q.id) && (
                          <Flag className="h-2.5 w-2.5 text-orange-500 absolute -top-0.5 -right-0.5 fill-orange-500" />
                        )}
                      </button>
                    )
                  })}
                </div>
              </CardContent>
            </Card>

            {/* Exam Info */}
            <Card>
              <CardHeader className="pb-3">
                <CardTitle className="text-base">Thông tin bài thi</CardTitle>
              </CardHeader>
              <CardContent className="space-y-3 text-sm">
                <div className="flex items-start gap-2">
                  <BookOpen className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Bài thi</div>
                    <div className="font-medium text-gray-900">{subject}</div>
                  </div>
                </div>
                <div className="flex items-start gap-2">
                  <Clock className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Thời gian làm bài</div>
                    <div className="font-medium text-gray-900">{timeLimit} phút</div>
                  </div>
                </div>
                <div className="flex items-start gap-2">
                  <ListChecks className="h-4 w-4 text-gray-400 mt-0.5 flex-shrink-0" />
                  <div>
                    <div className="text-xs text-gray-500">Số câu hỏi</div>
                    <div className="font-medium text-gray-900">{questions.length} câu</div>
                  </div>
                </div>

                {/* Progress */}
                <div className="pt-3 border-t border-gray-100">
                  <div className="flex items-center justify-between mb-1">
                    <span className="text-xs text-gray-500">Tiến độ làm bài</span>
                    <span className="text-xs font-medium text-gray-700">{answeredCount} / {questions.length} câu</span>
                  </div>
                  <div className="w-full h-2 bg-gray-200 rounded-full overflow-hidden">
                    <div className="h-full bg-blue-500 rounded-full transition-all" style={{ width: `${progress}%` }} />
                  </div>
                  <div className="text-right text-xs text-gray-500 mt-1">{Math.round(progress)}%</div>
                </div>

                {/* Submit Button */}
                <Button
                  variant="outline"
                  className="w-full h-11 border-red-300 text-red-600 hover:bg-red-50 gap-2 mt-2"
                  onClick={() => setShowSubmitConfirm(true)}
                >
                  <Send className="h-4 w-4" />
                  Nộp bài thi
                </Button>
              </CardContent>
            </Card>
          </div>
        </div>
      </div>

      {/* Fullscreen Denied Modal */}
      {fullscreenDenied && (
        <div className="fixed inset-0 z-[9999] bg-black/80 flex items-center justify-center p-4">
          <div className="bg-white rounded-2xl max-w-md w-full p-6 space-y-4 text-center shadow-2xl">
            <div className="w-12 h-12 rounded-full bg-red-100 flex items-center justify-center mx-auto text-[#D9272B]">
              <AlertCircle className="h-6 w-6" />
            </div>
            <h3 className="text-lg font-bold text-gray-900">Yêu cầu toàn màn hình</h3>
            <p className="text-sm text-gray-600">
              Bài thi yêu cầu chế độ toàn màn hình. Vui lòng cho phép quyền truy cập để tiếp tục làm bài.
            </p>
            <Button
              onClick={retryFullscreen}
              className="w-full h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold"
            >
              Thử lại (Vào toàn màn hình)
            </Button>
          </div>
        </div>
      )}

      {/* Warning Modal */}
      {showWarning && (
        <div className="fixed inset-0 bg-black/50 z-50 flex items-center justify-center">
          <div className="bg-white rounded-xl p-6 max-w-md mx-4 shadow-2xl">
            <div className="flex items-center gap-3 mb-4">
              <div className="w-12 h-12 rounded-full bg-red-100 flex items-center justify-center">
                <AlertTriangle className="h-6 w-6 text-red-600" />
              </div>
              <div>
                <h3 className="text-lg font-bold text-gray-900">Cảnh báo vi phạm!</h3>
                <p className="text-sm text-gray-500">Lần {violations}/{MAX_VIOLATIONS}</p>
              </div>
            </div>
            <p className="text-sm text-gray-700 mb-4">
              Bạn đã rời khỏi màn hình làm bài. Đây là lần vi phạm thứ <strong>{violations}</strong>.
              {violations >= MAX_VIOLATIONS
                ? " Bài thi sẽ tự động nộp!"
                : ` Nếu vi phạm thêm ${MAX_VIOLATIONS - violations} lần nữa, bài thi sẽ tự động nộp.`}
            </p>
            {violations < MAX_VIOLATIONS && (
              <Button
                className="w-full bg-[#D9272B] hover:bg-[#C42226] text-white"
                onClick={() => {
                  document.documentElement.requestFullscreen().catch(() => {})
                  setShowWarning(false)
                }}
              >
                Quay lại làm bài
              </Button>
            )}
          </div>
        </div>
      )}

      {/* Submit Confirmation Modal */}
      {showSubmitConfirm && (
        <div className="fixed inset-0 bg-black/50 z-50 flex items-center justify-center">
          <div className="bg-white rounded-xl p-6 max-w-md mx-4 shadow-2xl">
            <h3 className="text-lg font-bold text-gray-900 mb-2">Xác nhận nộp bài?</h3>
            <p className="text-sm text-gray-600 mb-1">
              Bạn đã trả lời <strong>{answeredCount}/{questions.length}</strong> câu hỏi.
            </p>
            {answeredCount < questions.length && (
              <p className="text-sm text-amber-600 mb-4">
                Còn {questions.length - answeredCount} câu chưa trả lời!
              </p>
            )}
            <div className="flex items-center gap-3 mt-4">
              <Button variant="outline" className="flex-1" onClick={() => setShowSubmitConfirm(false)}>
                Tiếp tục làm bài
              </Button>
              <Button className="flex-1 bg-[#D9272B] hover:bg-[#C42226] text-white" onClick={handleSubmit}>
                Nộp bài
              </Button>
            </div>
          </div>
        </div>
      )}

    </div>
  )
}
