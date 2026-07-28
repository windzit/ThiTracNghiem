export interface UserFriendlyError {
  code: string;
  message: string;
  statusCode?: number;
}

export class ApiErrorHandler {
  static handle(error: any): UserFriendlyError {
    // Server responded with error status
    if (error.response) {
      const status = error.response.status;
      const message = error.response.data?.message;
      
      // Log chi tiết cho debug (không hiển thị cho user)
      console.error(`API Error [${status}]:`, {
        url: error.config?.url,
        method: error.config?.method,
        data: error.response.data,
      });
      
      // Trả về message thân thiện theo status code
      switch (status) {
        case 400:
          return {
            code: 'BAD_REQUEST',
            message: message || 'Dữ liệu không hợp lệ',
            statusCode: status,
          };
        
        case 401:
          return {
            code: 'UNAUTHORIZED',
            message: 'Phiên đăng nhập hết hạn hoặc thông tin đăng nhập không đúng',
            statusCode: status,
          };
        
        case 403:
          return {
            code: 'FORBIDDEN',
            message: 'Bạn không có quyền thực hiện thao tác này',
            statusCode: status,
          };
        
        case 404:
          return {
            code: 'NOT_FOUND',
            message: message || 'Không tìm thấy dữ liệu',
            statusCode: status,
          };
        
        case 409:
          return {
            code: 'CONFLICT',
            message: message || 'Dữ liệu đã tồn tại',
            statusCode: status,
          };
        
        case 422:
          return {
            code: 'VALIDATION_ERROR',
            message: message || 'Dữ liệu không hợp lệ',
            statusCode: status,
          };
        
        case 500:
          return {
            code: 'SERVER_ERROR',
            message: 'Lỗi hệ thống, vui lòng thử lại sau',
            statusCode: status,
          };
        
        case 502:
          return {
            code: 'BAD_GATEWAY',
            message: 'Server tạm thời không khả dụng',
            statusCode: status,
          };
        
        case 503:
          return {
            code: 'SERVICE_UNAVAILABLE',
            message: 'Dịch vụ tạm thời không khả dụng',
            statusCode: status,
          };
        
        default:
          return {
            code: 'UNKNOWN_ERROR',
            message: message || 'Có lỗi xảy ra, vui lòng thử lại',
            statusCode: status,
          };
      }
    }
    
    // Request được gửi nhưng không nhận được response
    if (error.request) {
      console.error('Network Error: No response received', {
        url: error.config?.url,
        method: error.config?.method,
        request: error.request,
      });
      
      return {
        code: 'NETWORK_ERROR',
        message: 'Không thể kết nối đến server. Vui lòng kiểm tra kết nối mạng.',
      };
    }
    
    // Error khi setup request
    console.error('Request Error:', error.message);
    
    return {
      code: 'REQUEST_ERROR',
      message: 'Có lỗi xảy ra khi gửi yêu cầu',
    };
  }
  
  // Helper để check nếu là lỗi cụ thể
  static isNetworkError(error: any): boolean {
    return !error.response && !error.request;
  }
  
  static isServerError(error: any): boolean {
    return error.response && error.response.status >= 500;
  }
  
  static isClientError(error: any): boolean {
    return error.response && error.response.status >= 400 && error.response.status < 500;
  }
}