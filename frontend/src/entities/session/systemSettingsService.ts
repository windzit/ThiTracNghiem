import { api } from "@/shared/api/api"

export interface SystemSettings {
  fullscreenRequired: boolean
}

export const systemSettingsService = {
  getSettings: async (): Promise<SystemSettings> => {
    return await api.get<SystemSettings>('/api/system/settings')
  },

  updateSettings: async (required: boolean): Promise<SystemSettings> => {
    return await api.post<SystemSettings>('/api/system/settings', {
      fullscreenRequired: required,
    })
  },
}