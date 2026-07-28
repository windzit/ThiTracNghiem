// Notification types — dùng const object thay enum (vì tsconfig erasableSyntaxOnly)
export const NotificationType = {
  // Student notifications
  RESULT_PUBLISHED: "RESULT_PUBLISHED",
  
  // Teacher activities
  EXAM_CREATED: "EXAM_CREATED",
  RESULT_PUBLISHED_TEACHER: "RESULT_PUBLISHED_TEACHER",
  STUDENT_ADDED: "STUDENT_ADDED",
  SUBJECT_ADDED: "SUBJECT_ADDED",
  QUESTION_ADDED: "QUESTION_ADDED",
  REPORT_EXPORTED: "REPORT_EXPORTED",
} as const

export type NotificationTypeValue = (typeof NotificationType)[keyof typeof NotificationType]

export interface Notification {
  id: string
  type: NotificationTypeValue
  title: string
  description: string
  time: string
  isRead: boolean
  link: string
  icon: string
}

// Student notifications - chỉ có "Kết quả bài thi đã được công bố"
export const mockStudentNotifications: Notification[] = [
  {
    id: "notif-001",
    type: NotificationType.RESULT_PUBLISHED,
    title: "Kết quả thi đã được công bố",
    description: "Môn: Cấu trúc dữ liệu và giải thuật - Đề 1",
    time: "15 phút trước",
    isRead: false,
    link: "/student/exam-detail/exam-001",
    icon: "FileText",
  },
  {
    id: "notif-002",
    type: NotificationType.RESULT_PUBLISHED,
    title: "Kết quả thi đã được công bố",
    description: "Môn: Lập trình hướng đối tượng - Đề 2",
    time: "2 giờ trước",
    isRead: false,
    link: "/student/exam-detail/exam-002",
    icon: "FileText",
  },
  {
    id: "notif-003",
    type: NotificationType.RESULT_PUBLISHED,
    title: "Kết quả thi đã được công bố",
    description: "Môn: Cơ sở dữ liệu - Đề 1",
    time: "1 ngày trước",
    isRead: true,
    link: "/student/exam-detail/exam-003",
    icon: "FileText",
  },
  {
    id: "notif-004",
    type: NotificationType.RESULT_PUBLISHED,
    title: "Kết quả thi đã được công bố",
    description: "Môn: Mạng máy tính - Đề 1",
    time: "3 ngày trước",
    isRead: true,
    link: "/student/exam-detail/exam-004",
    icon: "FileText",
  },
  {
    id: "notif-005",
    type: NotificationType.RESULT_PUBLISHED,
    title: "Kết quả thi đã được công bố",
    description: "Môn: Toán rời rạc - Đề 1",
    time: "1 tuần trước",
    isRead: true,
    link: "/student/exam-detail/exam-005",
    icon: "FileText",
  },
]

// Teacher activities
export const mockTeacherActivities: Notification[] = [
  {
    id: "activity-001",
    type: NotificationType.EXAM_CREATED,
    title: "Bạn đã tạo bài thi mới",
    description: "Cấu trúc dữ liệu và giải thuật (Đề 2)",
    time: "10:30 AM",
    isRead: false,
    link: "/teacher/exam-preview/exam-006",
    icon: "ClipboardList",
  },
  {
    id: "activity-002",
    type: NotificationType.RESULT_PUBLISHED_TEACHER,
    title: "Bài thi đã được công bố",
    description: "Lập trình hướng đối tượng (Đề 1)",
    time: "09:15 AM",
    isRead: false,
    link: "/teacher/exam-results/exam-002",
    icon: "CheckCircle2",
  },
  {
    id: "activity-003",
    type: NotificationType.STUDENT_ADDED,
    title: "Thêm sinh viên vào lớp",
    description: "Vũ Thùy Linh vào lớp D24CQCE02-N",
    time: "Hôm qua, 16:45",
    isRead: true,
    link: "/teacher/classes/D24CQCE02-N",
    icon: "UserPlus",
  },
  {
    id: "activity-004",
    type: NotificationType.QUESTION_ADDED,
    title: "Thêm câu hỏi mới",
    description: "Trong môn Cơ sở dữ liệu",
    time: "Hôm qua, 14:20",
    isRead: true,
    link: "/teacher/questions",
    icon: "HelpCircle",
  },
  {
    id: "activity-005",
    type: NotificationType.REPORT_EXPORTED,
    title: "Xuất báo cáo kết quả",
    description: "Bảng điểm lớp D24CQCE01-N",
    time: "Hôm qua, 11:05",
    isRead: true,
    link: "/teacher/gradebook",
    icon: "TrendingUp",
  },
  {
    id: "activity-006",
    type: NotificationType.SUBJECT_ADDED,
    title: "Thêm môn học mới",
    description: "Trí tuệ nhân tạo",
    time: "2 ngày trước",
    isRead: true,
    link: "/teacher/subjects",
    icon: "BookOpen",
  },
  {
    id: "activity-007",
    type: NotificationType.EXAM_CREATED,
    title: "Bạn đã tạo bài thi mới",
    description: "Mạng máy tính (Đề 3)",
    time: "3 ngày trước",
    isRead: true,
    link: "/teacher/exam-preview/exam-007",
    icon: "ClipboardList",
  },
]