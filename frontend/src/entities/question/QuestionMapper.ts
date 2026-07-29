import type { BackendQuestion } from '@/types/backend';
import type { Question } from '@/types';
import { Difficulty } from '@/data/difficulty';

export class QuestionMapper {
  /**
   * Map BackendQuestion to Frontend Question
   * Backend: id, noidung, A, B, C, D, dapan, used, deleted
   * Frontend: id, content, subText, options[], correctAnswer, subject, type, status, difficulty, chapter, explanation, used, deleted
   */
  static toFrontend(backend: BackendQuestion, subjectName: string = '', mamh: string = ''): Question {
    // Map dapan (A/B/C/D) to correctAnswer index (0/1/2/3)
    const answerMap: Record<string, number> = {
      'A': 0,
      'B': 1,
      'C': 2,
      'D': 3,
    };

    // Business fields — direct from backend
    const used = backend.used === true;
    const deleted = backend.deleted === true;

    // Status is presentation only — derived from business fields
    const status: 'used' | 'unused' = deleted ? 'unused' : (used ? 'used' : 'unused');

    return {
      id: String(backend.id),
      content: backend.noidung,
      options: [
        { label: 'A', text: backend.A },
        { label: 'B', text: backend.B },
        { label: 'C', text: backend.C },
        { label: 'D', text: backend.D },
      ],
      correctAnswer: answerMap[backend.dapan] || 0,
      subject: subjectName,
      type: 'Trắc nghiệm',
      status: status,
      difficulty: Difficulty.MEDIUM,
      used: used,
      deleted: deleted,
      mamh: mamh || undefined,
    };
  }

  /**
   * Map Frontend Question to BackendQuestion
   */
  static toBackend(frontend: Question): BackendQuestion {
    // Map correctAnswer index to dapan (A/B/C/D)
    const indexMap: Record<number, string> = {
      0: 'A',
      1: 'B',
      2: 'C',
      3: 'D',
    };

    return {
      id: parseInt(frontend.id, 10),
      noidung: frontend.content,
      A: frontend.options[0]?.text || '',
      B: frontend.options[1]?.text || '',
      C: frontend.options[2]?.text || '',
      D: frontend.options[3]?.text || '',
      dapan: indexMap[frontend.correctAnswer] || 'A',
      used: frontend.used,
    };
  }

  /**
   * Map array
   */
  static toFrontendArray(backendArray: BackendQuestion[], subjectName: string = '', mamh: string = ''): Question[] {
    return backendArray.map(q => this.toFrontend(q, subjectName, mamh));
  }

  static toBackendArray(frontendArray: Question[]): BackendQuestion[] {
    return frontendArray.map(this.toBackend);
  }
}