import { useNavigate, useLocation } from "react-router-dom"
import { useState, useEffect, useRef } from "react"
import {
  Home,
  ClipboardList,
  Clock,
  User,
  LogOut,
  ChevronDown,
  Menu,
  Lock,
} from "lucide-react"
import { NotificationDropdown } from "@/widgets/notifications/NotificationDropdown"
import { authService } from "@/entities/session/authService"
import { examService } from "@/entities/exam/examService"

interface StudentLayoutProps {
  children: React.ReactNode
  breadcrumb?: { label: string; path?: string }[]
}

const menuItems = [
  { icon: Home, label: "Dashboard", path: "/student/dashboard" },
  { icon: ClipboardList, label: "Thi trắc nghiệm", path: "/student/take-exam" },
  { icon: Clock, label: "Lịch sử thi", path: "/student/history" },
]

export default function StudentLayout({ children, breadcrumb }: StudentLayoutProps) {
  const navigate = useNavigate()
  const location = useLocation()
  const [sidebarOpen, setSidebarOpen] = useState(false)
  const [isExamActive, setIsExamActive] = useState(false)

  // Check if student has an active exam in progress (to hide Logout button)
  useEffect(() => {
    const user = authService.getCurrentUser()
    if (!user || !user.id) return

    examService.getResumeSession(user.id).then((session) => {
      if (session && session.remainingSeconds > 0) {
        setIsExamActive(true)
      } else {
        setIsExamActive(false)
      }
    })
  }, [])

  const handleLogout = async () => {
    const ok = await authService.logout()
    if (!ok) {
      alert("Không thể đăng xuất khi đang làm bài thi!")
      return
    }
    navigate("/")
  }

  const isActive = (path: string) => location.pathname === path

  return (
    <div className="flex min-h-screen bg-[#F9FAFB]">
      {/* Mobile Overlay */}
      {sidebarOpen && (
        <div
          className="fixed inset-0 bg-black/30 z-30 lg:hidden"
          onClick={() => setSidebarOpen(false)}
        />
      )}

      {/* Sidebar */}
      <aside className={`w-60 bg-white border-r border-gray-200 flex flex-col fixed h-full z-40 transition-transform duration-200 lg:translate-x-0 ${sidebarOpen ? "translate-x-0" : "-translate-x-full"}`}>
        {/* Logo */}
        <div className="px-5 py-5 flex items-center gap-3">
          <img src="/logo-ptit.png" alt="PTIT" className="h-8 w-auto" />
          <div>
            <div className="text-lg font-bold text-[#D9272B]">PTIT CBT</div>
            <div className="text-[11px] text-gray-400 leading-tight">Computer Based Testing System</div>
          </div>
        </div>

        {/* Menu */}
        <nav className="flex-1 px-3 py-2 space-y-1">
          {menuItems.map((item) => {
            const Icon = item.icon
            const active = isActive(item.path)
            return (
              <button
                key={item.label}
                onClick={() => navigate(item.path)}
                className={`w-full flex items-center gap-3 px-3 py-2.5 rounded-lg text-sm font-medium transition-colors ${
                  active
                    ? "bg-[#FEF2F2] text-[#D9272B]"
                    : "text-gray-500 hover:bg-gray-50 hover:text-gray-700"
                }`}
              >
                <Icon className="h-5 w-5 flex-shrink-0" />
                {item.label}
              </button>
            )
          })}
        </nav>

        {/* Logout (Hide if Exam in progress) */}
        {!isExamActive && (
          <div className="px-3 py-4 border-t border-gray-100">
            <button
              onClick={handleLogout}
              className="w-full flex items-center gap-3 px-3 py-2.5 rounded-lg text-sm font-medium text-gray-500 hover:bg-gray-50 hover:text-gray-700 transition-colors"
            >
              <LogOut className="h-5 w-5 flex-shrink-0" />
              Đăng xuất
            </button>
          </div>
        )}
      </aside>

      {/* Main Content */}
      <div className="flex-1 lg:ml-60">
        {/* Top Bar */}
        <header className="bg-white border-b border-gray-200 px-4 lg:px-6 py-3 flex items-center justify-between sticky top-0 z-10">
          {/* Left */}
          <div className="flex items-center gap-3">
            <button
              onClick={() => setSidebarOpen(!sidebarOpen)}
              className="p-1 rounded-lg hover:bg-gray-100 transition-colors lg:hidden"
            >
              <Menu className="h-5 w-5 text-gray-500" />
            </button>
            {breadcrumb && breadcrumb.length > 0 ? (
              <div className="flex items-center gap-2 text-sm">
                {breadcrumb.map((item, i) => (
                  <span key={i} className="flex items-center gap-2">
                    {i > 0 && <span className="text-gray-300">›</span>}
                    {item.path ? (
                      <button
                        onClick={() => navigate(item.path!)}
                        className="text-gray-500 hover:text-gray-700 transition-colors"
                      >
                        {item.label}
                      </button>
                    ) : (
                      <span className="text-gray-700 font-medium">{item.label}</span>
                    )}
                  </span>
                ))}
              </div>
            ) : (
              <div className="text-base font-bold text-gray-900">Trang chủ</div>
            )}
          </div>

          {/* Right side */}
          <div className="flex items-center gap-2 lg:gap-4">
            {/* Notification Bell */}
            <NotificationDropdown
              items={[]}
              unreadCount={0}
            />

            {/* Student Info Badge (Static Vertical Info) */}
            <div className="flex items-center gap-2.5 px-3 py-1.5 rounded-lg bg-gray-50/80 border border-gray-200/60">
              <div className="w-8 h-8 lg:w-9 lg:h-9 rounded-full bg-red-50 border border-red-100 flex items-center justify-center overflow-hidden flex-shrink-0">
                <User className="h-4 w-4 lg:h-5 lg:w-5 text-[#D9272B]" />
              </div>
              <div className="text-left hidden lg:block">
                <div className="text-sm font-bold text-gray-900 leading-tight">
                  {authService.getCurrentUser()?.name || authService.getCurrentUser()?.id || "Sinh viên"}
                </div>
                <div className="text-xs text-gray-500 mt-0.5 space-y-0.5 font-medium">
                  <div>MSSV: {authService.getCurrentUser()?.id || "---"}</div>
                  {authService.getCurrentUser()?.classCode && (
                    <div>Lớp: {authService.getCurrentUser()?.classCode}</div>
                  )}
                </div>
              </div>
            </div>
          </div>
        </header>

        {/* Page Content */}
        <main className="p-4 lg:p-6">
          {children}
        </main>

        {/* Footer */}
        <footer className="text-center py-4 text-xs text-gray-400 border-t border-gray-100">
          © 2026 PTIT. All rights reserved.
        </footer>
      </div>
    </div>
  )
}