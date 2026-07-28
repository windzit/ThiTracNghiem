import { Button, Input } from "@/components/ui"
import { useNavigate } from "react-router-dom"
import { useState } from "react"
import { authService } from "@/services/authService"
import {
  ArrowLeft,
  UserCog,
  User,
  Lock,
  Eye,
  EyeOff,
  ShieldCheck,
  PieChart,
  Clock,
  ArrowRight,
} from "lucide-react"

export default function TeacherLogin() {
  const navigate = useNavigate()
  const [showPassword, setShowPassword] = useState(false)
  const [username, setUsername] = useState("")
  const [password, setPassword] = useState("")
  const [rememberMe, setRememberMe] = useState(false)

  const [error, setError] = useState("")

  const handleLogin = async (e: React.FormEvent) => {
    e.preventDefault()
    setError("")
    const normalizedUsername = username.trim().toUpperCase()
    console.log(`[TeacherLogin] Form submitted. Attempting login for username: "${normalizedUsername}"`);
    try {
      const res = await authService.loginTeacher(normalizedUsername, password)
      if (res.user) {
        console.log(`[TeacherLogin] Login succeeded. Redirecting to: "/teacher/dashboard"`);
        navigate("/teacher/dashboard")
      } else {
        console.warn(`[TeacherLogin] Login failed. Showing error message.`);
        setError(res.message || "Tên đăng nhập hoặc mật khẩu không đúng")
      }
    } catch (err) {
      console.error(`[TeacherLogin] Caught exception during login submission:`, err);
      setError("Có lỗi hệ thống xảy ra. Vui lòng thử lại sau.")
    }
  }

  return (
    <div className="min-h-screen flex flex-col bg-white">
      {/* Top Navigation Bar */}
      <nav className="flex items-center px-6 py-3">
        <button
          onClick={() => navigate("/")}
          className="flex items-center gap-2 text-sm text-gray-600 hover:text-gray-900 transition-colors"
        >
          <ArrowLeft className="h-4 w-4" />
          Quay lại chọn vai trò
        </button>
      </nav>

      {/* Main Content */}
      <div className="flex-1 relative">
        {/* Background Image - PTIT Building (anime style) */}
        <div
          className="absolute inset-0"
          style={{
            backgroundImage: "url('/bg-ptit-login.png')",
            backgroundSize: "cover",
            backgroundPosition: "center center",
            backgroundRepeat: "no-repeat",
          }}
        />
        {/* Gradient Overlay */}
        <div className="absolute inset-0 bg-gradient-to-br from-white/50 via-pink-50/40 to-white/30" />

        {/* Content Layer */}
        <div className="relative z-10 flex flex-col min-h-full p-10 pr-[440px]">
          {/* Logo Section */}
          <div className="mb-6">
            <div className="flex items-center gap-3 mb-2">
              <div>
                <div className="text-3xl font-bold">
                  <span className="text-[#D9272B]">PTIT</span>
                  <span className="text-gray-900 ml-2">CBT</span>
                </div>
                <div className="text-sm text-gray-500">Computer Based Testing System</div>
              </div>
            </div>
            {/* Red underline */}
            <div className="w-12 h-1 bg-[#D9272B] mt-3 mb-6" />

            {/* Description */}
            <p className="text-gray-600 text-base max-w-md">
              Nền tảng thi trắc nghiệm trực tuyến hiện đại, an toàn và tin cậy dành cho giảng viên và sinh viên.
            </p>
          </div>

          {/* Illustration */}
          <div className="flex-1 flex items-center justify-center my-2">
            <img
              src="/illustration-teacher-login.png"
              alt="Giảng viên PTIT"
              className="max-h-[320px] w-auto object-contain drop-shadow-lg"
            />
          </div>

          {/* Feature Cards */}
          <div className="grid grid-cols-3 gap-4 relative z-10">
            <div className="bg-white/80 backdrop-blur-sm rounded-2xl p-5 border border-gray-50 shadow-sm text-center">
              <div className="flex justify-center mb-3">
                <ShieldCheck className="h-8 w-8 text-[#D9272B]" />
              </div>
              <h3 className="text-sm font-bold text-gray-900 mb-1">An toàn</h3>
              <p className="text-xs text-gray-500">Bảo mật thông tin tuyệt đối</p>
            </div>
            <div className="bg-white/80 backdrop-blur-sm rounded-2xl p-5 border border-gray-50 shadow-sm text-center">
              <div className="flex justify-center mb-3">
                <PieChart className="h-8 w-8 text-[#D9272B]" />
              </div>
              <h3 className="text-sm font-bold text-gray-900 mb-1">Hiệu quả</h3>
              <p className="text-xs text-gray-500">Quản lý và đánh giá dễ dàng</p>
            </div>
            <div className="bg-white/80 backdrop-blur-sm rounded-2xl p-5 border border-gray-50 shadow-sm text-center">
              <div className="flex justify-center mb-3">
                <Clock className="h-8 w-8 text-[#D9272B]" />
              </div>
              <h3 className="text-sm font-bold text-gray-900 mb-1">Tiện lợi</h3>
              <p className="text-xs text-gray-500">Truy cập mọi lúc, mọi nơi</p>
            </div>
          </div>
        </div>

        {/* Login Card - Floating Overlay */}
        <div className="hidden lg:flex absolute right-12 top-1/2 -translate-y-1/2 z-20">
          <div className="w-[400px]">
            {/* Login Card */}
            <div className="bg-white rounded-2xl shadow-2xl border border-gray-100/80 p-6">
              {/* Icon */}
              <div className="flex justify-center mb-6">
                <div className="w-20 h-20 rounded-full bg-red-50 flex items-center justify-center">
                  <UserCog className="h-10 w-10 text-[#D9272B]" />
                </div>
              </div>

              {/* Title */}
              <div className="text-center mb-6">
                <h2 className="text-2xl font-bold text-gray-900 mb-2">
                  Đăng nhập dành cho <span className="text-[#D9272B]">Giảng viên</span>
                </h2>
              </div>

              {/* Divider - 3 segments (red + gray + gray) */}
              <div className="flex items-center justify-center gap-1 mb-8">
                <div className="w-8 h-1 bg-[#D9272B] rounded-full" />
                <div className="w-4 h-1 bg-gray-200 rounded-full" />
                <div className="w-4 h-1 bg-gray-200 rounded-full" />
              </div>

              {/* Form */}
              <form onSubmit={handleLogin} className="space-y-4">
                {/* Username */}
                <div>
                  <label className="block text-sm font-medium text-gray-700 mb-2">
                    Tên đăng nhập
                  </label>
                  <div className="relative">
                    <User className="absolute left-3 top-1/2 -translate-y-1/2 h-5 w-5 text-gray-400" />
                    <Input
                      type="text"
                      placeholder="Nhập tên đăng nhập (ví dụ: GV)"
                      className="pl-10 h-12"
                      value={username}
                      onChange={(e) => setUsername(e.target.value)}
                    />
                  </div>
                </div>

                {/* Password */}
                <div>
                  <label className="block text-sm font-medium text-gray-700 mb-2">
                    Mật khẩu
                  </label>
                  <div className="relative">
                    <Lock className="absolute left-3 top-1/2 -translate-y-1/2 h-5 w-5 text-gray-400" />
                    <Input
                      type={showPassword ? "text" : "password"}
                      placeholder="Nhập mật khẩu"
                      className="pl-10 pr-10 h-12"
                      value={password}
                      onChange={(e) => setPassword(e.target.value)}
                    />
                    <button
                      type="button"
                      onClick={() => setShowPassword(!showPassword)}
                      className="absolute right-3 top-1/2 -translate-y-1/2 text-gray-400 hover:text-gray-600"
                    >
                      {showPassword ? (
                        <EyeOff className="h-5 w-5" />
                      ) : (
                        <Eye className="h-5 w-5" />
                      )}
                    </button>
                  </div>
                </div>

                {/* Remember Me & Forgot Password */}
                <div className="flex items-center justify-between">
                  <label className="flex items-center gap-2 cursor-pointer">
                    <input
                      type="checkbox"
                      checked={rememberMe}
                      onChange={(e) => setRememberMe(e.target.checked)}
                      className="w-4 h-4 rounded border-gray-300 text-[#D9272B] focus:ring-[#D9272B]"
                    />
                    <span className="text-sm text-gray-600">Ghi nhớ đăng nhập</span>
                  </label>
                  <button type="button" className="text-sm text-[#D9272B] hover:underline font-medium">
                    Quên mật khẩu?
                  </button>
                </div>

                {/* Error Message */}
                {error && (
                  <div className="text-sm text-red-600 bg-red-50 border border-red-200 rounded-lg px-3 py-2">
                    {error}
                  </div>
                )}

                {/* Login Button */}
                <Button
                  type="submit"
                  className="w-full h-12 bg-[#D9272B] hover:bg-[#c4222a] text-white font-medium text-base rounded-lg"
                >
                  Đăng nhập
                  <ArrowRight className="ml-2 h-5 w-5" />
                </Button>
              </form>

              {/* Test credentials hint */}
              <div className="mt-4 p-3 bg-gray-50 rounded-lg border border-gray-200">
                <p className="text-xs text-gray-500 font-medium mb-1">Test credentials:</p>
                <p className="text-xs text-gray-400">Username: gvptit | Password: 123456</p>
              </div>

              {/* Divider with "hoặc" */}
              <div className="flex items-center gap-4 my-6">
                <div className="flex-1 h-px bg-gray-200" />
                <span className="text-sm text-gray-400">hoặc</span>
                <div className="flex-1 h-px bg-gray-200" />
              </div>

              {/* Google Login */}
              <button className="w-full h-12 border border-gray-200 rounded-lg flex items-center justify-center gap-3 hover:bg-gray-50 transition-colors">
                <svg className="h-5 w-5" viewBox="0 0 24 24">
                  <path
                    d="M22.56 12.25c0-.78-.07-1.53-.2-2.25H12v4.26h5.92c-.26 1.37-1.04 2.53-2.21 3.31v2.77h3.57c2.08-1.92 3.28-4.74 3.28-8.09z"
                    fill="#4285F4"
                  />
                  <path
                    d="M12 23c2.97 0 5.46-.98 7.28-2.66l-3.57-2.77c-.98.66-2.23 1.06-3.71 1.06-2.86 0-5.29-1.93-6.16-4.53H2.18v2.84C3.99 20.53 7.7 23 12 23z"
                    fill="#34A853"
                  />
                  <path
                    d="M5.84 14.09c-.22-.66-.35-1.36-.35-2.09s.13-1.43.35-2.09V7.07H2.18C1.43 8.55 1 10.22 1 12s.43 3.45 1.18 4.93l2.85-2.22.81-.62z"
                    fill="#FBBC05"
                  />
                  <path
                    d="M12 5.38c1.62 0 3.06.56 4.21 1.64l3.15-3.15C17.45 2.09 14.97 1 12 1 7.7 1 3.99 3.47 2.18 7.07l3.66 2.84c.87-2.6 3.3-4.53 6.16-4.53z"
                    fill="#EA4335"
                  />
                </svg>
                <span className="text-sm font-medium text-gray-700">Đăng nhập với Google</span>
              </button>

              {/* Support Info */}
              <div className="mt-4 pt-4 border-t border-gray-100">
                <div className="flex items-center justify-center gap-2 text-xs text-gray-500">
                  <ShieldCheck className="h-4 w-4 text-gray-400" />
                  <span>Hỗ trợ kỹ thuật: 024 3355 1212</span>
                  <span className="text-gray-300">|</span>
                  <span>Email: support@ptit.edu.vn</span>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>

      {/* Footer */}
      <footer className="text-center py-4 border-t border-gray-100">
        <p className="text-xs text-gray-400">© 2026 PTIT. All rights reserved.</p>
      </footer>
    </div>
  )
}