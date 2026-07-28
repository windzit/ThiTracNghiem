import { Component, type ErrorInfo, type ReactNode } from "react"
import { AlertOctagon, RotateCcw } from "lucide-react"

interface Props {
  children: ReactNode
}

interface State {
  hasError: boolean
  error: Error | null
}

export class RootErrorBoundary extends Component<Props, State> {
  public state: State = {
    hasError: false,
    error: null,
  }

  public static getDerivedStateFromError(error: Error): State {
    return { hasError: true, error }
  }

  public componentDidCatch(error: Error, errorInfo: ErrorInfo) {
    console.error("[RootErrorBoundary] Caught unhandled React error:", error, errorInfo)
  }

  componentDidMount() {
    window.onerror = (message, source, lineno, colno, error) => {
      console.error("[Global window.onerror]", { message, source, lineno, colno, error })
      return true
    }

    window.onunhandledrejection = (event) => {
      console.error("[Global window.onunhandledrejection]", event.reason)
      event.preventDefault()
    }
  }

  private handleReload = () => {
    window.location.reload()
  }

  public render() {
    if (this.state.hasError) {
      return (
        <div className="min-h-screen bg-[#F9FAFB] flex flex-col items-center justify-center p-6 text-center">
          <div className="bg-white rounded-2xl border border-gray-200 p-8 max-w-md w-full space-y-4 shadow-sm">
            <div className="w-12 h-12 rounded-full bg-red-50 text-red-500 flex items-center justify-center mx-auto">
              <AlertOctagon className="h-6 w-6 text-red-500" />
            </div>
            <h2 className="text-lg font-bold text-gray-900">Đã xảy ra lỗi hệ thống</h2>
            <p className="text-sm text-gray-500">
              {this.state.error?.message || "Đã xảy ra sự cố không mong muốn trong ứng dụng."}
            </p>
            <button
              onClick={this.handleReload}
              className="w-full h-11 bg-[#D9272B] hover:bg-[#C42226] text-white font-semibold rounded-xl flex items-center justify-center gap-2 text-sm"
            >
              <RotateCcw className="h-4 w-4" /> Tải lại ứng dụng
            </button>
          </div>
        </div>
      )
    }

    return this.props.children
  }
}
