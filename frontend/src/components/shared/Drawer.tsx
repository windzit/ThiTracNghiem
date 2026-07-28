import { X } from "lucide-react"
import type { ReactNode } from "react"

interface DrawerProps {
  open: boolean
  onClose: () => void
  title: string
  children: ReactNode
  footer?: ReactNode
  width?: string
}

export function Drawer({
  open,
  onClose,
  title,
  children,
  footer,
  width = "w-[420px]",
}: DrawerProps) {
  if (!open) return null

  return (
    <>
      {/* Overlay */}
      <div
        className="fixed inset-0 bg-black/30 z-40"
        onClick={onClose}
      />
      {/* Panel */}
      <div
        className={`fixed right-0 top-0 h-full ${width} bg-white shadow-2xl z-50 flex flex-col overflow-y-auto`}
      >
        {/* Header */}
        <div className="flex items-center justify-between px-6 py-5 border-b border-gray-200">
          <h2 className="text-lg font-bold text-gray-900">{title}</h2>
          <button
            className="p-1 rounded-lg hover:bg-gray-100 transition-colors text-gray-500"
            onClick={onClose}
          >
            <X className="h-5 w-5" />
          </button>
        </div>

        {/* Body */}
        <div className="flex-1 px-6 py-5 space-y-5">{children}</div>

        {/* Footer */}
        {footer && (
          <div className="px-6 py-4 border-t border-gray-200 flex items-center gap-3">
            {footer}
          </div>
        )}
      </div>
    </>
  )
}