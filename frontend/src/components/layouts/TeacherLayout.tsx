import { useNavigate, useLocation } from "react-router-dom"
import { useState, useEffect, useRef } from "react"
import {
  Home,
  Users,
  User,
  BookOpen,
  HelpCircle,
  ClipboardList,
  BarChart3,
  LogOut,
  ChevronDown,
  Menu,
  Lock,
} from "lucide-react"
import { NotificationDropdown } from "@/components/shared/NotificationDropdown"
import { authService } from "@/services/authService"
import { useToast } from "@/context/ToastContext"
import { systemSettingsService } from "@/services/systemSettingsService"

interface TeacherLayoutProps {
  children: React.ReactNode
  breadcrumb?: { label: string; path?: string }[]
  pageTitle?: string
}

const menuItems = [
  { section: "DASHBOARD" },
  { icon: Home, label: "Dashboard", path: "/teacher/dashboard" },
  { section: "QUẢN LÝ" },
  { icon: Users, label: "Lớp học", path: "/teacher/classes" },
  { icon: BookOpen, label: "Môn học", path: "/teacher/subjects" },
  { icon: HelpCircle, label: "Câu hỏi", path: "/teacher/questions" },
  { section: "BÁO CÁO" },
  { icon: BarChart3, label: "Bảng điểm", path: "/teacher/gradebook" },
]

export default function TeacherLayout({ children, breadcrumb, pageTitle }: TeacherLayoutProps) {
  const navigate = useNavigate()
  const location = useLocation()
  const [sidebarOpen, setSidebarOpen] = useState(false)
  const [avatarOpen, setAvatarOpen] = useState(false)
  const avatarRef = useRef<HTMLDivElement>(null)

  const { showSuccess, showError } = useToast()
  const [fullscreenRequired, setFullscreenRequired] = useState(false)

  useEffect(() => {
    systemSettingsService.getSettings()
      .then((settings) => setFullscreenRequired(settings.fullscreenRequired))
      .catch((err) => {
        console.error("Failed to load settings:", err)
        // Keep default false, don't crash
      })
  }, [])

  const toggleSupervision = async () => {
    const previousState = fullscreenRequired
    try {
      await systemSettingsService.updateSettings(!previousState)
      // Backend is single source of truth — re-fetch after POST
      const latest = await systemSettingsService.getSettings()
      setFullscreenRequired(latest.fullscreenRequired)
      showSuccess(
        "Cập nhật thành công",
        `Đã ${latest.fullscreenRequired ? "BẬT" : "TẮT"} chế độ giám sát thi Fullscreen.`
      )
    } catch (err: any) {
      setFullscreenRequired(previousState)
      const backendMsg = err?.response?.data?.message || err?.message || "Lỗi không xác định"
      showError("Lỗi hệ thống", `Không thể cập nhật cấu hình giám sát: ${backendMsg}`)
    }
  }

  // Close avatar dropdown when clicking outside
  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (avatarRef.current && !avatarRef.current.contains(event.target as Node)) {
        setAvatarOpen(false)
      }
    }
    document.addEventListener("mousedown", handleClickOutside)
    return () => document.removeEventListener("mousedown", handleClickOutside)
  }, [])

  const isActive = (path: string) => {
    if (location.pathname === path) return true
    if (location.pathname.startsWith(path + "/")) return true
    return false
  }

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
      <aside className={`w-[220px] bg-white border-r border-gray-200 flex flex-col fixed h-full z-40 transition-transform duration-200 lg:translate-x-0 ${sidebarOpen ? "translate-x-0" : "-translate-x-full"}`}>
        {/* Logo */}
        <div className="px-5 py-5 flex items-center gap-3">
          <img src="/logo-ptit.png" alt="PTIT" className="h-8 w-auto" />
          <div>
            <div className="text-base font-bold text-[#D9272B]">PTIT CBT</div>
            <div className="text-[10px] text-gray-400 leading-tight">Computer Based Testing System</div>
          </div>
        </div>

        {/* Menu */}
        <nav className="flex-1 px-3 py-2 space-y-0.5 overflow-y-auto">
          {menuItems.map((item, i) => {
            if ("section" in item) {
              return (
                <div key={i} className="px-3 pt-4 pb-2 text-[10px] font-semibold text-gray-400 uppercase tracking-wider">
                  {item.section}
                </div>
              )
            }
            const Icon = item.icon
            const active = "path" in item && item.path !== undefined && isActive(item.path)
            return (
              <button
                key={item.label}
                onClick={() => "path" in item && navigate(item.path)}
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

        {/* Logout */}
        <div className="px-3 py-4 border-t border-gray-100">
          <button
            onClick={() => navigate("/")}
            className="w-full flex items-center gap-3 px-3 py-2.5 rounded-lg text-sm font-medium text-gray-500 hover:bg-gray-50 hover:text-gray-700 transition-colors"
          >
            <LogOut className="h-5 w-5 flex-shrink-0" />
            Đăng xuất
          </button>
        </div>
      </aside>

      {/* Main Content */}
      <div className="flex-1 lg:ml-[220px]">
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
            <Menu className="h-5 w-5 text-gray-500 hidden lg:block" />
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
              <div className="text-base font-bold text-gray-900">
                {pageTitle || menuItems.find((item) => "path" in item && item.path !== undefined && isActive(item.path))?.label || "Trang chủ"}
              </div>
            )}
          </div>

          {/* Right */}
          <div className="flex items-center gap-2 lg:gap-4">
            {/* 🛡 Supervision Mode Toggle */}
            <div className="flex items-center gap-2 bg-gray-50 border border-gray-200 px-3 py-1.5 rounded-full mr-1 sm:mr-2">
              <span className="text-xs font-semibold text-gray-700 hidden sm:inline flex items-center gap-1">
                🛡 Giám sát
              </span>
              <button
                type="button"
                onClick={toggleSupervision}
                className={`relative inline-flex h-5 w-9 shrink-0 cursor-pointer rounded-full border-2 border-transparent transition-colors duration-200 ease-in-out focus:outline-none ${
                  fullscreenRequired ? "bg-red-600" : "bg-gray-200"
                }`}
              >
                <span
                  className={`pointer-events-none inline-block h-4 w-4 transform rounded-full bg-white shadow ring-0 transition duration-200 ease-in-out ${
                    fullscreenRequired ? "translate-x-4" : "translate-x-0"
                  }`}
                />
              </button>
              <span className={`text-[10px] font-bold select-none ${fullscreenRequired ? "text-red-600" : "text-gray-400"}`}>
                {fullscreenRequired ? "ON" : "OFF"}
              </span>
            </div>

            {/* Notification Bell */}
            <NotificationDropdown
              items={[]}
              unreadCount={0}
            />

            {/* Avatar Dropdown */}
            <div className="relative" ref={avatarRef}>
              <button
                onClick={() => setAvatarOpen(!avatarOpen)}
                className="flex items-center gap-2 lg:gap-3 pl-2 lg:pl-3 pr-1 lg:pr-2 py-1.5 rounded-lg hover:bg-gray-50 transition-colors"
              >
                <div className="w-8 h-8 lg:w-9 lg:h-9 rounded-full bg-gray-200 flex items-center justify-center overflow-hidden">
                  <User className="h-4 w-4 lg:h-5 lg:w-5 text-gray-500" />
                </div>
                <div className="text-left hidden lg:block">
                  <div className="text-sm font-semibold text-gray-900">
                    {authService.getCurrentUser()?.name || authService.getCurrentUser()?.id || "Giảng viên"}
                  </div>
                  <div className="text-xs text-gray-400">Giảng viên</div>
                </div>
                <ChevronDown className="h-4 w-4 text-gray-400 hidden lg:block" />
              </button>

              {avatarOpen && (
                <div className="absolute right-0 top-full mt-2 w-56 bg-white rounded-xl border border-gray-200 shadow-lg z-50 overflow-hidden">
                  {/* User Info Header */}
                  <div className="px-4 py-3 border-b border-gray-100">
                    <div className="text-sm font-semibold text-gray-900">
                      {authService.getCurrentUser()?.name || authService.getCurrentUser()?.id || "Giảng viên"}
                    </div>
                    <div className="text-xs text-gray-400">Giảng viên</div>
                  </div>

                  {/* Menu Items */}
                  <div className="py-1">
                    <button
                      disabled
                      className="w-full flex items-center gap-3 px-4 py-2.5 text-sm text-gray-400 cursor-not-allowed"
                    >
                      <User className="h-4 w-4" />
                      Thông tin tài khoản
                    </button>
                    <button
                      disabled
                      className="w-full flex items-center gap-3 px-4 py-2.5 text-sm text-gray-400 cursor-not-allowed"
                    >
                      <Lock className="h-4 w-4" />
                      Đổi mật khẩu
                    </button>
                  </div>

                  {/* Divider + Logout */}
                  <div className="border-t border-gray-100 py-1">
                    <button
                      onClick={() => {
                        setAvatarOpen(false)
                        navigate("/")
                      }}
                      className="w-full flex items-center gap-3 px-4 py-2.5 text-sm text-red-600 hover:bg-red-50 transition-colors"
                    >
                      <LogOut className="h-4 w-4" />
                      Đăng xuất
                    </button>
                  </div>
                </div>
              )}
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