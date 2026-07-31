import { Search, Filter } from "lucide-react"
import type { LucideIcon } from "lucide-react"
import { Input } from "@/shared/ui/input"
import { Button } from "@/shared/ui/button"
import { Select } from "@/shared/ui/select"

interface ToolbarFilter {
  type: "select"
  value: string
  onChange: (value: string) => void
  options: { value: string; label: string }[]
  className?: string
}

interface ToolbarAction {
  label: string
  icon?: LucideIcon
  onClick: () => void
  variant?: "primary" | "outline"
  className?: string
}

interface PageToolbarProps {
  searchPlaceholder?: string
  searchValue: string
  onSearchChange: (value: string) => void
  filters?: ToolbarFilter[]
  actions?: ToolbarAction[]
  className?: string
}

export function PageToolbar({
  searchPlaceholder = "Tìm kiếm...",
  searchValue,
  onSearchChange,
  filters = [],
  actions = [],
  className = "",
}: PageToolbarProps) {
  return (
    <div className={`flex items-center gap-3 ${className}`}>
      {/* Search */}
      <div className="relative flex-1 max-w-md">
        <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
        <Input
          placeholder={searchPlaceholder}
          className="pl-10 h-10"
          value={searchValue}
          onChange={(e) => onSearchChange(e.target.value)}
        />
      </div>

      {/* Filters */}
      {filters.map((filter, idx) => (
        <Select
          key={idx}
          className={`h-10 ${filter.className || "w-[180px]"}`}
          value={filter.value}
          onChange={(e) => filter.onChange(e.target.value)}
          options={filter.options}
        />
      ))}

      {/* Filter button */}
      <Button variant="outline" className="h-10 gap-2">
        <Filter className="h-4 w-4" />
        Bộ lọc
      </Button>

      {/* Spacer */}
      <div className="flex-1" />

      {/* Actions */}
      {actions.map((action, idx) => {
        const Icon = action.icon
        const isPrimary = action.variant === "primary"
        return (
          <Button
            key={idx}
            className={`h-10 gap-2 ${
              isPrimary
                ? "bg-[#D9272B] hover:bg-[#C42226] text-white"
                : ""
            } ${action.className || ""}`}
            variant={isPrimary ? undefined : "outline"}
            onClick={action.onClick}
          >
            {Icon && <Icon className="h-4 w-4" />}
            {action.label}
          </Button>
        )
      })}
    </div>
  )
}