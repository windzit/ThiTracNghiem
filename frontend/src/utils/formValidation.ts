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
 * Normalization Pipeline for Code/PK fields (MASV, MALOP, MAMH):
 * trim() -> uppercase (no space collapse)
 */
export function normalizeIdentifier(value: string): string {
  return value.trim().toUpperCase()
}

/**
 * Normalization Pipeline for Code/PK legacy alias:
 */
export function normalizeCode(value: string): string {
  return normalizeIdentifier(value)
}

/**
 * Normalization Pipeline for Human-readable Text fields (TENLOP, TENMH, HO, TEN, NOIDUNG, A, B, C, D):
 * trim() -> collapse consecutive spaces -> preserve case
 */
export function normalizeText(value: string): string {
  return value.trim().replace(/\s+/g, ' ')
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

/**
 * Validate Class Code (MALOP)
 * Backend MALOP_CHECKER: length > 0 && length <= 15, no forbidden chars
 */
export function validateClassCode(code: string): string | null {
  const normalized = code.trim()
  if (!normalized) return "Mã lớp không được để trống."
  if (normalized.length > VALIDATION_CONSTANTS.MAX_CODE_LENGTH) {
    return `Mã lớp không vượt quá ${VALIDATION_CONSTANTS.MAX_CODE_LENGTH} ký tự.`
  }
  if (containsForbiddenChars(code)) {
    return "Mã lớp không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Class Name (TENLOP)
 * Backend TENLOP_CHECKER: length > 0, no forbidden chars
 */
export function validateClassName(name: string): string | null {
  if (!name.trim()) return "Tên lớp không được để trống."
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
 */
export function validateSubjectCode(code: string): string | null {
  const normalized = code.trim()
  if (!normalized) return "Mã môn không được để trống."
  if (normalized.length > VALIDATION_CONSTANTS.MAX_CODE_LENGTH) {
    return `Mã môn không vượt quá ${VALIDATION_CONSTANTS.MAX_CODE_LENGTH} ký tự.`
  }
  if (containsForbiddenChars(code)) {
    return "Mã môn không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Subject Name (TENMH)
 */
export function validateSubjectName(name: string): string | null {
  if (!name.trim()) return "Tên môn không được để trống."
  if (containsForbiddenChars(name)) {
    return "Tên môn không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Student ID (MASV)
 */
export function validateStudentId(masv: string): string | null {
  const normalized = masv.trim()
  if (!normalized) return "Mã sinh viên không được để trống."
  if (normalized.length > VALIDATION_CONSTANTS.MAX_CODE_LENGTH) {
    return `Mã sinh viên không vượt quá ${VALIDATION_CONSTANTS.MAX_CODE_LENGTH} ký tự.`
  }
  if (containsForbiddenChars(masv)) {
    return "Mã sinh viên không được chứa ký tự cấm (|, tab, xuống dòng...)."
  }
  return null
}

/**
 * Validate Student Name / HO / TEN
 */
export function validateStudentName(name: string): string | null {
  if (!name.trim()) return "Họ tên không được để trống."
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
 */
export function validateQuestionOption(opt: string, label: string): string | null {
  if (!opt || !opt.trim()) return `Đáp án ${label} không được để trống.`
  if (containsForbiddenChars(opt)) {
    return `Đáp án ${label} không được chứa ký tự cấm (|, tab, xuống dòng...).`
  }
  return null
}
