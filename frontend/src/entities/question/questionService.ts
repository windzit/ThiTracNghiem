import { api } from '@/config/api'
import type { Question } from '@/types'
import type { BackendQuestion, BackendSubjectWithQuestions } from '@/types/backend'
import { QuestionMapper } from '@/mappers/QuestionMapper'

export interface QuestionFilters {
  subject?: string
  status?: string
  search?: string
  mamh?: string // Backend subject code (MAMH)
}

/**
 * Normalize question response — xử lý case sensitivity
 * Backend GET trả A/B/C/D (uppercase), POST/PUT trả a/b/c/d (lowercase)
 */
function normalizeQuestion(q: any): BackendQuestion {
  return {
    id: q.id,
    noidung: q.noidung,
    A: q.A || q.a || '',
    B: q.B || q.b || '',
    C: q.C || q.c || '',
    D: q.D || q.d || '',
    dapan: q.dapan || '',
    used: q.used || false,
    deleted: q.deleted === true || q.deleted === '1' || q.deleted === 1,
  }
}

export const questionService = {
  /**
   * Lấy danh sách câu hỏi — có thể filter theo subject, status, search
   * Nếu có mamh → fetch từ 1 subject. Nếu không → fetch từ tất cả.
   */
  getQuestions: async (filters?: QuestionFilters): Promise<Question[]> => {
    // Nếu có mamh filter → fetch câu hỏi từ subject đó
    if (filters?.mamh) {
      const data: BackendSubjectWithQuestions = await api.get(`/api/subjects/${filters.mamh}`)
      const normalized = (data.questions || []).map(normalizeQuestion)
      let result = QuestionMapper.toFrontendArray(normalized, data.tenmh)

      // Apply frontend filters
      if (filters.status && filters.status !== 'all') {
        result = result.filter(q => q.status === filters.status)
      }
      if (filters.search) {
        const lower = filters.search.toLowerCase()
        result = result.filter(q =>
          q.content.toLowerCase().includes(lower) ||
          q.id.toLowerCase().includes(lower)
        )
      }
      return result
    }

    // Nếu filter theo subject name (không phải mamh)
    if (filters?.subject && filters.subject !== 'all') {
      const { subjectService } = await import('@/entities/subject/subjectService')
      const subjects = await subjectService.getAll()
      const subject = subjects.find(
        s => s.name === filters.subject || s.code === filters.subject
      )
      if (subject) {
        return questionService.getQuestions({
          ...filters,
          mamh: subject.code,
          subject: undefined,
        })
      }
      return []
    }

    // Không có filter subject → fetch từ tất cả subjects
    const { subjectService } = await import('@/entities/subject/subjectService')
    const subjects = await subjectService.getAll()
    let allQuestions: Question[] = []

    for (const subj of subjects) {
      try {
        const data: BackendSubjectWithQuestions = await api.get(`/api/subjects/${subj.code}`)
        const normalized = (data.questions || []).map(normalizeQuestion)
        const questions = QuestionMapper.toFrontendArray(normalized, data.tenmh)
        allQuestions.push(...questions)
      } catch {
        // Skip subject nếu lỗi
      }
    }

    // Apply remaining filters
    if (filters) {
      if (filters.status && filters.status !== 'all') {
        allQuestions = allQuestions.filter(q => q.status === filters.status)
      }
      if (filters.search) {
        const lower = filters.search.toLowerCase()
        allQuestions = allQuestions.filter(q =>
          q.content.toLowerCase().includes(lower) ||
          q.id.toLowerCase().includes(lower)
        )
      }
    }

    return allQuestions
  },

  getQuestionById: async (id: string): Promise<Question | undefined> => {
    // Backend không có GET single question endpoint
    // Phải search qua tất cả subjects
    const { subjectService } = await import('@/entities/subject/subjectService')
    const subjects = await subjectService.getAll()
    for (const subj of subjects) {
      try {
        const data: BackendSubjectWithQuestions = await api.get(`/api/subjects/${subj.code}`)
        const found = (data.questions || []).find(q => String(q.id) === id)
        if (found) {
          return QuestionMapper.toFrontend(normalizeQuestion(found), data.tenmh)
        }
      } catch {
        // Skip subject nếu lỗi
      }
    }
    return undefined
  },

  /**
   * Tạo câu hỏi mới
   * Backend POST /api/questions dùng lowercase a/b/c/d
   */
  createQuestion: async (data: Omit<Question, 'id'> & { mamh: string }): Promise<Question> => {
    const backendData = QuestionMapper.toBackend({ ...data, id: '0' } as Question)
    // Backend POST dùng lowercase keys cho options
    const response: any = await api.post('/api/questions', {
      mamh: data.mamh,
      noidung: backendData.noidung,
      a: backendData.A,
      b: backendData.B,
      c: backendData.C,
      d: backendData.D,
      dapan: backendData.dapan,
    })
    return QuestionMapper.toFrontend(normalizeQuestion(response), data.subject || '')
  },

  /**
   * Cập nhật câu hỏi
   * Backend PUT /api/questions/:id dùng lowercase a/b/c/d
   */
  updateQuestion: async (
    id: string,
    data: Partial<Question> & { mamh: string }
  ): Promise<{ success: boolean; data?: Question; message?: string }> => {
    try {
      const updateBody: Record<string, any> = { mamh: data.mamh }
      if (data.content) updateBody.noidung = data.content
      if (data.options) {
        updateBody.a = data.options[0]?.text || ''
        updateBody.b = data.options[1]?.text || ''
        updateBody.c = data.options[2]?.text || ''
        updateBody.d = data.options[3]?.text || ''
      }
      if (data.correctAnswer !== undefined) {
        const indexMap: Record<number, string> = { 0: 'A', 1: 'B', 2: 'C', 3: 'D' }
        updateBody.dapan = indexMap[data.correctAnswer] || 'A'
      }

      const response: any = await api.put(`/api/questions/${id}`, updateBody)
      const updated = QuestionMapper.toFrontend(normalizeQuestion(response), data.subject || '')
      return { success: true, data: updated }
    } catch (error: any) {
      return {
        success: false,
        message: error?.response?.data?.message || 'Không thể cập nhật câu hỏi'
      }
    }
  },

  /**
   * Xóa câu hỏi
   * Backend: DELETE /api/questions/:id?mamh=xxx (bắt buộc truyền mamh)
   */
  deleteQuestion: async (id: string, mamh?: string): Promise<{ success: boolean; message?: string; mode?: string }> => {
    try {
      let url = `/api/questions/${id}`
      if (mamh) {
        url += `?mamh=${encodeURIComponent(mamh)}`
      }
      const res: any = await api.delete(url)
      return { success: true, mode: res?.mode || 'hard' }
    } catch (error: any) {
      return {
        success: false,
        message: error?.response?.data?.message || 'Không thể xóa câu hỏi'
      }
    }
  },

  /**
   * Xóa hàng loạt câu hỏi (Transactional)
   * Backend: POST /api/questions/bulk-delete
   */
  bulkDeleteQuestions: async (
    items: Array<{ id: string; mamh: string }>
  ): Promise<{ success: boolean; message?: string; deletedCount?: number }> => {
    try {
      const payload = items.map(item => ({ id: Number(item.id), mamh: item.mamh }))
      const res: any = await api.post('/api/questions/bulk-delete', payload)
      return { success: true, deletedCount: res?.deletedCount || items.length }
    } catch (error: any) {
      return {
        success: false,
        message: error?.response?.data?.message || 'Không thể xóa hàng loạt câu hỏi'
      }
    }
  },
}