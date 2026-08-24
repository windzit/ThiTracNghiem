import React, { createContext, useContext, useState, useEffect, useCallback } from "react"
import { authService } from "@/entities/session/authService"
import { examService, type ResumeExamResponse } from "@/entities/exam/examService"

interface ExamSessionContextType {
  activeSession: ResumeExamResponse | null
  hasActiveExam: boolean
  refreshSession: () => Promise<ResumeExamResponse | null>
  clearSession: () => void
}

const ExamSessionContext = createContext<ExamSessionContextType>({
  activeSession: null,
  hasActiveExam: false,
  refreshSession: async () => null,
  clearSession: () => {},
})

export const ExamSessionProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [activeSession, setActiveSession] = useState<ResumeExamResponse | null>(null)

  const refreshSession = useCallback(async (): Promise<ResumeExamResponse | null> => {
    const user = authService.getCurrentUser()
    if (!user || !user.id || user.role !== "student") {
      setActiveSession(null)
      return null
    }

    try {
      const session = await examService.getResumeSession(user.id)
      if (session && session.remainingSeconds > 0) {
        setActiveSession(session)
        return session
      }
      setActiveSession(null)
      return null
    } catch {
      setActiveSession(null)
      return null
    }
  }, [])

  const clearSession = useCallback(() => {
    setActiveSession(null)
  }, [])

  useEffect(() => {
    refreshSession()
  }, [refreshSession])

  const hasActiveExam = Boolean(activeSession && activeSession.remainingSeconds > 0)

  return (
    <ExamSessionContext.Provider
      value={{
        activeSession,
        hasActiveExam,
        refreshSession,
        clearSession,
      }}
    >
      {children}
    </ExamSessionContext.Provider>
  )
}

export const useExamSession = () => useContext(ExamSessionContext)
