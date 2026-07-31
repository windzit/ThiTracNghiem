import { Bell } from "lucide-react"
import type { Notification } from "@/shared/types"
import { NotificationItem } from "./NotificationItem"

interface NotificationListProps {
  items: Notification[]
  onItemClick?: (notification: Notification) => void
  emptyMessage?: string
}

export function NotificationList({ items, onItemClick, emptyMessage = "Không có thông báo nào" }: NotificationListProps) {
  if (items.length === 0) {
    return (
      <div className="flex flex-col items-center justify-center py-12 text-gray-400">
        <Bell className="h-10 w-10 mb-3 text-gray-300" />
        <p className="text-sm">{emptyMessage}</p>
      </div>
    )
  }

  return (
    <div className="divide-y divide-gray-100">
      {items.map((item) => (
        <NotificationItem key={item.id} notification={item} onClick={onItemClick} />
      ))}
    </div>
  )
}