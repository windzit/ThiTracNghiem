import type { BackendStudent } from '@/types/backend';
import type { Student } from '@/types';

export class StudentMapper {
  /**
   * Map BackendStudent to Frontend Student
   * Backend: masv, ho, ten, phai, malop
   */
  static toFrontend(backend: BackendStudent): Student {
    const scores = backend.scores ? backend.scores.map(s => ({
      subjectCode: s.mamh,
      subjectName: s.tenmh,
      score: typeof s.diem === 'number' ? s.diem : parseFloat(s.diem as any)
    })) : [];

    const ho = backend.ho || '';
    const ten = backend.ten || '';

    return {
      id: backend.masv,
      ho: ho,
      ten: ten,
      name: `${ho} ${ten}`.trim(),
      classCode: backend.malop,
      gender: (backend.phai as "Nam" | "Nữ") || "Nam",
      examCount: backend.examCount ?? scores.length,
      scores: scores,
    };
  }

  /**
   * Map Frontend Student to BackendStudent
   */
  static toBackend(frontend: Student): BackendStudent {
    let ho = frontend.ho || '';
    let ten = frontend.ten || '';

    if (!ho && !ten && frontend.name) {
      const nameParts = frontend.name.trim().split(' ');
      ten = nameParts.pop() || '';
      ho = nameParts.join(' ');
    }

    return {
      masv: frontend.id,
      ho: ho,
      ten: ten,
      phai: frontend.gender || 'Nam',
      malop: frontend.classCode,
    };
  }

  /**
   * Map array
   */
  static toFrontendArray(backendArray: BackendStudent[]): Student[] {
    return backendArray.map(this.toFrontend);
  }

  static toBackendArray(frontendArray: Student[]): BackendStudent[] {
    return frontendArray.map(this.toBackend);
  }
}