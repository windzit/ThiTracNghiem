export interface ClassItem {
  id: string
  name: string
  dept?: string
  current: number
  total: number
  status: "active" | "inactive"
}

export const mockClasses: ClassItem[] = [
  { id: "D24CQCE01-N", name: "CS202 - Cấu trúc dữ liệu và giải thuật", dept: "CNTT", current: 62, total: 70, status: "active" },
  { id: "D24CQCE02-N", name: "CS203 - Lập trình Web", dept: "CNTT", current: 58, total: 65, status: "active" },
  { id: "D24CQCE03-N", name: "CS204 - Cơ sở dữ liệu", dept: "CNTT", current: 55, total: 60, status: "active" },
  { id: "D24CQCE04-N", name: "CS205 - Mạng máy tính", dept: "CNTT", current: 50, total: 55, status: "active" },
  { id: "D23CQIN01", name: "IN101 - An toàn thông tin", dept: "CNTT", current: 45, total: 50, status: "inactive" },
]