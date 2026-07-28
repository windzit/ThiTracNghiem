import { api } from '@/config/api'
import type { Student } from '@/types'
import type { BackendStudent } from '@/types/backend'
import { StudentMapper } from '@/mappers/StudentMapper'

export const studentService = {
  getAll: async (): Promise<Student[]> => {
    // Backend không có GET all students endpoint
    // Fetch tất cả classes trước, rồi fetch students từng class
    const { classService } = await import('./classService')
    const classes = await classService.getAll()
    const allStudents: Student[] = []
    for (const cls of classes) {
      try {
        const students = await studentService.getByClass(cls.id)
        allStudents.push(...students)
      } catch {
        // Skip class nếu lỗi
      }
    }
    return allStudents
  },

  getById: async (id: string): Promise<Student | undefined> => {
    try {
      const data: BackendStudent = await api.get(`/api/students/${id}`)
      return StudentMapper.toFrontend(data)
    } catch {
      return undefined
    }
  },

  getByClass: async (classCode: string): Promise<Student[]> => {
    const data: BackendStudent[] = await api.get('/api/students', { malop: classCode })
    // Backend GET /api/students?malop=xxx không trả malop trong mỗi student
    // Inject malop thủ công vì ta biết classCode từ request
    const withClass = data.map(s => ({ ...s, malop: s.malop || classCode }))
    return StudentMapper.toFrontendArray(withClass)
  },

  search: async (term: string): Promise<Student[]> => {
    // Backend không có search API → filter ở frontend
    const all = await studentService.getAll()
    const lower = term.toLowerCase()
    return all.filter(s =>
      s.id.toLowerCase().includes(lower) ||
      s.name.toLowerCase().includes(lower) ||
      s.classCode.toLowerCase().includes(lower)
    )
  },

  createStudent: async (classCode: string, data: Omit<Student, 'classCode'>): Promise<void> => {
    let ho = data.ho || ''
    let ten = data.ten || ''
    if (!ho && !ten && data.name) {
      const nameParts = data.name.trim().split(' ')
      ten = nameParts.pop() || ''
      ho = nameParts.join(' ')
    }

    await api.post('/api/students', {
      malop: classCode,
      masv: data.id,
      ho,
      ten,
      phai: data.gender || 'Nam',
      password: data.password || '123456',
    })
  },

  updateStudent: async (id: string, data: Partial<Student>): Promise<void> => {
    const updateBody: Record<string, string> = {}
    if (data.ho !== undefined) updateBody.ho = data.ho
    if (data.ten !== undefined) updateBody.ten = data.ten
    if (data.ho === undefined && data.ten === undefined && data.name) {
      const nameParts = data.name.trim().split(' ')
      updateBody.ten = nameParts.pop() || ''
      updateBody.ho = nameParts.join(' ')
    }
    if (data.gender) updateBody.phai = data.gender
    if (data.password) updateBody.password = data.password

    await api.put(`/api/students/${id}`, updateBody)
  },

  deleteStudent: async (id: string): Promise<void> => {
    await api.delete(`/api/students/${id}`)
  },

  bulkDeleteStudents: async (ids: string[]): Promise<void> => {
    await api.post('/api/students/bulk-delete', { masv: ids })
  },
}
