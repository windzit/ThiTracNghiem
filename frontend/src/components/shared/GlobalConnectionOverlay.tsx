import React, { useState } from "react"
import { useConnection } from "@/context/ConnectionContext"
import { WifiOff, RefreshCw } from "lucide-react"
import { Button } from "@/components/ui/button"

export const GlobalConnectionOverlay: React.FC = () => {
  const { isDisconnected, retryConnection } = useConnection()
  const [isRetrying, setIsRetrying] = useState(false)
  const [retryFailed, setRetryFailed] = useState(false)

  if (!isDisconnected) return null

  const handleRetry = async () => {
    setIsRetrying(true)
    setRetryFailed(false)
    const success = await retryConnection()
    setIsRetrying(false)
    if (!success) {
      setRetryFailed(true)
    }
  }

  return (
    <div
      className="fixed inset-0 z-[99999] bg-black/80 backdrop-blur-md flex flex-col items-center justify-center p-6 text-white select-none pointer-events-auto"
      style={{ isolation: "isolate" }}
    >
      <div className="bg-gray-900/95 border border-gray-700 rounded-2xl p-8 max-w-md w-full text-center space-y-6 shadow-2xl animate-in fade-in zoom-in duration-200">
        <div className="w-16 h-16 rounded-2xl bg-red-500/20 border border-red-500/30 flex items-center justify-center mx-auto">
          <WifiOff className="h-8 w-8 text-red-500" />
        </div>

        <div className="space-y-2">
          <h2 className="text-xl font-bold text-white">Mất kết nối với máy chủ</h2>
          <p className="text-sm text-gray-300">
            Hệ thống tạm dừng thao tác để bảo vệ dữ liệu. Vui lòng bấm "Thử lại kết nối" sau khi khởi động lại máy chủ.
          </p>
        </div>

        {retryFailed && (
          <div className="text-xs font-semibold text-red-400 bg-red-500/10 border border-red-500/20 py-2.5 px-4 rounded-xl animate-shake">
            Không thể kết nối đến máy chủ. Vui lòng kiểm tra server.exe.
          </div>
        )}

        <Button
          onClick={handleRetry}
          disabled={isRetrying}
          className="w-full h-12 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold rounded-xl flex items-center justify-center gap-2 shadow-lg transition-all active:scale-[0.98]"
        >
          <RefreshCw className={`h-5 w-5 ${isRetrying ? "animate-spin" : ""}`} />
          {isRetrying ? "Đang kiểm tra kết nối..." : "Thử lại kết nối"}
        </Button>
      </div>
    </div>
  )
}
