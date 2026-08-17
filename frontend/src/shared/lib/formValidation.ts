export const VALIDATION_CONSTANTS = {
  MAX_CODE_LENGTH: 15,
} as const

/**
 * Check if string contains forbidden characters matching C++ StorageValidator:
 * pipe '|', carriage return '\r', newline '\n', tab '\t', or ASCII control chars (< 32)
 */
export function containsForbiddenChars(str: string): boolean {
  for (let i = 0; i < str.length; i++) {
    const code = str.charCodeAt(i)
    if (code < 32 || str[i] === '|' || str[i] === '\r' || str[i] === '\n' || str[i] === '\t') {
      return true
    }
  }
  return false
}

/**
 * Title Case Transformation:
 * trim() -> collapse consecutive spaces -> Capitalize First Letter Of Each Word
 * Example: "cOng   nGhE" -> "Cong Nghe", "ngUYeN   vaN   an" -> "Nguyen Van An"
 */
export function toTitleCase(value: string): string {
  const trimmed = value.trim().replace(/\s+/g, ' ')
  if (!trimmed) return ''
  return trimmed
    .split(' ')
    .map((word) => (word ? word.charAt(0).toUpperCase() + word.slice(1).toLowerCase() : ''))
    .join(' ')
}

/**
 * Normalization Pipeline for Code/PK fields (MASV, MALOP, MAMH):
 * strip all spaces -> uppercase
 * Example: " d 22-cq cn01 " -> "D22-CQCN01"
 */
export function normalizeIdentifier(value: string): string {
  return value.replace(/\s+/g, '').toUpperCase()
}

/**
 * Normalization Pipeline for Code/PK legacy alias:
 */
export function normalizeCode(value: string): string {
  return normalizeIdentifier(value)
}

/**
 * Normalization Pipeline for Human-readable Text fields (TENLOP, TENMH, HO, TEN):
 * trim() -> collapse consecutive spaces -> Title Case
 */
export function normalizeText(value: string): string {
  return toTitleCase(value)
}

/**
 * Normalization Pipeline for Question Content (NOIDUNG):
 * trim() -> collapse consecutive spaces -> PRESERVE EXACT USER CASE (NO AUTO-CAPITALIZATION)
 */
export function normalizeQuestionContent(value: string): string {
  return value.trim().replace(/\s+/g, ' ')
}

/**
 * Normalization Pipeline for Question Options (A, B, C, D):
 * trim() -> collapse consecutive spaces -> Capitalize ONLY the FIRST letter of the first word (Sentence Case)
 * Example: "phuong an   a   cau 10" -> "Phuong an a cau 10"
 */
export function normalizeQuestionOption(value: string): string {
  const collapsed = value.trim().replace(/\s+/g, ' ')
  if (!collapsed) return ''
  return collapsed.charAt(0).toUpperCase() + collapsed.slice(1)
}

/**
 * Legacy alias for Question Text:
 */
export function normalizeQuestionText(value: string): string {
  return normalizeQuestionContent(value)
}

/**
 * Normalization Pipeline for Department (Khoa):
 */
export function normalizeDepartment(value: string): string {
  return normalizeText(value)
}

/**
 * Normalization Pipeline for Password:
 * NEVER trim, NEVER uppercase, NEVER modify raw user input.
 */
export function normalizePassword(value: string): string {
  return value
}

// Allowed regex patterns
const CODE_REGEX = /^[A-Za-z0-9-]+$/
const NAME_REGEX = /^[A-Za-z0-9\s\-_()\u00C0-\u024F\u1EA0-\u1EF9]+$/
const STUDENT_NAME_REGEX = /^[A-Za-z\s\u00C0-\u024F\u1EA0-\u1EF9]+$/

/**
 * Validate Class Code (MALOP)
 * Rule 1: Letters, numbers, and hyphen '-'. No spaces, max 15 chars.
 */
export function validateClassCode(code: string): string | null {
  const stripped = code.replace(/\s+/g, '')
  if (!stripped) return "Mã lớp không được để trống."
  if (stripped.length > VALIDATION_CONSTANTS.MAX_CODE_LENGTH) {
    return `Mã lớp không vượt quá ${VALIDATION_CONSTANTS.MAX_CODE_LENGTH} ký tự.`
  }
  if (!CODE_REGEX.test(stripped)) {
    return "Mã lớp chỉ được chứa chữ cái, chữ số và dấu gạch ngang '-'."
  }
  if (containsForbiddenChars(code)) {
    return "Mã lớp không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Class Name (TENLOP)
 * Rule 2: Letters, numbers, spaces, '-', '_', '(', ')'
 */
export function validateClassName(name: string): string | null {
  const trimmed = name.trim()
  if (!trimmed) return "Tên lớp không được để trống."
  if (!NAME_REGEX.test(trimmed)) {
    return "Tên lớp chỉ được chứa chữ cái, chữ số, khoảng trắng, '-', '_', '(' và ')'."
  }
  if (containsForbiddenChars(name)) {
    return "Tên lớp không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Department (Khoa)
 */
export function validateDepartment(dept: string): string | null {
  const normalized = normalizeDepartment(dept)
  if (!normalized) return "Khoa không được để trống."
  if (containsForbiddenChars(dept)) {
    return "Khoa không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Subject Code (MAMH)
 * Rule 1: Letters, numbers, and hyphen '-'. No spaces, max 15 chars.
 */
export function validateSubjectCode(code: string): string | null {
  const stripped = code.replace(/\s+/g, '')
  if (!stripped) return "Mã môn không được để trống."
  if (stripped.length > VALIDATION_CONSTANTS.MAX_CODE_LENGTH) {
    return `Mã môn không vượt quá ${VALIDATION_CONSTANTS.MAX_CODE_LENGTH} ký tự.`
  }
  if (!CODE_REGEX.test(stripped)) {
    return "Mã môn chỉ được chứa chữ cái, chữ số và dấu gạch ngang '-'."
  }
  if (containsForbiddenChars(code)) {
    return "Mã môn không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Subject Name (TENMH)
 * Rule 2: Letters, numbers, spaces, '-', '_', '(', ')'
 */
export function validateSubjectName(name: string): string | null {
  const trimmed = name.trim()
  if (!trimmed) return "Tên môn không được để trống."
  if (!NAME_REGEX.test(trimmed)) {
    return "Tên môn chỉ được chứa chữ cái, chữ số, khoảng trắng, '-', '_', '(' và ')'."
  }
  if (containsForbiddenChars(name)) {
    return "Tên môn không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Student ID (MASV)
 * Rule 1: Letters, numbers, and hyphen '-'. No spaces, max 15 chars.
 */
export function validateStudentId(masv: string): string | null {
  const stripped = masv.replace(/\s+/g, '')
  if (!stripped) return "Mã sinh viên không được để trống."
  if (stripped.length > VALIDATION_CONSTANTS.MAX_CODE_LENGTH) {
    return `Mã sinh viên không vượt quá ${VALIDATION_CONSTANTS.MAX_CODE_LENGTH} ký tự.`
  }
  if (!CODE_REGEX.test(stripped)) {
    return "Mã sinh viên chỉ được chứa chữ cái, chữ số và dấu gạch ngang '-'."
  }
  if (containsForbiddenChars(masv)) {
    return "Mã sinh viên không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Student Name / HO / TEN
 * Rule 3: ONLY letters and spaces
 */
export function validateStudentName(name: string): string | null {
  const trimmed = name.trim()
  if (!trimmed) return "Họ tên không được để trống."
  if (!STUDENT_NAME_REGEX.test(trimmed)) {
    return "Họ tên sinh viên chỉ được chứa chữ cái và khoảng trắng."
  }
  if (containsForbiddenChars(name)) {
    return "Họ tên không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Password
 */
export function validatePassword(pass: string): string | null {
  if (!pass) return "Mật khẩu không được để trống."
  if (containsForbiddenChars(pass)) {
    return "Mật khẩu không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Gender (PHAI) - must be "Nam" or "Nữ"
 */
export function validateGender(phai: string): string | null {
  if (!phai || !phai.trim()) return "Giới tính không được để trống."
  const normalized = phai.trim()
  if (normalized !== "Nam" && normalized !== "Nữ") {
    return "Giới tính phải là 'Nam' hoặc 'Nữ'."
  }
  return null
}

/**
 * Validate Question Content (NOIDUNG)
 * Rule 4: Allow all chars (except control chars/pipes)
 */
export function validateQuestionContent(content: string): string | null {
  if (!content || !content.trim()) return "Nội dung câu hỏi không được để trống."
  if (containsForbiddenChars(content)) {
    return "Nội dung câu hỏi không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Question Option (A/B/C/D)
 * Rule 4: Allow all chars (except control chars/pipes)
 */
export function validateQuestionOption(opt: string, label: string): string | null {
  if (!opt || !opt.trim()) return `Đáp án ${label} không được để trống.`
  if (containsForbiddenChars(opt)) {
    return `Đáp án ${label} không được chứa ký tự cấm (|, tab, xuống dòng...).`
  }
  return null
}

/**
 * Auto-split student name so TÊN is strictly 1 single word,
 * and HỌ contains all preceding middle and family names.
 * Example:
 *   ho="Bùi", ten="Ngọc An" -> { ho: "Bùi Ngọc", ten: "An" }
 *   ho="", ten="Nguyễn Văn An" -> { ho: "Nguyễn Văn", ten: "An" }
 *   ho="Nguyễn Văn", ten="An" -> { ho: "Nguyễn Văn", ten: "An" }
 */
export function splitStudentName(hoInput: string, tenInput: string): { ho: string; ten: string } {
  const combined = `${hoInput.trim()} ${tenInput.trim()}`.trim()
  if (!combined) return { ho: '', ten: '' }

  const parts = combined.split(/\s+/).filter(Boolean)
  if (parts.length === 1) {
    return { ho: '', ten: toTitleCase(parts[0]) }
  }

  const ten = toTitleCase(parts.pop() || '')
  const ho = toTitleCase(parts.join(' '))
  return { ho, ten }
}

export const formatStudentNameFields = splitStudentName


