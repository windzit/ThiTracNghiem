import { Badge } from "@/components/ui/badge"
import {
  Difficulty,
  DIFFICULTY_LABELS,
  DIFFICULTY_STYLES,
  type DifficultyValue,
} from "@/data/difficulty"

interface DifficultyBadgeProps {
  difficulty: DifficultyValue
  className?: string
}

export function DifficultyBadge({ difficulty, className }: DifficultyBadgeProps) {
  const label = DIFFICULTY_LABELS[difficulty] ?? difficulty
  const style = DIFFICULTY_STYLES[difficulty] ?? ""

  return (
    <Badge
      className={`font-medium text-xs px-2 py-0.5 rounded-full ${style} ${className ?? ""}`}
    >
      {label}
    </Badge>
  )
}

// Re-export để tiện dùng
export { Difficulty, DIFFICULTY_LABELS, DIFFICULTY_STYLES }
export type { DifficultyValue }