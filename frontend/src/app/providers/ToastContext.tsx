import React, { createContext, useContext, useState, useCallback, useRef } from "react"
import { CheckCircle2, AlertTriangle, AlertCircle, WifiOff, X, Loader2 } from "lucide-react"
import { Button } from "@/shared/ui/button"

export type ToastType = "success" | "warning" | "error" | "network"

export interface ToastItem {
  id: string
  type: ToastType
  title: string
  detail?: string
  code?: string
  priority: number
  duration?: number
  retryAction?: () => void
}

export type DialogSeverity = "normal" | "warning" | "danger"

export interface ConfirmationOptions {
  title: string
  message: string
  severity?: DialogSeverity
  confirmText?: string
  cancelText?: string
  onConfirm: () => Promise<void> | void
}

interface ToastContextType {
  showSuccess: (title: string, detail?: string) => void
  showError: (title: string, detail?: string, code?: string) => void
  showWarning: (title: string, detail?: string) => void
  showNetworkError: (title: string, detail?: string, onRetry?: () => void) => void
  confirm: (options: ConfirmationOptions) => void
}

const ToastContext = createContext<ToastContextType | null>(null)

export const useToast = () => {
  const context = useContext(ToastContext)
  if (!context) {
    throw new Error("useToast must be used within a ToastProvider")
  }
  return context
}

export const ToastProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [toasts, setToasts] = useState<ToastItem[]>([])
  const [activeDialog, setActiveDialog] = useState<ConfirmationOptions | null>(null)
  const [isDialogSubmitting, setIsDialogSubmitting] = useState(false)
  const lastToastRef = useRef<{ key: string; time: number }>({ key: "", time: 0 })

  const removeToast = useCallback((id: string) => {
    setToasts((prev) => prev.filter((t) => t.id !== id))
  }, [])

  const addToast = useCallback((toast: Omit<ToastItem, "id">) => {
    const key = `${toast.type}:${toast.title}:${toast.detail || ""}`
    const now = Date.now()
    if (lastToastRef.current.key === key && now - lastToastRef.current.time < 2000) {
      return
    }
    lastToastRef.current = { key, time: now }

    const id = Math.random().toString(36).substring(2, 9)
    const newItem: ToastItem = { ...toast, id }

    setToasts((prev) => {
      const updated = [newItem, ...prev]
      updated.sort((a, b) => a.priority - b.priority)
      return updated.slice(0, 3)
    })

    if (toast.duration && toast.duration > 0) {
      setTimeout(() => {
        removeToast(id)
      }, toast.duration)
    }
  }, [removeToast])

  const showSuccess = useCallback((title: string, detail?: string) => {
    addToast({ type: "success", title, detail, priority: 4, duration: 4000 })
  }, [addToast])

  const showError = useCallback((title: string, detail?: string, code?: string) => {
    addToast({ type: "error", title, detail, code, priority: 1, duration: 6000 })
  }, [addToast])

  const showWarning = useCallback((title: string, detail?: string) => {
    addToast({ type: "warning", title, detail, priority: 3, duration: 5000 })
  }, [addToast])

  const showNetworkError = useCallback((title: string, detail?: string, onRetry?: () => void) => {
    addToast({ type: "network", title, detail, priority: 2, duration: 0, retryAction: onRetry })
  }, [addToast])

  const confirm = useCallback((options: ConfirmationOptions) => {
    setActiveDialog(options)
  }, [])

  const handleDialogConfirm = async () => {
    if (!activeDialog) return
    setIsDialogSubmitting(true)
    try {
      await activeDialog.onConfirm()
      setActiveDialog(null)
    } catch (err) {
      console.error("[ConfirmationDialog] Error on confirm:", err)
    } finally {
      setIsDialogSubmitting(false)
    }
  }

  return (
    <ToastContext.Provider value={{ showSuccess, showError, showWarning, showNetworkError, confirm }}>
      {children}

      {/* Global Toast Container */}
      <div className="fixed top-4 right-4 z-[9999] flex flex-col gap-2 max-w-[420px] w-full pointer-events-none px-4 md:px-0">
        {toasts.map((t) => (
          <div
            key={t.id}
            aria-live={t.type === "error" || t.type === "network" ? "assertive" : "polite"}
            className={`pointer-events-auto p-4 rounded-xl border shadow-lg flex items-start gap-3 transition-all duration-200 ${
              t.type === "success"
                ? "bg-white border-green-200 text-green-900"
                : t.type === "warning"
                ? "bg-white border-amber-200 text-amber-900"
                : t.type === "network"
                ? "bg-red-900 border-red-800 text-white"
                : "bg-white border-red-200 text-red-900"
            }`}
          >
            <div className="mt-0.5 flex-shrink-0">
              {t.type === "success" && <CheckCircle2 className="h-5 w-5 text-green-500" />}
              {t.type === "warning" && <AlertTriangle className="h-5 w-5 text-amber-500" />}
              {t.type === "error" && <AlertCircle className="h-5 w-5 text-red-500" />}
              {t.type === "network" && <WifiOff className="h-5 w-5 text-white" />}
            </div>
            <div className="flex-1 space-y-1 text-sm">
              <div className="font-semibold leading-tight">{t.title}</div>
              {t.detail && <div className={`text-xs ${t.type === "network" ? "text-red-200" : "text-gray-600"}`}>{t.detail}</div>}
              {t.code && <div className="text-[11px] font-mono text-red-500 font-medium">Mã lỗi: {t.code}</div>}
              {t.retryAction && (
                <button
                  onClick={() => {
                    t.retryAction?.()
                    removeToast(t.id)
                  }}
                  className="mt-2 text-xs font-semibold px-2.5 py-1 bg-white/20 hover:bg-white/30 rounded text-white transition-colors"
                >
                  Thử lại
                </button>
              )}
            </div>
            <button
              onClick={() => removeToast(t.id)}
              aria-label="Đóng thông báo"
              className="text-gray-400 hover:text-gray-600 p-0.5 rounded-lg transition-colors flex-shrink-0"
            >
              <X className="h-4 w-4" />
            </button>
          </div>
        ))}
      </div>

      {/* Global Confirmation Modal */}
      {activeDialog && (
        <div className="fixed inset-0 z-[10000] bg-black/50 backdrop-blur-sm flex items-center justify-center p-4">
          <div
            className="bg-white rounded-2xl max-w-md w-full p-6 space-y-4 shadow-xl border border-gray-100"
            onKeyDown={(e) => {
              if (e.key === "Escape" && !isDialogSubmitting) setActiveDialog(null)
            }}
          >
            <div className="flex items-center gap-3">
              <div
                className={`w-10 h-10 rounded-full flex items-center justify-center flex-shrink-0 ${
                  activeDialog.severity === "danger"
                    ? "bg-red-50 text-red-600"
                    : activeDialog.severity === "warning"
                    ? "bg-amber-50 text-amber-600"
                    : "bg-blue-50 text-blue-600"
                }`}
              >
                <AlertTriangle className="h-5 w-5" />
              </div>
              <h3 className="text-lg font-bold text-gray-900">{activeDialog.title}</h3>
            </div>
            <p className="text-sm text-gray-600 leading-relaxed">{activeDialog.message}</p>

            <div className="flex items-center justify-end gap-3 pt-2">
              <Button
                variant="outline"
                disabled={isDialogSubmitting}
                onClick={() => setActiveDialog(null)}
                className="h-10 px-4"
              >
                {activeDialog.cancelText || "Hủy"}
              </Button>
              <Button
                disabled={isDialogSubmitting}
                onClick={handleDialogConfirm}
                className={`h-10 px-4 font-semibold text-white ${
                  activeDialog.severity === "danger"
                    ? "bg-[#D9272B] hover:bg-[#C42226]"
                    : activeDialog.severity === "warning"
                    ? "bg-amber-600 hover:bg-amber-700"
                    : "bg-blue-600 hover:bg-blue-700"
                }`}
              >
                {isDialogSubmitting && <Loader2 className="h-4 w-4 mr-2 animate-spin" />}
                {activeDialog.confirmText || "Xác nhận"}
              </Button>
            </div>
          </div>
        </div>
      )}
    </ToastContext.Provider>
  )
}
