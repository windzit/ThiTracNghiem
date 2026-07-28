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

export const mockSubjects: Subject[] = [
  { id: "1", code: "CTDLGT", name: "Cấu trúc dữ liệu và giải thuật", questionCount: 156, createdAt: "10/06/2026", status: "active" },
  { id: "2", code: "LTWEB", name: "Lập trình Web", questionCount: 98, createdAt: "08/06/2026", status: "active" },
  { id: "3", code: "CSDL", name: "Cơ sở dữ liệu", questionCount: 132, createdAt: "05/06/2026", status: "active" },
  { id: "4", code: "HDH", name: "Hệ điều hành", questionCount: 87, createdAt: "03/06/2026", status: "active" },
  { id: "5", code: "MANGMT", name: "Mạng máy tính", questionCount: 113, createdAt: "01/06/2026", status: "active" },
  { id: "6", code: "TTRR", name: "Toán rời rạc", questionCount: 74, createdAt: "28/05/2026", status: "active" },
  { id: "7", code: "LTVAN", name: "Lập trình hướng đối tượng", questionCount: 101, createdAt: "25/05/2026", status: "active" },
  { id: "8", code: "AI", name: "Trí tuệ nhân tạo", questionCount: 65, createdAt: "20/05/2026", status: "hidden" },
  { id: "9", code: "TKCSDL", name: "Thiết kế cơ sở dữ liệu", questionCount: 56, createdAt: "18/05/2026", status: "hidden" },
  { id: "10", code: "ATTT", name: "An toàn thông tin", questionCount: 79, createdAt: "15/05/2026", status: "active" },
]