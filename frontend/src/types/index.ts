import type { DifficultyValue } from "@/data/difficulty"

export interface ClassItem {
  id: string
  name: string
  malop?: string
  tenlop?: string
  dept?: string
  current?: number
  total?: number
  siso?: number
  status?: "active" | "inactive"
  createdDate?: string
  updatedDate?: string
}

export interface Student {
  id: string
  ho?: string
  ten?: string
  name: string
  avatar?: string
  classCode: string
  email?: string
  phone?: string
  status?: "active" | "inactive"
  gender?: "Nam" | "Nữ"
  password?: string
  dob?: string
  examCount?: number
  scores?: Array<{ subjectCode: string; subjectName: string; score: number }>
}

export interface Subject {
  id: string
  code: string
  name: string
  questionCount: number
  createdAt: string
  status: "active" | "hidden"
  description?: string
  note?: string
}

export interface Question {
  id: string
  content: string
  subText?: string
  options: { label: string; text: string }[]
  correctAnswer: number
  subject: string
  type: "Trắc nghiệm" | "Tự luận"
  status: "used" | "unused"
  difficulty: DifficultyValue
  chapter?: string
  explanation?: string
  used: boolean
  deleted: boolean
  mamh?: string
}

export interface ScoreItem {
  masv: string
  mamh: string
  diem: number
}

export interface ExamSession {
  subjectId: string
  subjectName: string
  numQuestions: number
  timeLimit: number
  questions: Question[]
}

export interface Notification {
  id: string
  type: string
  title: string
  description: string
  time: string
  isRead: boolean
  link: string
  icon: string
}
