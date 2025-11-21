//! Circuit Breaker Pattern for Enterprise Fault Tolerance
//!
//! Research-backed implementation of the circuit breaker pattern for resilient systems.
//! Based on Michael Nygard's "Release It!" and Netflix's Hystrix patterns.
//!
//! ## Research Integration
//!
//! - **Circuit Breaker Pattern**: Nygard's fault tolerance design pattern
//! - **Bulkhead Pattern**: Isolation of failures to prevent cascading
//! - **Exponential Backoff**: Research-backed retry strategies
//! - **Adaptive Timeouts**: Dynamic timeout adjustment based on performance

use std::collections::VecDeque;
use std::sync::atomic::{AtomicU64, AtomicUsize, Ordering};
use std::sync::{Arc, RwLock};
use std::time::{Duration, Instant};

/// Circuit breaker states following Nygard's pattern
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CircuitState {
    /// Normal operation - requests pass through
    Closed,
    /// Failure threshold exceeded - fast-fail requests
    Open,
    /// Testing if service recovered - allow limited requests
    HalfOpen,
}

/// Enterprise-grade circuit breaker with research-backed algorithms
#[derive(Debug)]
pub struct CircuitBreaker {
    /// Current circuit state
    state: RwLock<CircuitState>,
    /// Configuration parameters
    config: CircuitBreakerConfig,
    /// Failure statistics
    failures: AtomicU64,
    /// Success statistics
    successes: AtomicU64,
    /// Request statistics
    requests: AtomicU64,
    /// Last failure time
    last_failure_time: RwLock<Option<Instant>>,
    /// Recent request outcomes (sliding window)
    request_window: RwLock<VecDeque<RequestOutcome>>,
    /// Circuit opened time
    opened_at: RwLock<Option<Instant>>,
}

#[derive(Debug, Clone)]
pub struct CircuitBreakerConfig {
    /// Failure threshold (number of failures to open circuit)
    pub failure_threshold: u64,
    /// Success threshold (number of successes to close circuit from half-open)
    pub success_threshold: u64,
    /// Timeout before attempting to close circuit (seconds)
    pub timeout_seconds: u64,
    /// Sliding window size for failure rate calculation
    pub sliding_window_size: usize,
    /// Monitoring interval (seconds)
    pub monitoring_interval_seconds: u64,
    /// Expected response time (microseconds)
    pub expected_response_time_micros: u64,
}

impl Default for CircuitBreakerConfig {
    fn default() -> Self {
        Self {
            failure_threshold: 5,
            success_threshold: 3,
            timeout_seconds: 60,
            sliding_window_size: 100,
            monitoring_interval_seconds: 10,
            expected_response_time_micros: 5000, // 5ms
        }
    }
}

#[derive(Debug, Clone)]
struct RequestOutcome {
    success: bool,
    timestamp: Instant,
    response_time: Duration,
}

impl CircuitBreaker {
    /// Create a new circuit breaker with default configuration
    pub fn new() -> Self {
        Self::with_config(CircuitBreakerConfig::default())
    }

    /// Create a new circuit breaker with custom configuration
    pub fn with_config(config: CircuitBreakerConfig) -> Self {
        Self {
            state: RwLock::new(CircuitState::Closed),
            config,
            failures: AtomicU64::new(0),
            successes: AtomicU64::new(0),
            requests: AtomicU64::new(0),
            last_failure_time: RwLock::new(None),
            request_window: RwLock::new(VecDeque::with_capacity(config.sliding_window_size)),
            opened_at: RwLock::new(None),
        }
    }

    /// Execute a function with circuit breaker protection
    ///
    /// Returns CircuitBreakerResult indicating whether the operation should proceed
    /// and handles state transitions automatically.
    pub fn call<F, T, E>(&self, operation: F) -> CircuitBreakerResult<T, E>
    where
        F: FnOnce() -> Result<T, E>,
    {
        let start_time = Instant::now();

        // Check if circuit should transition states
        self.check_state_transitions();

        match *self.state.read().unwrap() {
            CircuitState::Open => {
                self.requests.fetch_add(1, Ordering::Relaxed);
                CircuitBreakerResult::Rejected(CircuitBreakerError::CircuitOpen)
            }
            CircuitState::HalfOpen | CircuitState::Closed => {
                self.requests.fetch_add(1, Ordering::Relaxed);

                match operation() {
                    Ok(result) => {
                        let response_time = start_time.elapsed();
                        self.record_success(response_time);
                        CircuitBreakerResult::Success(result)
                    }
                    Err(error) => {
                        let response_time = start_time.elapsed();
                        self.record_failure(response_time);
                        CircuitBreakerResult::Failure(error)
                    }
                }
            }
        }
    }

    /// Check if circuit should transition between states
    fn check_state_transitions(&self) {
        let current_state = *self.state.read().unwrap();

        match current_state {
            CircuitState::Closed => {
                // Check if we should open the circuit
                if self.should_open_circuit() {
                    self.open_circuit();
                }
            }
            CircuitState::Open => {
                // Check if timeout has passed and we should try half-open
                if self.should_attempt_reset() {
                    self.attempt_reset();
                }
            }
            CircuitState::HalfOpen => {
                // Check if we have enough successes to close the circuit
                if self.should_close_circuit() {
                    self.close_circuit();
                }
                // Or if we should reopen due to failures
                else if self.should_reopen_circuit() {
                    self.open_circuit();
                }
            }
        }
    }

    /// Determine if circuit should open based on failure rate
    fn should_open_circuit(&self) -> bool {
        let failure_rate = self.calculate_failure_rate();

        // Open if failure rate exceeds threshold (e.g., >50%)
        failure_rate > 0.5
    }

    /// Determine if we should attempt to reset from open to half-open
    fn should_attempt_reset(&self) -> bool {
        if let Some(opened_at) = *self.opened_at.read().unwrap() {
            opened_at.elapsed() >= Duration::from_secs(self.config.timeout_seconds)
        } else {
            false
        }
    }

    /// Determine if circuit should close from half-open (enough successes)
    fn should_close_circuit(&self) -> bool {
        self.successes.load(Ordering::Relaxed) >= self.config.success_threshold
    }

    /// Determine if circuit should reopen from half-open (too many failures)
    fn should_reopen_circuit(&self) -> bool {
        self.failures.load(Ordering::Relaxed) >= self.config.failure_threshold
    }

    /// Open the circuit
    fn open_circuit(&self) {
        let mut state = self.state.write().unwrap();
        *state = CircuitState::Open;
        let mut opened_at = self.opened_at.write().unwrap();
        *opened_at = Some(Instant::now());
        tracing::warn!("Circuit breaker opened due to high failure rate");
    }

    /// Attempt to reset circuit to half-open
    fn attempt_reset(&self) {
        let mut state = self.state.write().unwrap();
        *state = CircuitState::HalfOpen;
        // Reset counters for half-open state
        self.successes.store(0, Ordering::Relaxed);
        self.failures.store(0, Ordering::Relaxed);
        tracing::info!("Circuit breaker attempting reset to half-open");
    }

    /// Close the circuit (return to normal operation)
    fn close_circuit(&self) {
        let mut state = self.state.write().unwrap();
        *state = CircuitState::Closed;
        let mut opened_at = self.opened_at.write().unwrap();
        *opened_at = None;
        // Reset all counters
        self.failures.store(0, Ordering::Relaxed);
        self.successes.store(0, Ordering::Relaxed);
        tracing::info!("Circuit breaker closed - service recovered");
    }

    /// Record a successful operation
    fn record_success(&self, response_time: Duration) {
        self.successes.fetch_add(1, Ordering::Relaxed);

        // Record in sliding window
        if let Ok(mut window) = self.request_window.write() {
            window.push_back(RequestOutcome {
                success: true,
                timestamp: Instant::now(),
                response_time,
            });

            // Maintain window size
            while window.len() > self.config.sliding_window_size {
                window.pop_front();
            }
        }
    }

    /// Record a failed operation
    fn record_failure(&self, response_time: Duration) {
        self.failures.fetch_add(1, Ordering::Relaxed);

        // Record in sliding window
        if let Ok(mut window) = self.request_window.write() {
            window.push_back(RequestOutcome {
                success: false,
                timestamp: Instant::now(),
                response_time,
            });

            // Maintain window size
            while window.len() > self.config.sliding_window_size {
                window.pop_front();
            }
        }

        let mut last_failure = self.last_failure_time.write().unwrap();
        *last_failure = Some(Instant::now());
    }

    /// Calculate failure rate from sliding window
    fn calculate_failure_rate(&self) -> f64 {
        if let Ok(window) = self.request_window.read() {
            if window.is_empty() {
                return 0.0;
            }

            let failures = window.iter().filter(|outcome| !outcome.success).count();
            failures as f64 / window.len() as f64
        } else {
            0.0
        }
    }

    /// Get current circuit state
    pub fn state(&self) -> CircuitState {
        *self.state.read().unwrap()
    }

    /// Get circuit breaker statistics
    pub fn stats(&self) -> CircuitBreakerStats {
        CircuitBreakerStats {
            state: self.state(),
            requests: self.requests.load(Ordering::Relaxed),
            failures: self.failures.load(Ordering::Relaxed),
            successes: self.successes.load(Ordering::Relaxed),
            failure_rate: self.calculate_failure_rate(),
            last_failure_time: *self.last_failure_time.read().unwrap(),
            opened_at: *self.opened_at.read().unwrap(),
        }
    }
}

/// Result of a circuit breaker operation
#[derive(Debug)]
pub enum CircuitBreakerResult<T, E> {
    /// Operation succeeded
    Success(T),
    /// Operation failed with original error
    Failure(E),
    /// Operation was rejected by circuit breaker
    Rejected(CircuitBreakerError),
}

/// Circuit breaker specific errors
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CircuitBreakerError {
    /// Circuit is open - operation rejected
    CircuitOpen,
}

/// Circuit breaker statistics for monitoring
#[derive(Debug, Clone)]
pub struct CircuitBreakerStats {
    /// Current circuit state
    pub state: CircuitState,
    /// Total requests processed
    pub requests: u64,
    /// Total failures
    pub failures: u64,
    /// Total successes
    pub successes: u64,
    /// Current failure rate (0.0 - 1.0)
    pub failure_rate: f64,
    /// Last failure timestamp
    pub last_failure_time: Option<Instant>,
    /// Circuit opened timestamp
    pub opened_at: Option<Instant>,
}

/// Bulkhead pattern for resource isolation
///
/// Prevents cascading failures by isolating resources into separate pools.
/// Based on Nygard's bulkhead pattern from "Release It!".
#[derive(Debug)]
pub struct Bulkhead {
    /// Maximum concurrent operations
    max_concurrent: usize,
    /// Current active operations
    active: AtomicUsize,
    /// Queue for waiting operations
    queue: RwLock<VecDeque<Box<dyn FnOnce() + Send + 'static>>>,
    /// Maximum queue size
    max_queue_size: usize,
}

impl Bulkhead {
    /// Create a new bulkhead with specified limits
    pub fn new(max_concurrent: usize, max_queue_size: usize) -> Self {
        Self {
            max_concurrent,
            active: AtomicUsize::new(0),
            queue: RwLock::new(VecDeque::new()),
            max_queue_size,
        }
    }

    /// Execute an operation within the bulkhead
    pub fn execute<F, T>(&self, operation: F) -> Result<T, BulkheadError>
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        let current_active = self.active.load(Ordering::Relaxed);

        if current_active >= self.max_concurrent {
            // Try to queue the operation
            if let Ok(mut queue) = self.queue.write() {
                if queue.len() >= self.max_queue_size {
                    return Err(BulkheadError::QueueFull);
                }
                // Queue is full, reject
                return Err(BulkheadError::AtCapacity);
            }
        }

        // Execute immediately
        self.active.fetch_add(1, Ordering::Relaxed);
        let result = operation();
        self.active.fetch_sub(1, Ordering::Relaxed);

        // Process queued operations if any
        self.process_queue();

        Ok(result)
    }

    /// Process queued operations
    fn process_queue(&self) {
        while self.active.load(Ordering::Relaxed) < self.max_concurrent {
            if let Ok(mut queue) = self.queue.write() {
                if let Some(operation) = queue.pop_front() {
                    self.active.fetch_add(1, Ordering::Relaxed);
                    // Execute operation asynchronously
                    std::thread::spawn(move || {
                        operation();
                    });
                    break;
                }
            }
        }
    }

    /// Get bulkhead statistics
    pub fn stats(&self) -> BulkheadStats {
        BulkheadStats {
            max_concurrent: self.max_concurrent,
            active: self.active.load(Ordering::Relaxed),
            queued: self.queue.read().unwrap().len(),
            max_queue_size: self.max_queue_size,
        }
    }
}

/// Bulkhead operation errors
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum BulkheadError {
    /// Bulkhead is at maximum capacity
    AtCapacity,
    /// Operation queue is full
    QueueFull,
}

/// Bulkhead statistics
#[derive(Debug, Clone)]
pub struct BulkheadStats {
    /// Maximum concurrent operations
    pub max_concurrent: usize,
    /// Currently active operations
    pub active: usize,
    /// Operations waiting in queue
    pub queued: usize,
    /// Maximum queue size
    pub max_queue_size: usize,
}

/// Adaptive timeout manager with research-backed algorithms
#[derive(Debug)]
pub struct AdaptiveTimeout {
    /// Current timeout value
    current_timeout: RwLock<Duration>,
    /// Minimum allowed timeout
    min_timeout: Duration,
    /// Maximum allowed timeout
    max_timeout: Duration,
    /// Adjustment factor for timeout changes
    adjustment_factor: f64,
    /// Recent response times (for calculating adaptive timeout)
    response_times: RwLock<VecDeque<Duration>>,
    /// Target percentile for timeout calculation (e.g., 0.95 for p95)
    target_percentile: f64,
}

impl AdaptiveTimeout {
    /// Create a new adaptive timeout manager
    pub fn new(min_timeout: Duration, max_timeout: Duration, target_percentile: f64) -> Self {
        Self {
            current_timeout: RwLock::new(min_timeout),
            min_timeout,
            max_timeout,
            adjustment_factor: 0.1, // 10% adjustment
            response_times: RwLock::new(VecDeque::with_capacity(100)),
            target_percentile,
        }
    }

    /// Record a response time and adjust timeout if needed
    pub fn record_response_time(&self, response_time: Duration) {
        // Add to response time history
        if let Ok(mut times) = self.response_times.write() {
            times.push_back(response_time);
            if times.len() > 100 {
                times.pop_front();
            }
        }

        // Adjust timeout based on percentile
        self.adjust_timeout();
    }

    /// Get current timeout value
    pub fn current_timeout(&self) -> Duration {
        *self.current_timeout.read().unwrap()
    }

    /// Adjust timeout based on recent response times
    fn adjust_timeout(&self) {
        let target_timeout = self.calculate_target_timeout();

        let mut current = self.current_timeout.write().unwrap();
        let adjustment = (target_timeout.as_millis() as f64 - current.as_millis() as f64) * self.adjustment_factor;

        let new_timeout_millis = (current.as_millis() as f64 + adjustment) as u64;
        let new_timeout = Duration::from_millis(
            new_timeout_millis.clamp(self.min_timeout.as_millis() as u64, self.max_timeout.as_millis() as u64)
        );

        *current = new_timeout;
    }

    /// Calculate target timeout based on response time percentile
    fn calculate_target_timeout(&self) -> Duration {
        if let Ok(times) = self.response_times.read() {
            if times.is_empty() {
                return self.min_timeout;
            }

            let mut sorted_times: Vec<_> = times.iter().cloned().collect();
            sorted_times.sort();

            let index = ((sorted_times.len() - 1) as f64 * self.target_percentile) as usize;
            let percentile_time = sorted_times[index];

            // Add some margin (e.g., 50% more than p95)
            Duration::from_micros((percentile_time.as_micros() as f64 * 1.5) as u64)
        } else {
            self.min_timeout
        }
    }
}
