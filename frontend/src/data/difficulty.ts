// Difficulty constants — dùng string literal union type thay enum
// (vì tsconfig có erasableSyntaxOnly, không dùng enum được)

export const Difficulty = {
  EASY: "EASY" as const,
  MEDIUM: "MEDIUM" as const,
  HARD: "HARD" as const,
} as const

export type DifficultyValue = (typeof Difficulty)[keyof typeof Difficulty]

// Map sang label tiếng Việt (chỉ dùng ở UI layer)
export const DIFFICULTY_LABELS: Record<DifficultyValue, string> = {
  [Difficulty.EASY]: "Dễ",
  [Difficulty.MEDIUM]: "Trung bình",
  [Difficulty.HARD]: "Khó",
}

// Map sang style Tailwind (chỉ dùng ở UI layer)
export const DIFFICULTY_STYLES: Record<DifficultyValue, string> = {
  [Difficulty.EASY]: "bg-green-50 text-green-700 border-green-200 hover:bg-green-50",
  [Difficulty.MEDIUM]: "bg-yellow-50 text-yellow-700 border-yellow-200 hover:bg-yellow-50",
  [Difficulty.HARD]: "bg-red-50 text-red-700 border-red-200 hover:bg-red-50",
}

// Helper: parse string từ backend/API sang DifficultyValue
export function parseDifficulty(value: string): DifficultyValue {
  const upper = value.toUpperCase()
  if (upper === "EASY" || upper === "DỄ" || upper === "DE") return Difficulty.EASY
  if (upper === "MEDIUM" || upper === "TRUNG BÌNH" || upper === "TRUNG BINH") return Difficulty.MEDIUM
  if (upper === "HARD" || upper === "KHÓ" || upper === "KHO") return Difficulty.HARD
  return Difficulty.MEDIUM // fallback
}

// Helper: DifficultyValue sang string tiếng Việt (cho form select options)
export function difficultyToVietnamese(d: DifficultyValue): string {
  return DIFFICULTY_LABELS[d]
}

// Danh sách tất cả difficulty values (cho dropdown filter)
export const DIFFICULTY_OPTIONS: { value: DifficultyValue; label: string }[] = [
  { value: Difficulty.EASY, label: "Dễ" },
  { value: Difficulty.MEDIUM, label: "Trung bình" },
  { value: Difficulty.HARD, label: "Khó" },
]