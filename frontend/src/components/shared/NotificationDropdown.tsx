import { useState, useRef, useEffect } from "react"
import { useNavigate } from "react-router-dom"
import { Bell, ArrowRight } from "lucide-react"
import type { Notification } from "@/types"
import { NotificationList } from "./NotificationList"

interface NotificationDropdownProps {
  items: Notification[]
  unreadCount: number
  onViewAll?: () => void
  onItemClick?: (notification: Notification) => void
}

export function NotificationDropdown({ items, unreadCount, onViewAll, onItemClick }: NotificationDropdownProps) {
  const [isOpen, setIsOpen] = useState(false)
  const dropdownRef = useRef<HTMLDivElement>(null)
  const navigate = useNavigate()

  // Close dropdown when clicking outside
  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (dropdownRef.current && !dropdownRef.current.contains(event.target as Node)) {
        setIsOpen(false)
      }
    }
    document.addEventListener("mousedown", handleClickOutside)
    return () => document.removeEventListener("mousedown", handleClickOutside)
  }, [])

  const handleItemClick = (notification: Notification) => {
    onItemClick?.(notification)
    setIsOpen(false)
    if (notification.link) {
      navigate(notification.link)
    }
  }

  const displayItems = items.slice(0, 3)

  return (
    <div className="relative" ref={dropdownRef}>
      <button
        onClick={() => setIsOpen(!isOpen)}
        className="relative p-2 rounded-lg hover:bg-gray-100 transition-colors"
      >
        <Bell className="h-5 w-5 text-gray-500" />
        {unreadCount > 0 && (
          <span className="absolute -top-0.5 -right-0.5 w-4 h-4 bg-[#D9272B] text-white text-[10px] font-bold rounded-full flex items-center justify-center">
            {unreadCount > 9 ? "9+" : unreadCount}
          </span>
        )}
      </button>

      {isOpen && (
        <div className="absolute right-0 top-full mt-2 w-80 bg-white rounded-xl border border-gray-200 shadow-lg z-50 overflow-hidden">
          <div className="px-4 py-3 border-b border-gray-100">
            <h3 className="text-sm font-bold text-gray-900">Thông báo</h3>
          </div>

          <div className="max-h-80 overflow-y-auto">
            {displayItems.length > 0 ? (
              <NotificationList items={displayItems} onItemClick={handleItemClick} />
            ) : (
              <div className="py-8 text-center text-sm text-gray-400">
                Không có thông báo mới
              </div>
            )}
          </div>

          <button
            onClick={() => {
              onViewAll?.()
              setIsOpen(false)
            }}
            className="w-full px-4 py-3 border-t border-gray-100 text-sm text-[#D9272B] font-medium flex items-center justify-center gap-1 hover:bg-gray-50 transition-colors"
          >
            Xem tất cả <ArrowRight className="h-3.5 w-3.5" />
          </button>
        </div>
      )}
    </div>
  )
}