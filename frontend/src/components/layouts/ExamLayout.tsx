import { Outlet } from "react-router-dom"

export default function ExamLayout() {
  return (
    <div className="min-h-screen bg-[#F9FAFB]">
      <Outlet />
    </div>
  )
}