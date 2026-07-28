import { api } from '@/config/api'
import type { Subject } from '@/types'
import type { BackendSubject, BackendSubjectWithQuestions } from '@/types/backend'
import { SubjectMapper } from '@/mappers/SubjectMapper'

export const subjectService = {
  getAll: async (): Promise<Subject[]> => {
    const data: BackendSubject[] = await api.get('/api/subjects')
    return SubjectMapper.toFrontendArray(data)
  },

  getById: async (id: string): Promise<Subject | undefined> => {
    try {
      const data: BackendSubjectWithQuestions = await api.get(`/api/subjects/${id}`)
      const subject = SubjectMapper.toFrontend(data)
      // GET /api/subjects/:id cũng trả questions → lấy questionCount
      subject.questionCount = data.questions?.length || 0
      return subject
    } catch {
      return undefined
    }
  },

  getAvailable: async (): Promise<Subject[]> => {
    const all = await subjectService.getAll()
    return all.filter(s => s.status === 'active')
  },

  search: async (term: string): Promise<Subject[]> => {
    // Backend không có search API → filter ở frontend
    const all = await subjectService.getAll()
    const lower = term.toLowerCase()
    return all.filter(s =>
      s.code.toLowerCase().includes(lower) ||
      s.name.toLowerCase().includes(lower)
    )
  },

  createSubject: async (data: { mamh: string; tenmh: string }): Promise<void> => {
    await api.post('/api/subjects', data)
  },

  updateSubject: async (id: string, data: { tenmh: string }): Promise<void> => {
    await api.put(`/api/subjects/${id}`, data)
  },

  deleteSubject: async (id: string): Promise<void> => {
    await api.delete(`/api/subjects/${id}`)
  },
}