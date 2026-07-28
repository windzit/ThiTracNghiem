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

const firstNames = ["Nguyễn", "Trần", "Lê", "Phạm", "Hoàng", "Vũ", "Đỗ", "Bùi", "Ngô", "Phan", "Lý", "Đặng", "Trịnh", "Võ"]
const middleNames = ["Văn", "Thị", "Minh", "Thanh", "Quốc", "Đức", "Thùy", "Anh", "Hoàng", "Xuân", "Hữu", "Tấn", "Công", "Duy"]
const lastNames = ["An", "Bình", "Chi", "Dũng", "Hải", "Hòa", "Huy", "Hương", "Khoa", "Lan", "Long", "Mai", "Nam", "Phúc", "Quân", "Thảo", "Tùng", "Vinh", "Yến", "Triết", "Thắng", "Ngọc", "Hiếu", "Phương"]

export const mockStudents: Student[] = [
  { id: "N24DECE046", name: "Đỗ Minh Triết", avatar: "ĐT", classCode: "D24CQCE01-N", email: "n24dece046@ptit.edu.vn", phone: "0398 123 456", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE028", name: "Lê Thành Long", avatar: "LL", classCode: "D24CQCE01-N", email: "n24dece028@ptit.edu.vn", phone: "0398 234 567", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE015", name: "Nguyễn Văn An", avatar: "NA", classCode: "D24CQCE01-N", email: "n24dece015@ptit.edu.vn", phone: "0398 345 678", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE037", name: "Trần Thị Mai", avatar: "TM", classCode: "D24CQCE02-N", email: "n24dece037@ptit.edu.vn", phone: "0398 456 789", status: "active", gender: "Nữ", password: "123456" },
  { id: "N24DECE077", name: "Phạm Quốc Huy", avatar: "QH", classCode: "D24CQCE02-N", email: "n24dece077@ptit.edu.vn", phone: "0398 567 890", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE091", name: "Vũ Thùy Linh", avatar: "TL", classCode: "D24CQCE02-N", email: "n24dece091@ptit.edu.vn", phone: "0398 678 901", status: "active", gender: "Nữ", password: "123456" },
  { id: "N24DECE103", name: "Hoàng Đức Duy", avatar: "DD", classCode: "D24CQCE03-N", email: "n24dece103@ptit.edu.vn", phone: "0398 789 012", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE112", name: "Bùi Anh Khoa", avatar: "AK", classCode: "D24CQCE03-N", email: "n24dece112@ptit.edu.vn", phone: "0398 890 123", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE125", name: "Ngô Minh Quân", avatar: "MQ", classCode: "D24CQCE03-N", email: "n24dece125@ptit.edu.vn", phone: "0398 901 234", status: "active", gender: "Nam", password: "123456" },
  { id: "N24DECE138", name: "Phan Thế Anh", avatar: "TA", classCode: "D24CQCE04-N", email: "n24dece138@ptit.edu.vn", phone: "0398 012 345", status: "active", gender: "Nam", password: "123456" },
]

export function generateStudents(count: number = 128): Student[] {
  const classes = ["D24CQCE01-N", "D24CQCE02-N", "D24CQCE03-N", "D24CQCE04-N"]
  const students: Student[] = []
  const usedNames = new Set<string>()

  for (let i = 1; i <= count; i++) {
    let name = ""
    do {
      const fn = firstNames[Math.floor(Math.random() * firstNames.length)]
      const mn = middleNames[Math.floor(Math.random() * middleNames.length)]
      const ln = lastNames[Math.floor(Math.random() * lastNames.length)]
      name = `${fn} ${mn} ${ln}`
    } while (usedNames.has(name))
    usedNames.add(name)

    const classIdx = Math.floor((i - 1) / 32)
    const gender = Math.random() > 0.4 ? "Nam" : "Nữ"

    students.push({
      id: `N24DECE${String(i).padStart(3, "0")}`,
      name,
      avatar: name.split(" ").map(n => n[0]).slice(-2).join(""),
      classCode: classes[classIdx % 4],
      email: `n24dece${String(i).padStart(3, "0")}@ptit.edu.vn`,
      phone: `0398 ${String(Math.floor(Math.random() * 1000)).padStart(3, "0")} ${String(Math.floor(Math.random() * 1000)).padStart(3, "0")}`,
      status: Math.random() > 0.05 ? "active" : "inactive",
      gender,
      password: "123456",
    })
  }

  // Override first 10 with fixed data
  for (let i = 0; i < Math.min(10, mockStudents.length); i++) {
    students[i] = { ...mockStudents[i] }
  }

  return students
}