import { api } from "@/config/api"

export interface ScoreboardStudent {
  masv: string
  ho: string
  ten: string
  examCount?: number
  scores: Record<string, number | null>
}

export interface ScoreboardResponse {
  malop: string
  students: ScoreboardStudent[]
}

export interface ExamReportQuestion {
  id: number
  noidung: string
  A: string
  B: string
  C: string
  D: string
  dapan: string
  selectedAnswer?: string
}

export interface ExamReportResponse {
  masv: string
  ho: string
  ten: string
  phai: string
  mamh: string
  tenmh: string
  diem: number
  questions: ExamReportQuestion[]
}

export const reportService = {
  getScoreboard: (malop: string): Promise<ScoreboardResponse> => {
    return api.get<ScoreboardResponse>("/api/reports/scoreboard", { malop })
  },

  getExamReport: (malop: string, masv: string, mamh: string): Promise<ExamReportResponse> => {
    return api.get<ExamReportResponse>("/api/reports/exam", { malop, masv, mamh })
  },

  deleteScore: (masv: string, mamh: string, malop?: string): Promise<void> => {
    return api.delete("/api/scores", { masv, mamh, malop })
  },
}
