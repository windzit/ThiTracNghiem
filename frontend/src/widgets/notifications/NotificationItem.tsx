import {
  FileText,
  ClipboardList,
  CheckCircle2,
  UserPlus,
  HelpCircle,
  TrendingUp,
  BookOpen,
} from "lucide-react"
import type { Notification } from "@/shared/types"

// Map icon name → Lucide icon component
const iconMap: Record<string, React.ComponentType<{ className?: string }>> = {
  FileText,
  ClipboardList,
  CheckCircle2,
  UserPlus,
  HelpCircle,
  TrendingUp,
  BookOpen,
}

// Map notification type → icon color
const typeIconStyles: Record<string, { bg: string; color: string }> = {
  RESULT_PUBLISHED: { bg: "bg-blue-50", color: "text-[#3B82F6]" },
  EXAM_CREATED: { bg: "bg-purple-50", color: "text-[#8B5CF6]" },
  RESULT_PUBLISHED_TEACHER: { bg: "bg-green-50", color: "text-[#10B981]" },
  STUDENT_ADDED: { bg: "bg-orange-50", color: "text-[#F59E0B]" },
  SUBJECT_ADDED: { bg: "bg-green-50", color: "text-[#10B981]" },
  QUESTION_ADDED: { bg: "bg-blue-50", color: "text-[#3B82F6]" },
  REPORT_EXPORTED: { bg: "bg-red-50", color: "text-[#D9272B]" },
}

interface NotificationItemProps {
  notification: Notification
  onClick?: (notification: Notification) => void
}

export function NotificationItem({ notification, onClick }: NotificationItemProps) {
  const IconComponent = iconMap[notification.icon] ?? FileText
  const iconStyle = typeIconStyles[notification.type] ?? { bg: "bg-gray-50", color: "text-gray-500" }

  return (
    <button
      onClick={() => onClick?.(notification)}
      className={`w-full flex items-start gap-3 py-3 px-4 text-left transition-colors hover:bg-gray-50 ${
        !notification.isRead ? "bg-blue-50/30" : ""
      }`}
    >
      <div className={`w-9 h-9 rounded-lg ${iconStyle.bg} flex items-center justify-center flex-shrink-0`}>
        <IconComponent className={`h-4.5 w-4.5 ${iconStyle.color}`} />
      </div>
      <div className="flex-1 min-w-0">
        <div className="text-[13px] font-medium text-gray-900 leading-snug">{notification.title}</div>
        <div className="text-xs text-gray-400 mt-0.5">{notification.description}</div>
      </div>
      <div className="text-xs text-gray-400 flex-shrink-0 text-right whitespace-nowrap ml-2">
        {notification.time}
      </div>
      {!notification.isRead && (
        <div className="w-2 h-2 bg-[#D9272B] rounded-full flex-shrink-0 mt-1.5" />
      )}
    </button>
  )
}