import { useNavigate } from "react-router-dom"
import { Shield, Info, GraduationCap, User, CheckCircle2, ArrowRight } from "lucide-react"

export default function RoleSelection() {
  const navigate = useNavigate()

  return (
    <div className="relative min-h-screen flex flex-col bg-white overflow-hidden">
      {/* Background Image */}
      <div
        className="absolute inset-0 z-0"
        style={{
          backgroundImage: "url('/bg-ptit.png')",
          backgroundSize: "cover",
          backgroundPosition: "left center",
          backgroundRepeat: "no-repeat",
          opacity: 0.3,
        }}
      />

      {/* Dot Pattern - Right Side */}
      <div className="absolute top-20 right-10 z-0 opacity-20">
        <div className="grid grid-cols-6 gap-2">
          {Array.from({ length: 36 }).map((_, i) => (
            <div key={i} className="w-1.5 h-1.5 rounded-full bg-red-300" />
          ))}
        </div>
      </div>

      {/* Curved Lines - Bottom Right */}
      <div className="absolute bottom-0 right-0 z-0 opacity-10">
        <svg width="400" height="200" viewBox="0 0 400 200" fill="none">
          <path d="M0 200 Q100 100 200 50 T400 0" stroke="#D9272B" strokeWidth="1" fill="none" />
          <path d="M0 200 Q120 120 220 70 T400 20" stroke="#D9272B" strokeWidth="1" fill="none" />
          <path d="M0 200 Q140 140 240 90 T400 40" stroke="#D9272B" strokeWidth="1" fill="none" />
          <path d="M0 200 Q160 160 260 110 T400 60" stroke="#D9272B" strokeWidth="1" fill="none" />
          <path d="M0 200 Q180 180 280 130 T400 80" stroke="#D9272B" strokeWidth="1" fill="none" />
        </svg>
      </div>

      {/* Content */}
      <div className="relative z-10 flex flex-col min-h-screen">
        {/* Header */}
        <header className="flex items-center justify-between px-8 py-4">
          <div className="flex items-center gap-3">
            <img src="/logo-ptit.png" alt="PTIT Logo" className="h-10 w-auto" />
            <div>
              <div className="text-lg font-bold text-[#D9272B]">PTIT CBT</div>
              <div className="text-xs text-gray-500">Computer Based Testing System</div>
            </div>
          </div>
          <button className="flex items-center gap-2 text-sm text-gray-600 hover:text-gray-900 transition-colors">
            <Info className="h-4 w-4" />
            Giới thiệu hệ thống
          </button>
        </header>

        {/* Main Content */}
        <main className="flex-1 flex flex-col items-center px-4 pt-8 pb-12">
          {/* Hero Section */}
          <div className="text-center max-w-2xl mx-auto mb-10">
            {/* Welcome Badge */}
            <div className="inline-flex items-center gap-2 px-4 py-2 rounded-full border border-red-200 bg-white mb-6">
              <Shield className="h-4 w-4 text-[#D9272B]" />
              <span className="text-sm text-[#D9272B] font-medium">Chào mừng bạn đến với</span>
            </div>

            {/* Title */}
            <h1 className="text-4xl font-bold text-gray-900 mb-4">
              Hệ thống thi trắc nghiệm <span className="text-[#D9272B]">PTIT</span>
            </h1>

            {/* Subtitle */}
            <p className="text-gray-500 text-base mb-6 max-w-lg mx-auto">
              Nền tảng thi trắc nghiệm trực tuyến hiện đại, an toàn và tin cậy dành cho giảng viên và sinh viên.
            </p>

            {/* Divider */}
            <div className="w-10 h-0.5 bg-[#D9272B] mx-auto mb-6" />

            {/* Question */}
            <p className="text-gray-700 font-medium text-base">
              Bạn muốn đăng nhập với vai trò nào?
            </p>
          </div>

          {/* Role Cards */}
          <div className="grid grid-cols-1 md:grid-cols-2 gap-6 max-w-4xl w-full">
            {/* Teacher Card */}
            <div className="bg-white rounded-2xl border border-gray-200 shadow-sm p-6 flex flex-col items-center text-center hover:shadow-md transition-shadow">
              {/* Illustration */}
              <div className="w-48 h-48 mb-6 flex items-center justify-center">
                <img
                  src="/illustration-teacher.png"
                  alt="Giảng viên"
                  className="w-full h-full object-contain"
                />
              </div>

              {/* Icon + Title */}
              <div className="flex items-center gap-3 mb-3 self-start w-full">
                <div className="w-10 h-10 rounded-lg bg-[#D9272B] flex items-center justify-center flex-shrink-0">
                  <GraduationCap className="h-5 w-5 text-white" />
                </div>
                <h2 className="text-lg font-bold text-gray-900">Cổng giảng viên</h2>
              </div>

              {/* Description */}
              <p className="text-sm text-gray-500 mb-4 self-start w-full">
                Quản lý lớp học, môn học, ngân hàng câu hỏi, đề thi và kết quả của sinh viên.
              </p>

              {/* Bullet Points */}
              <ul className="space-y-2 mb-6 self-start w-full">
                <li className="flex items-center gap-2 text-sm text-gray-600">
                  <CheckCircle2 className="h-4 w-4 text-[#D9272B] flex-shrink-0" />
                  Quản lý dữ liệu toàn diện
                </li>
                <li className="flex items-center gap-2 text-sm text-gray-600">
                  <CheckCircle2 className="h-4 w-4 text-[#D9272B] flex-shrink-0" />
                  Tạo và quản lý đề thi
                </li>
                <li className="flex items-center gap-2 text-sm text-gray-600">
                  <CheckCircle2 className="h-4 w-4 text-[#D9272B] flex-shrink-0" />
                  Xem báo cáo và thống kê
                </li>
              </ul>

              {/* Button */}
              <button
                onClick={() => navigate("/teacher/login")}
                className="w-full bg-[#D9272B] text-white font-medium py-3 px-6 rounded-lg hover:bg-[#c4222a] transition-colors flex items-center justify-center gap-2 mt-auto"
              >
                Tiếp tục với Giảng viên
                <ArrowRight className="h-4 w-4" />
              </button>
            </div>

            {/* Student Card */}
            <div className="bg-white rounded-2xl border border-gray-200 shadow-sm p-6 flex flex-col items-center text-center hover:shadow-md transition-shadow">
              {/* Illustration */}
              <div className="w-48 h-48 mb-6 flex items-center justify-center">
                <img
                  src="/illustration-student.png"
                  alt="Sinh viên"
                  className="w-full h-full object-contain"
                />
              </div>

              {/* Icon + Title */}
              <div className="flex items-center gap-3 mb-3 self-start w-full">
                <div className="w-10 h-10 rounded-lg bg-[#D9272B] flex items-center justify-center flex-shrink-0">
                  <User className="h-5 w-5 text-white" />
                </div>
                <h2 className="text-lg font-bold text-gray-900">Cổng sinh viên</h2>
              </div>

              {/* Description */}
              <p className="text-sm text-gray-500 mb-4 self-start w-full">
                Tham gia thi trắc nghiệm, xem lịch sử thi và kết quả của bạn.
              </p>

              {/* Bullet Points */}
              <ul className="space-y-2 mb-6 self-start w-full">
                <li className="flex items-center gap-2 text-sm text-gray-600">
                  <CheckCircle2 className="h-4 w-4 text-[#D9272B] flex-shrink-0" />
                  Tham gia các kỳ thi
                </li>
                <li className="flex items-center gap-2 text-sm text-gray-600">
                  <CheckCircle2 className="h-4 w-4 text-[#D9272B] flex-shrink-0" />
                  Xem kết quả và lịch sử thi
                </li>
                <li className="flex items-center gap-2 text-sm text-gray-600">
                  <CheckCircle2 className="h-4 w-4 text-[#D9272B] flex-shrink-0" />
                  Cập nhật thông tin cá nhân
                </li>
              </ul>

              {/* Button */}
              <button
                onClick={() => navigate("/student/login")}
                className="w-full bg-[#D9272B] text-white font-medium py-3 px-6 rounded-lg hover:bg-[#c4222a] transition-colors flex items-center justify-center gap-2 mt-auto"
              >
                Tiếp tục với Sinh viên
                <ArrowRight className="h-4 w-4" />
              </button>
            </div>
          </div>
        </main>

        {/* Footer */}
        <footer className="text-center py-6 border-t border-gray-100">
          <div className="flex items-center justify-center gap-2 mb-2">
            <Shield className="h-4 w-4 text-gray-400" />
            <span className="text-sm text-gray-500">
              An toàn <span className="mx-1">•</span> Bảo mật <span className="mx-1">•</span> Tin cậy
            </span>
          </div>
          <p className="text-xs text-gray-400">© 2026 PTIT. All rights reserved.</p>
        </footer>
      </div>
    </div>
  )
}