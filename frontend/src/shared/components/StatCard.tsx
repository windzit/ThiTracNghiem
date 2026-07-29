import type { LucideIcon } from "lucide-react"

interface StatCardProps {
  icon: LucideIcon
  value: string | number
  label: string
  sublabel?: string
  iconBg?: string
  iconColor?: string
  shape?: "rounded-xl" | "rounded-full"
  size?: "sm" | "md"
}

export function StatCard({
  icon: Icon,
  value,
  label,
  sublabel,
  iconBg = "bg-blue-50",
  iconColor = "text-blue-500",
  shape = "rounded-xl",
  size = "md",
}: StatCardProps) {
  const isSm = size === "sm"

  return (
    <div className="bg-white rounded-xl border border-gray-200 flex items-center gap-4 p-5">
      <div
        className={`${isSm ? "w-12 h-12" : "w-14 h-14"} ${shape} ${iconBg} flex items-center justify-center flex-shrink-0`}
      >
        <Icon className={`${isSm ? "h-6 w-6" : "h-7 w-7"} ${iconColor}`} />
      </div>
      <div className="min-w-0">
        <div className={`${isSm ? "text-xs" : "text-sm"} text-gray-500`}>{label}</div>
        <div className={`${isSm ? "text-2xl" : "text-3xl"} font-bold text-gray-900 leading-tight`}>
          {value}
        </div>
        {sublabel && (
          <div className={`${isSm ? "text-[10px]" : "text-xs"} text-gray-400 mt-0.5`}>
            {sublabel}
          </div>
        )}
      </div>
    </div>
  )
}