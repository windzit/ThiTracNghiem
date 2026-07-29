import { reportService } from "@/entities/report/reportService"

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
  getStudentStats: async (malop: string, masv: string): Promise<StudentStats> => {
    try {
      const res = await reportService.getScoreboard(malop)
      const student = res.students?.find((s) => s.masv === masv)
      if (!student || !student.scores) {
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

      const entries = Object.entries(student.scores)
      const totalExams = entries.length
      const taken = entries.filter(([_, score]) => score !== null) as [string, number][]
      const takenExams = taken.length
      const passedExams = taken.filter(([_, s]) => s >= 5.0).length
      const failedExams = takenExams - passedExams
      const notTaken = totalExams - takenExams
      const sum = taken.reduce((acc, [_, s]) => acc + s, 0)
      const avg = takenExams > 0 ? sum / takenExams : 0

      const scoresList = taken.map(([mamh, score]) => ({ mamh, score }))
      const chartData = [
        { name: "Đạt (≥5.0)", value: passedExams, color: "#10B981" },
        { name: "Chưa đạt (<5.0)", value: failedExams, color: "#EF4444" },
      ]

      return {
        totalExams,
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