import axios from 'axios';

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL !== undefined ? import.meta.env.VITE_API_BASE_URL : 'http://localhost:8080';

let onNetworkErrorCallback: ((disconnected: boolean) => void) | null = null;

export const setNetworkErrorCallback = (cb: (disconnected: boolean) => void) => {
  onNetworkErrorCallback = cb;
};

export const apiClient = axios.create({
  baseURL: API_BASE_URL,
  headers: {
    'Content-Type': 'application/json',
  },
  timeout: 10000, // 10 seconds timeout
});

// Request interceptor - log payload
apiClient.interceptors.request.use(
  (config) => {
    console.log(`[HTTP Request] ${config.method?.toUpperCase()} ${config.url}`, {
      headers: config.headers,
      data: config.data,
      params: config.params
    });
    return config;
  },
  (error) => {
    console.error(`[HTTP Request Error]`, error);
    return Promise.reject(error);
  }
);

// Response interceptor - log status and body
apiClient.interceptors.response.use(
  (response) => {
    console.log(`[HTTP Response] ${response.config.method?.toUpperCase()} ${response.config.url} - Status: ${response.status}`, {
      body: response.data
    });

    // Server responded successfully -> reset network error state
    if (onNetworkErrorCallback) {
      onNetworkErrorCallback(false);
    }

    // 204 No Content (DELETE responses)
    if (response.status === 204 || !response.data) {
      return null;
    }
    // Unwrap {success: true, data: ...} envelope từ backend
    if (response.data && typeof response.data === 'object' && 'success' in response.data) {
      return response.data.data;
    }
    // Fallback: trả về response.data nếu không có envelope
    return response.data;
  },
  (error) => {
    if (error.response) {
      console.error(`[HTTP Response Error] ${error.config?.method?.toUpperCase()} ${error.config?.url} - Status: ${error.response.status}`, {
        body: error.response.data
      });
      // Server error (502, 503, 504) -> trigger disconnected overlay
      if (error.response.status >= 502) {
        if (onNetworkErrorCallback) {
          onNetworkErrorCallback(true);
        }
      }
    } else if (error.request) {
      console.error('[HTTP Response Error] No response received:', error.request);
      // Network failure / server offline -> trigger disconnected overlay
      if (onNetworkErrorCallback) {
        onNetworkErrorCallback(true);
      }
    } else {
      console.error('[HTTP Response Error] Setting up request failed:', error.message);
    }
    
    return Promise.reject(error);
  }
);

// Helper functions cho các HTTP methods
export const api = {
  get: <T = any>(url: string, params?: any): Promise<T> => {
    return apiClient.get(url, { params }) as Promise<T>;
  },

  post: <T = any>(url: string, data?: any): Promise<T> => {
    return apiClient.post(url, data) as Promise<T>;
  },

  put: <T = any>(url: string, data?: any): Promise<T> => {
    return apiClient.put(url, data) as Promise<T>;
  },

  delete: <T = any>(url: string, params?: any): Promise<T> => {
    return apiClient.delete(url, { params }) as Promise<T>;
  },
};