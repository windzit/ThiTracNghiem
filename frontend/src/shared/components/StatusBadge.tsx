import { Badge } from "@/components/ui/badge"

type StatusVariant =
  | "active"
  | "inactive"
  | "hidden"
  | "used"
  | "unused"
  | "published"
  | "unpublished"

interface StatusBadgeProps {
  status: StatusVariant | string
  className?: string
}

const statusConfig: Record<string, { label: string; classes: string }> = {
  active: {
    label: "Đang hoạt động",
    classes: "bg-[#F0FDF4] text-[#16A34A] border border-green-200",
  },
  inactive: {
    label: "Đã kết thúc",
    classes: "bg-[#F3F4F6] text-[#6B7280] border border-gray-200",
  },
  hidden: {
    label: "Đã ẩn",
    classes: "bg-orange-50 text-orange-700 border border-orange-200",
  },
  used: {
    label: "Đã sử dụng",
    classes: "bg-green-50 text-green-700 border border-green-200",
  },
  unused: {
    label: "Chưa sử dụng",
    classes: "bg-orange-50 text-orange-700 border border-orange-200",
  },
  deleted: {
    label: "Vô hiệu hóa",
    classes: "bg-gray-100 text-gray-500 border border-gray-200",
  },
  published: {
    label: "Đã công bố",
    classes: "bg-green-50 text-green-700 border border-green-200",
  },
  unpublished: {
    label: "Chưa công bố",
    classes: "bg-orange-50 text-orange-700 border border-orange-200",
  },
}

export function StatusBadge({ status, className = "" }: StatusBadgeProps) {
  const config = statusConfig[status] || {
    label: status,
    classes: "bg-gray-50 text-gray-700 border border-gray-200",
  }

  return (
    <Badge
      className={`font-medium text-xs px-3 py-1 rounded-full hover:${config.classes.split(" ")[0]} ${config.classes} ${className}`}
    >
      {config.label}
    </Badge>
  )
}