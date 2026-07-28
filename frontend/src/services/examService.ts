import { api } from "@/config/api"

export interface ExamQuestion {
  id: number
  noidung: string
  A: string
  B: string
  C: string
  D: string
  selectedAnswer?: string
}

export interface StartExamResponse {
  questions: ExamQuestion[]
  socau: number
  thoigian: number
  remainingSeconds: number
  fullscreenRequired?: boolean
}

export interface ResumeExamResponse {
  masv: string
  mamh: string
  tenmh: string
  remainingSeconds: number
  thoiGianPhut: number
  thoiGianBatDau: number
  questions: ExamQuestion[]
  fullscreenRequired?: boolean
}

export interface SubmitAnswerPayload {
  id: number
  answer: string
}

export interface SubmitExamResponse {
  soDung: number
  total: number
  diem: number
  saved: boolean
}

export const examService = {
  startExam: (masv: string, mamh: string, socau: number, thoigian: number = 15, thoigianSec?: number): Promise<StartExamResponse> => {
    return api.post<StartExamResponse>("/api/exams/start", { masv, mamh, socau, thoigian, thoigianSec })
  },

  getResumeSession: async (masv: string): Promise<ResumeExamResponse | null> => {
    try {
      const res = await api.get<ResumeExamResponse>(`/api/exams/resume?masv=${encodeURIComponent(masv)}`)
      return res
    } catch {
      return null
    }
  },

  submitAnswer: async (masv: string, mamh: string, questionId: number, answer: string): Promise<void> => {
    try {
      await api.put("/api/exams/answer", { masv, mamh, questionId, answer })
    } catch (err) {
      console.error("[examService] submitAnswer error:", err)
    }
  },

  submitExam: (masv: string, mamh: string, answers: SubmitAnswerPayload[]): Promise<SubmitExamResponse> => {
    return api.post<SubmitExamResponse>("/api/exams/submit", { masv, mamh, answers })
  },
}