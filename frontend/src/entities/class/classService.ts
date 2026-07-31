import { api } from '@/shared/api/api'
import type { ClassItem } from '@/shared/types'
import type { BackendClass } from '@/shared/types/backend'
import { ClassMapper } from '@/entities/class/ClassMapper'

export const classService = {
  getAll: async (): Promise<ClassItem[]> => {
    const data: BackendClass[] = await api.get('/api/classes')
    return ClassMapper.toFrontendArray(data)
  },

  getById: async (id: string): Promise<ClassItem | undefined> => {
    try {
      const data: BackendClass = await api.get(`/api/classes/${id}`)
      return ClassMapper.toFrontend(data)
    } catch {
      return undefined
    }
  },

  search: async (term: string): Promise<ClassItem[]> => {
    // Backend không có search API → filter ở frontend
    const all = await classService.getAll()
    const lower = term.toLowerCase()
    return all.filter(c =>
      c.id.toLowerCase().includes(lower) ||
      c.name.toLowerCase().includes(lower)
    )
  },

  createClass: async (classData: ClassItem): Promise<void> => {
    const backend = ClassMapper.toBackend(classData);
    await api.post('/api/classes', {
      malop: backend.malop,
      tenlop: backend.tenlop,
    })
  },

  updateClass: async (id: string, classData: Partial<ClassItem>): Promise<void> => {
    const name = classData.name ? classData.name.trim() : '';
    await api.put(`/api/classes/${id}`, {
      tenlop: name,
    })
  },

  deleteClass: async (id: string): Promise<void> => {
    await api.delete(`/api/classes/${id}`)
  },
}
