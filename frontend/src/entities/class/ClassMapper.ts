import type { BackendClass } from '@/shared/types/backend';
import type { ClassItem } from '@/shared/types';

export class ClassMapper {
  /**
   * Map BackendClass to Frontend ClassItem
   */
  static toFrontend(backend: BackendClass): ClassItem {
    let cleanName = backend.tenlop || '';
    
    // Clean up legacy custom year format if present
    const yearMatch = cleanName.match(/\[\d{4}\s*-\s*\d{4}\]$/);
    if (yearMatch) {
      cleanName = cleanName.replace(/\s*\[\d{4}\s*-\s*\d{4}\]$/, '').trim();
    }

    return {
      id: backend.malop,
      name: cleanName,
      dept: '',
      current: backend.siso || 0,
      total: backend.siso || 0,
      status: 'active',
    };
  }

  /**
   * Map Frontend ClassItem to BackendClass
   */
  static toBackend(frontend: ClassItem): BackendClass {
    return {
      malop: frontend.id,
      tenlop: frontend.name.trim(),
      siso: frontend.total || 0,
    };
  }

  /**
   * Map array of BackendClass to Frontend ClassItem[]
   */
  static toFrontendArray(backendArray: BackendClass[]): ClassItem[] {
    return backendArray.map(this.toFrontend);
  }

  /**
   * Map array of Frontend ClassItem to BackendClass[]
   */
  static toBackendArray(frontendArray: ClassItem[]): BackendClass[] {
    return frontendArray.map(this.toBackend);
  }
}