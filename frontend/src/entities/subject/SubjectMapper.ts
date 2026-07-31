import type { BackendSubject } from '@/shared/types/backend';
import type { Subject } from '@/shared/types';

export class SubjectMapper {
  /**
   * Map BackendSubject to Frontend Subject
   * Backend: mamh, tenmh, used
   * Frontend: id, code, name, questionCount, createdAt, status, description, note
   */
  static toFrontend(backend: BackendSubject): Subject {
    return {
      id: backend.mamh,
      code: backend.mamh,
      name: backend.tenmh,
      questionCount: backend.questionCount ?? 0,
      createdAt: new Date().toLocaleDateString('vi-VN'),
      status: backend.used ? 'hidden' : 'active',
      description: `Môn học ${backend.tenmh}`,
    };
  }

  /**
   * Map Frontend Subject to BackendSubject
   */
  static toBackend(frontend: Subject): BackendSubject {
    return {
      mamh: frontend.code,
      tenmh: frontend.name,
      used: frontend.status === 'hidden',
    };
  }

  /**
   * Map array
   */
  static toFrontendArray(backendArray: BackendSubject[]): Subject[] {
    return backendArray.map(this.toFrontend);
  }

  static toBackendArray(frontendArray: Subject[]): BackendSubject[] {
    return frontendArray.map(this.toBackend);
  }
}