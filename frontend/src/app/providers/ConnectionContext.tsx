import React, { createContext, useContext, useState, useEffect } from "react"
import { setNetworkErrorCallback, api } from "@/shared/api/api"

interface ConnectionContextType {
  isDisconnected: boolean
  setDisconnected: (disconnected: boolean) => void
  retryConnection: () => Promise<boolean>
}

const ConnectionContext = createContext<ConnectionContextType>({
  isDisconnected: false,
  setDisconnected: () => {},
  retryConnection: async () => false,
})

export const ConnectionProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [isDisconnected, setIsDisconnected] = useState(false)

  // Register Axios interceptor callback for reactive connection detection
  useEffect(() => {
    setNetworkErrorCallback((disconnected) => {
      setIsDisconnected(disconnected)
    })
  }, [])

  const retryConnection = async (): Promise<boolean> => {
    try {
      await api.get('/')
      setIsDisconnected(false)
      return true
    } catch {
      setIsDisconnected(true)
      return false
    }
  }

  return (
    <ConnectionContext.Provider value={{ isDisconnected, setDisconnected: setIsDisconnected, retryConnection }}>
      {children}
    </ConnectionContext.Provider>
  )
}

export const useConnection = () => useContext(ConnectionContext)
