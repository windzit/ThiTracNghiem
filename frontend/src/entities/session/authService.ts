import { api } from '@/shared/api/api'

export interface User {
  id: string
  name: string
  role: "student" | "teacher"
  classCode?: string
}

export interface LoginResult {
  user: User | null
  message?: string
}

let currentUser: User | null = (() => {
  try {
    const saved = localStorage.getItem("current_user")
    return saved ? JSON.parse(saved) : null
  } catch {
    return null
  }
})()

export const authService = {
  loginStudent: async (studentId: string, password: string): Promise<LoginResult> => {
    try {
      const payload = {
        username: studentId,
        password,
        role: 'student',
      }
      const data: any = await api.post('/api/login', payload)

      let classCode = ''
      try {
        const studentDetail: any = await api.get(`/api/students/${data.masv}`)
        classCode = studentDetail.malop || ''
      } catch (err) {
        console.warn(`[authService] class fetch warning:`, err)
      }

      currentUser = {
        id: data.masv,
        name: `${data.ho} ${data.ten}`,
        role: 'student',
        classCode,
      }
      localStorage.setItem("current_user", JSON.stringify(currentUser))
      return { user: currentUser }
    } catch (error: any) {
      const errorData = error?.response?.data
      return {
        user: null,
        message: errorData?.message || 'Tên đăng nhập hoặc mật khẩu không đúng'
      }
    }
  },

  loginTeacher: async (username: string, password: string): Promise<LoginResult> => {
    try {
      const payload = {
        username,
        password,
        role: 'teacher',
      }
      const data: any = await api.post('/api/login', payload)

      currentUser = {
        id: data.username || username,
        name: data.username || username,
        role: 'teacher',
      }
      localStorage.setItem("current_user", JSON.stringify(currentUser))
      return { user: currentUser }
    } catch (error: any) {
      const errorData = error?.response?.data
      return {
        user: null,
        message: errorData?.message || 'Tên đăng nhập hoặc mật khẩu không đúng'
      }
    }
  },

  logout: async (): Promise<boolean> => {
    if (currentUser) {
      try {
        await api.post('/api/logout', { userId: currentUser.id, role: currentUser.role })
      } catch (err: any) {
        if (err?.response?.status === 403) {
          console.warn("[authService] Logout forbidden while exam is in progress")
          return false
        }
      }
    }
    currentUser = null
    localStorage.removeItem("current_user")
    return true
  },

  getCurrentUser: (): User | null => {
    return currentUser
  },

  isAuthenticated: (): boolean => {
    return currentUser !== null
  },
}