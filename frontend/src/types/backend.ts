// Backend API Response Types
// Định nghĩa theo đúng API contract từ server.cpp

export interface ApiResponse<T = any> {
  success: boolean;
  data?: T;
  message?: string;
}

// Class/Lớp học
export interface BackendClass {
  malop: string;
  tenlop: string;
  siso: number;
}

// Student/Sinh viên
export interface BackendStudent {
  masv: string;
  ho: string;
  ten: string;
  phai: string;
  malop: string;
  tenlop?: string;
  examCount?: number;
  scores?: Array<{ mamh: string; tenmh: string; diem: number }>;
}

// Subject/Môn học
export interface BackendSubject {
  mamh: string;
  tenmh: string;
  used: boolean;
  questionCount?: number;
}

// Question/Câu hỏi
export interface BackendQuestion {
  id: number;
  noidung: string;
  A: string;
  B: string;
  C: string;
  D: string;
  dapan: string;
  used: boolean;
  deleted?: boolean;
}

// Subject with Questions (từ GET /api/subjects/:id)
export interface BackendSubjectWithQuestions extends BackendSubject {
  questions: BackendQuestion[];
}

// Login Request
export interface LoginRequest {
  username: string;
  password: string;
  role: 'student' | 'teacher';
}

// Login Response (Student)
export interface LoginStudentResponse {
  role: 'student';
  masv: string;
  ho: string;
  ten: string;
  phai: string;
  malop: string;
}

// Login Response (Teacher)
export interface LoginTeacherResponse {
  role: 'teacher';
  username: string;
}

// Create Class Request
export interface CreateClassRequest {
  malop: string;
  tenlop: string;
}

// Update Class Request
export interface UpdateClassRequest {
  tenlop: string;
}

// Create Student Request
export interface CreateStudentRequest {
  malop: string;
  masv: string;
  ho: string;
  ten: string;
  phai: string;
  password: string;
}

// Update Student Request
export interface UpdateStudentRequest {
  ho?: string;
  ten?: string;
  phai?: string;
  password?: string;
}

// Create Question Request
export interface CreateQuestionRequest {
  mamh: string;
  noidung: string;
  A: string;
  B: string;
  C: string;
  D: string;
  dapan: string;
}

// Update Question Request
export interface UpdateQuestionRequest {
  mamh: string;
  noidung?: string;
  A?: string;
  B?: string;
  C?: string;
  D?: string;
  dapan?: string;
}