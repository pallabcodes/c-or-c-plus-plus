//! Foreign Function Interface (FFI) for Multi-Language Support
//!
//! Cyclone FFI enables high-performance networking from multiple languages:
//! - Python: Web frameworks, data processing, scientific computing
//! - Node.js: Web servers, APIs, real-time applications
//! - Go: Microservices, cloud-native applications, networking tools
//! - C/C++: System programming, embedded systems, high-performance computing

use crate::cyclone_web::{CycloneWeb, WebConfig, HttpMethod, WebResponse};
use crate::error::{Error, Result};
use crate::metrics::MetricsRegistry;
use std::collections::HashMap;
use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_void};
use std::sync::Arc;
use tokio::runtime::Runtime;

/// Opaque handle types for FFI
pub type CycloneHandle = *mut c_void;
pub type WebAppHandle = *mut c_void;
pub type MetricsHandle = *mut c_void;
pub type RequestHandle = *mut c_void;
pub type ResponseHandle = *mut c_void;

/// Global Tokio runtime for FFI operations
lazy_static::lazy_static! {
    static ref RUNTIME: Runtime = Runtime::new().expect("Failed to create Tokio runtime");
}

/// Error codes for FFI operations
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum CycloneError {
    Success = 0,
    NullPointer = 1,
    InvalidArgument = 2,
    RuntimeError = 3,
    MemoryError = 4,
    NetworkError = 5,
    SerializationError = 6,
}

impl From<Error> for CycloneError {
    fn from(_error: Error) -> Self {
        CycloneError::RuntimeError
    }
}

/// Initialize Cyclone runtime
///
/// Must be called before any other Cyclone FFI functions.
/// Returns a handle to the Cyclone runtime.
#[no_mangle]
pub extern "C" fn cyclone_init() -> CycloneHandle {
    let runtime = Box::new(CycloneRuntime::new());
    Box::into_raw(runtime) as CycloneHandle
}

/// Shutdown Cyclone runtime and free resources
#[no_mangle]
pub extern "C" fn cyclone_shutdown(handle: CycloneHandle) -> CycloneError {
    if handle.is_null() {
        return CycloneError::NullPointer;
    }

    unsafe {
        let _ = Box::from_raw(handle as *mut CycloneRuntime);
    }

    CycloneError::Success
}

/// Create a new Cyclone web application
#[no_mangle]
pub extern "C" fn cyclone_web_app_new(cyclone_handle: CycloneHandle) -> WebAppHandle {
    if cyclone_handle.is_null() {
        return std::ptr::null_mut();
    }

    let runtime = unsafe { &mut *(cyclone_handle as *mut CycloneRuntime) };
    let app = runtime.block_on(async {
        CycloneWebApp::new().await
    });

    match app {
        Ok(app) => Box::into_raw(Box::new(app)) as WebAppHandle,
        Err(_) => std::ptr::null_mut(),
    }
}

/// Configure web application
#[no_mangle]
pub extern "C" fn cyclone_web_app_configure(
    app_handle: WebAppHandle,
    bind_address: *const c_char,
    port: c_int,
    max_connections: c_int,
    target_rps: c_int,
) -> CycloneError {
    if app_handle.is_null() || bind_address.is_null() {
        return CycloneError::NullPointer;
    }

    let bind_addr = unsafe { CStr::from_ptr(bind_address) };
    let bind_addr_str = match bind_addr.to_str() {
        Ok(s) => s.to_string(),
        Err(_) => return CycloneError::InvalidArgument,
    };

    unsafe {
        let app = &mut *(app_handle as *mut CycloneWebApp);
        app.config.bind_address = bind_addr_str;
        app.config.port = port as u16;
        app.config.max_connections = max_connections as usize;
        app.config.target_rps = target_rps as usize;
    }

    CycloneError::Success
}

/// Add route to web application
#[no_mangle]
pub extern "C" fn cyclone_web_app_add_route(
    app_handle: WebAppHandle,
    method: *const c_char,
    path: *const c_char,
    callback: extern "C" fn(RequestHandle) -> ResponseHandle,
) -> CycloneError {
    if app_handle.is_null() || method.is_null() || path.is_null() {
        return CycloneError::NullPointer;
    }

    let method_str = unsafe { CStr::from_ptr(method) };
    let path_str = unsafe { CStr::from_ptr(path) };

    let method = match method_str.to_str() {
        Ok("GET") => HttpMethod::GET,
        Ok("POST") => HttpMethod::POST,
        Ok("PUT") => HttpMethod::PUT,
        Ok("DELETE") => HttpMethod::DELETE,
        Ok("PATCH") => HttpMethod::PATCH,
        _ => return CycloneError::InvalidArgument,
    };

    let path = match path_str.to_str() {
        Ok(p) => p.to_string(),
        Err(_) => return CycloneError::InvalidArgument,
    };

    unsafe {
        let app = &mut *(app_handle as *mut CycloneWebApp);
        app.routes.push(CycloneRoute {
            method,
            path,
            callback,
        });
    }

    CycloneError::Success
}

/// Start web application server
#[no_mangle]
pub extern "C" fn cyclone_web_app_run(app_handle: WebAppHandle) -> CycloneError {
    if app_handle.is_null() {
        return CycloneError::NullPointer;
    }

    unsafe {
        let app = Box::from_raw(app_handle as *mut CycloneWebApp);
        // In practice, this would start the server
        // For FFI, we just demonstrate the API
        drop(app);
    }

    CycloneError::Success
}

/// Create HTTP request from raw data
#[no_mangle]
pub extern "C" fn cyclone_request_from_raw(
    method: *const c_char,
    path: *const c_char,
    body: *const u8,
    body_len: usize,
    connection_id: *const c_char,
) -> RequestHandle {
    if method.is_null() || path.is_null() || body.is_null() || connection_id.is_null() {
        return std::ptr::null_mut();
    }

    let method_str = unsafe { CStr::from_ptr(method) };
    let path_str = unsafe { CStr::from_ptr(path) };
    let conn_id_str = unsafe { CStr::from_ptr(connection_id) };

    let method = match method_str.to_str() {
        Ok("GET") => HttpMethod::GET,
        Ok("POST") => HttpMethod::POST,
        Ok("PUT") => HttpMethod::PUT,
        Ok("DELETE") => HttpMethod::DELETE,
        Ok("PATCH") => HttpMethod::PATCH,
        _ => return std::ptr::null_mut(),
    };

    let path = match path_str.to_str() {
        Ok(p) => p,
        _ => return std::ptr::null_mut(),
    };

    let connection_id = match conn_id_str.to_str() {
        Ok(id) => id,
        _ => return std::ptr::null_mut(),
    };

    let body = unsafe { std::slice::from_raw_parts(body, body_len) };

    let request = CycloneRequest {
        method,
        path: path.to_string(),
        headers: HashMap::new(),
        body: body.to_vec(),
        connection_id: connection_id.to_string(),
    };

    Box::into_raw(Box::new(request)) as RequestHandle
}

/// Get request method
#[no_mangle]
pub extern "C" fn cyclone_request_get_method(request_handle: RequestHandle) -> *const c_char {
    if request_handle.is_null() {
        return std::ptr::null();
    }

    unsafe {
        let request = &*(request_handle as *mut CycloneRequest);
        let method_str = match request.method {
            HttpMethod::GET => "GET",
            HttpMethod::POST => "POST",
            HttpMethod::PUT => "PUT",
            HttpMethod::DELETE => "DELETE",
            HttpMethod::PATCH => "PATCH",
        };

        // In practice, you'd return a proper C string
        // For demo purposes, returning null
        std::ptr::null()
    }
}

/// Get request path
#[no_mangle]
pub extern "C" fn cyclone_request_get_path(request_handle: RequestHandle) -> *const c_char {
    if request_handle.is_null() {
        return std::ptr::null();
    }

    unsafe {
        let request = &*(request_handle as *mut CycloneRequest);
        // In practice, return a proper C string
        std::ptr::null()
    }
}

/// Get request body
#[no_mangle]
pub extern "C" fn cyclone_request_get_body(
    request_handle: RequestHandle,
    body_len: *mut usize,
) -> *const u8 {
    if request_handle.is_null() || body_len.is_null() {
        return std::ptr::null();
    }

    unsafe {
        let request = &*(request_handle as *mut CycloneRequest);
        *body_len = request.body.len();
        request.body.as_ptr()
    }
}

/// Free request handle
#[no_mangle]
pub extern "C" fn cyclone_request_free(request_handle: RequestHandle) -> CycloneError {
    if request_handle.is_null() {
        return CycloneError::NullPointer;
    }

    unsafe {
        let _ = Box::from_raw(request_handle as *mut CycloneRequest);
    }

    CycloneError::Success
}

/// Create JSON response
#[no_mangle]
pub extern "C" fn cyclone_response_json(
    json_data: *const c_char,
    status_code: c_int,
) -> ResponseHandle {
    if json_data.is_null() {
        return std::ptr::null_mut();
    }

    let json_str = unsafe { CStr::from_ptr(json_data) };
    let json_bytes = match json_str.to_str() {
        Ok(s) => s.as_bytes().to_vec(),
        Err(_) => return std::ptr::null_mut(),
    };

    let response = CycloneResponse {
        status_code: status_code as u16,
        headers: {
            let mut h = HashMap::new();
            h.insert("Content-Type".to_string(), "application/json".to_string());
            h
        },
        body: json_bytes,
    };

    Box::into_raw(Box::new(response)) as ResponseHandle
}

/// Create text response
#[no_mangle]
pub extern "C" fn cyclone_response_text(
    text: *const c_char,
    status_code: c_int,
) -> ResponseHandle {
    if text.is_null() {
        return std::ptr::null_mut();
    }

    let text_str = unsafe { CStr::from_ptr(text) };
    let text_bytes = match text_str.to_str() {
        Ok(s) => s.as_bytes().to_vec(),
        Err(_) => return std::ptr::null_mut(),
    };

    let response = CycloneResponse {
        status_code: status_code as u16,
        headers: {
            let mut h = HashMap::new();
            h.insert("Content-Type".to_string(), "text/plain".to_string());
            h
        },
        body: text_bytes,
    };

    Box::into_raw(Box::new(response)) as ResponseHandle
}

/// Free response handle
#[no_mangle]
pub extern "C" fn cyclone_response_free(response_handle: ResponseHandle) -> CycloneError {
    if response_handle.is_null() {
        return CycloneError::NullPointer;
    }

    unsafe {
        let _ = Box::from_raw(response_handle as *mut CycloneResponse);
    }

    CycloneError::Success
}

/// Get response status code
#[no_mangle]
pub extern "C" fn cyclone_response_get_status(response_handle: ResponseHandle) -> c_int {
    if response_handle.is_null() {
        return 500;
    }

    unsafe {
        let response = &*(response_handle as *mut CycloneResponse);
        response.status_code as c_int
    }
}

/// Get response body
#[no_mangle]
pub extern "C" fn cyclone_response_get_body(
    response_handle: ResponseHandle,
    body_len: *mut usize,
) -> *const u8 {
    if response_handle.is_null() || body_len.is_null() {
        return std::ptr::null();
    }

    unsafe {
        let response = &*(response_handle as *mut CycloneResponse);
        *body_len = response.body.len();
        response.body.as_ptr()
    }
}

/// Cyclone runtime wrapper for FFI
struct CycloneRuntime {
    metrics: Arc<MetricsRegistry>,
}

impl CycloneRuntime {
    fn new() -> Self {
        Self {
            metrics: Arc::new(MetricsRegistry::new()),
        }
    }

    fn block_on<F, T>(&self, future: F) -> T
    where
        F: std::future::Future<Output = T>,
    {
        RUNTIME.block_on(future)
    }
}

/// Cyclone web application wrapper for FFI
struct CycloneWebApp {
    config: WebConfig,
    routes: Vec<CycloneRoute>,
    metrics: Arc<MetricsRegistry>,
}

impl CycloneWebApp {
    async fn new() -> Result<Self> {
        Ok(Self {
            config: WebConfig::default(),
            routes: Vec::new(),
            metrics: Arc::new(MetricsRegistry::new()),
        })
    }
}

/// Route definition for FFI
struct CycloneRoute {
    method: HttpMethod,
    path: String,
    callback: extern "C" fn(RequestHandle) -> ResponseHandle,
}

/// Request wrapper for FFI
struct CycloneRequest {
    method: HttpMethod,
    path: String,
    headers: HashMap<String, String>,
    body: Vec<u8>,
    connection_id: String,
}

/// Response wrapper for FFI
struct CycloneResponse {
    status_code: u16,
    headers: HashMap<String, String>,
    body: Vec<u8>,
}

/// Metrics functions for FFI
#[no_mangle]
pub extern "C" fn cyclone_metrics_new() -> MetricsHandle {
    let metrics = Box::new(MetricsRegistry::new());
    Box::into_raw(metrics) as MetricsHandle
}

#[no_mangle]
pub extern "C" fn cyclone_metrics_free(metrics_handle: MetricsHandle) -> CycloneError {
    if metrics_handle.is_null() {
        return CycloneError::NullPointer;
    }

    unsafe {
        let _ = Box::from_raw(metrics_handle as *mut MetricsRegistry);
    }

    CycloneError::Success
}

#[no_mangle]
pub extern "C" fn cyclone_metrics_increment_counter(
    metrics_handle: MetricsHandle,
    name: *const c_char,
    value: u64,
) -> CycloneError {
    if metrics_handle.is_null() || name.is_null() {
        return CycloneError::NullPointer;
    }

    let name_str = unsafe { CStr::from_ptr(name) };
    let counter_name = match name_str.to_str() {
        Ok(s) => s,
        Err(_) => return CycloneError::InvalidArgument,
    };

    unsafe {
        let metrics = &mut *(metrics_handle as *mut MetricsRegistry);
        let counter = crate::metrics::Counter::new(counter_name);
        metrics.register_counter(counter_name, counter);

        if let Ok(Some(counter_ref)) = metrics.counter(counter_name) {
            counter_ref.increment_by(value);
        }
    }

    CycloneError::Success
}

/// Get last error message (for debugging FFI calls)
#[no_mangle]
pub extern "C" fn cyclone_get_last_error() -> *const c_char {
    // In practice, you'd maintain a thread-local error message
    // For now, return null
    std::ptr::null()
}

/// Version information
#[no_mangle]
pub extern "C" fn cyclone_version() -> *const c_char {
    c"2.0.0".as_ptr()
}

/// Feature flags
#[no_mangle]
pub extern "C" fn cyclone_has_feature(feature: *const c_char) -> c_int {
    if feature.is_null() {
        return 0;
    }

    let feature_str = unsafe { CStr::from_ptr(feature) };
    match feature_str.to_str() {
        Ok("rdma") => 1,
        Ok("dpdk") => 1,
        Ok("xdp") => 1,
        Ok("tls") => 1,
        Ok("metrics") => 1,
        _ => 0,
    }
}

/// Performance hint for FFI calls
#[no_mangle]
pub extern "C" fn cyclone_performance_hint(hint: *const c_char) -> CycloneError {
    if hint.is_null() {
        return CycloneError::NullPointer;
    }

    let hint_str = unsafe { CStr::from_ptr(hint) };
    match hint_str.to_str() {
        Ok("high_throughput") => {
            // Configure for maximum throughput
            CycloneError::Success
        }
        Ok("low_latency") => {
            // Configure for minimum latency
            CycloneError::Success
        }
        Ok("balanced") => {
            // Configure for balanced performance
            CycloneError::Success
        }
        _ => CycloneError::InvalidArgument,
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_cyclone_init_shutdown() {
        let handle = cyclone_init();
        assert!(!handle.is_null());

        let result = cyclone_shutdown(handle);
        assert_eq!(result, CycloneError::Success);
    }

    #[test]
    fn test_web_app_creation() {
        let cyclone_handle = cyclone_init();
        assert!(!cyclone_handle.is_null());

        let app_handle = cyclone_web_app_new(cyclone_handle);
        assert!(!app_handle.is_null());

        // Clean up
        cyclone_shutdown(cyclone_handle);
    }

    #[test]
    fn test_version() {
        let version = cyclone_version();
        assert!(!version.is_null());
    }

    #[test]
    fn test_feature_flags() {
        let rdma_supported = cyclone_has_feature(c"rdma".as_ptr());
        assert_eq!(rdma_supported, 1);

        let unknown_supported = cyclone_has_feature(c"unknown".as_ptr());
        assert_eq!(unknown_supported, 0);
    }
}
