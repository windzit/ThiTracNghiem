import { studentService } from "@/entities/student/studentService"

export interface StudentStats {
  totalExams: number
  takenExams: number
  passedExams: number
  failedExams: number
  avgScore: number
  avgScoreFormatted: string
  scoresList: { mamh: string; score: number }[]
  chartData: { name: string; value: number; color: string }[]
}

export const resultService = {
  getStudentStats: async (_malop: string, masv: string): Promise<StudentStats> => {
    try {
      const student = await studentService.getById(masv)
      if (!student || !student.scores || student.scores.length === 0) {
        return {
          totalExams: 0,
          takenExams: 0,
          passedExams: 0,
          failedExams: 0,
          avgScore: 0,
          avgScoreFormatted: "0.00",
          scoresList: [],
          chartData: [
            { name: "Đạt (≥5.0)", value: 0, color: "#10B981" },
            { name: "Chưa đạt (<5.0)", value: 0, color: "#EF4444" },
          ],
        }
      }

      const scoresList = student.scores.map((s) => ({ mamh: s.subjectCode, score: s.score }))
      const takenExams = scoresList.length
      const passedExams = scoresList.filter((s) => s.score >= 5.0).length
      const failedExams = takenExams - passedExams
      const sum = scoresList.reduce((acc, s) => acc + s.score, 0)
      const avg = takenExams > 0 ? sum / takenExams : 0

      const chartData = [
        { name: "Đạt (≥5.0)", value: passedExams, color: "#10B981" },
        { name: "Chưa đạt (<5.0)", value: failedExams, color: "#EF4444" },
      ]

      return {
        totalExams: takenExams,
        takenExams,
        passedExams,
        failedExams,
        avgScore: avg,
        avgScoreFormatted: avg.toFixed(2),
        scoresList,
        chartData,
      }
    } catch (err) {
      console.error("[resultService] getStudentStats error:", err)
      return {
        totalExams: 0,
        takenExams: 0,
        passedExams: 0,
        failedExams: 0,
        avgScore: 0,
        avgScoreFormatted: "0.00",
        scoresList: [],
        chartData: [
          { name: "Đạt (≥5.0)", value: 0, color: "#10B981" },
          { name: "Chưa đạt (<5.0)", value: 0, color: "#EF4444" },
        ],
      }
    }
  },
}