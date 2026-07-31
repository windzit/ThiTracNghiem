import type { Subject } from "@/shared/types"

/**
 * Format subject display label: MAMH - TENMH
 * Example: CSDL - Cơ sở dữ liệu
 */
export function formatSubjectLabel(subject: Partial<Subject> | null | undefined): string {
  if (!subject) return ""
  const code = subject.code || subject.id || ""
  const name = subject.name || ""
  if (code && name && code !== name) {
    return `${code} - ${name}`
  }
  return code || name
}
