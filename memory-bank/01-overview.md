# 01 — Overview

## Project Purpose
Xây dựng phần mềm thi trắc nghiệm (Computer-Based Test — CBT) dạng ứng dụng console C++, phục vụ một cơ sở đào tạo tổ chức kiểm tra trắc nghiệm cho sinh viên theo môn học, theo lớp.

**Confidence**: Verified (from debai.md + source code)

## Stakeholders
| Stakeholder | Role |
|------------|------|
| Sinh viên (SV) | Người học, làm bài thi, xem kết quả |
| Giáo viên/Quản trị (GV) | Toàn quyền quản trị dữ liệu, xem báo cáo |
| Hệ thống | Tự sinh Id, chấm điểm, đọc/ghi file |

**Confidence**: Verified

## Scope
**In scope:**
- Quản lý môn học (BST), lớp (mảng con trỏ), sinh viên (linked list), câu hỏi (linked list), điểm thi (linked list)
- Thi trắc nghiệm có bấm giờ, chấm điểm tự động
- Xem chi tiết bài thi và bảng điểm theo lớp
- Lưu trữ bằng file TXT

**Out of scope:**
- Thi đồng thời nhiều sinh viên qua mạng
- Giao diện đồ hoạ (console only per assignment)
- Xác thực sinh trắc học, chống gian lận AI

**Confidence**: Verified

## Authoritative Documents
| Document | Role |
|----------|------|
| `document/debai.md` | **Primary** assignment specification (source of truth) |
| `document/TongQuan.md` | Secondary BA analysis (older version, some differences) |
| `document/UI_goiy.md` | Duplicate of TongQuan.md |
| `document/Chitiet_1.md` | Detailed spec for functions a/ through e/ |
| `document/Chitiet_2.md` | Detailed spec for functions f/ through i/ |

**Confidence**: Verified