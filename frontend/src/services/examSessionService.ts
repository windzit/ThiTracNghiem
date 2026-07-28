// Mock exam session service — chuẩn bị cho backend C++
// Sau này sẽ thay bằng HTTP calls

export type ViolationType = "EXIT_FULLSCREEN" | "SWITCH_TAB" | "WINDOW_BLUR" | "COPY" | "PASTE"

interface Violation {
  type: ViolationType
  timestamp: string
}

let violationLog: Violation[] = []
let isSessionActive = false

export const examSessionService = {
  startSession: (): void => {
    isSessionActive = true
    violationLog = []
  },

  recordViolation: (type: ViolationType): void => {
    if (!isSessionActive) return
    violationLog.push({
      type,
      timestamp: new Date().toISOString(),
    })
  },

  getViolationCount: (): number => {
    return violationLog.length
  },

  getViolations: (): Violation[] => {
    return [...violationLog]
  },

  isSessionActive: (): boolean => {
    return isSessionActive
  },

  endSession: (): void => {
    isSessionActive = false
    violationLog = []
  },

  submitExam: (): void => {
    isSessionActive = false
    violationLog = []
  },
}