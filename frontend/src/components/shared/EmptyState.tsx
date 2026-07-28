import React from 'react'
import { FolderOpen } from 'lucide-react'

interface EmptyStateProps {
  icon?: React.ReactNode
  title: string
  description?: string
  action?: React.ReactNode
  actionLabel?: string
  onAction?: () => void
  className?: string
}

export const EmptyState: React.FC<EmptyStateProps> = ({
  icon,
  title,
  description,
  action,
  actionLabel,
  onAction,
  className = '',
}) => {
  return (
    <div className={`flex flex-col items-center justify-center p-8 text-center bg-gray-50/50 rounded-xl border border-dashed border-gray-200 ${className}`}>
      <div className="w-12 h-12 rounded-full bg-gray-100 flex items-center justify-center text-gray-400 mb-3">
        {icon || <FolderOpen className="w-6 h-6" />}
      </div>
      <h4 className="text-base font-semibold text-gray-800 mb-1">{title}</h4>
      {description && <p className="text-sm text-gray-500 max-w-sm mb-4">{description}</p>}
      {action && <div>{action}</div>}
      {actionLabel && (
        <button
          onClick={onAction}
          className="px-4 py-2 text-sm font-medium text-white bg-[#D9272B] hover:bg-[#C42226] rounded-lg transition-colors"
        >
          {actionLabel}
        </button>
      )}
    </div>
  )
}
