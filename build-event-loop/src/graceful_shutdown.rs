//! Graceful Shutdown with Connection Draining
//!
//! Enterprise-grade shutdown management with research-backed draining strategies.
//! Based on Kubernetes graceful shutdown patterns and cloud-native best practices.
//!
//! ## Research Integration
//!
//! - **Graceful Shutdown Pattern**: Cloud-native application lifecycle management
//! - **Connection Draining**: Zero-downtime deployments (Kubernetes preStop hooks)
//! - **Signal Handling**: Unix signal processing for clean shutdowns
//! - **Resource Cleanup**: Deterministic resource deallocation

use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use std::sync::{Arc, RwLock};
use std::time::{Duration, Instant};
use tokio::sync::broadcast;

/// Graceful shutdown coordinator for enterprise applications
///
/// Manages the complete shutdown lifecycle with connection draining,
/// resource cleanup, and timeout enforcement.
#[derive(Debug)]
pub struct GracefulShutdown {
    /// Shutdown signal sender
    shutdown_tx: broadcast::Sender<()>,
    /// Shutdown signal receiver
    shutdown_rx: broadcast::Receiver<()>,
    /// Shutdown initiated flag
    shutdown_initiated: AtomicBool,
    /// Shutdown start time
    shutdown_start: RwLock<Option<Instant>>,
    /// Maximum shutdown timeout
    max_shutdown_timeout: Duration,
    /// Registered shutdown handlers
    handlers: RwLock<Vec<ShutdownHandler>>,
    /// Active connections counter
    active_connections: AtomicUsize,
    /// Shutdown statistics
    stats: ShutdownStats,
}

#[derive(Debug, Clone, Default)]
pub struct ShutdownStats {
    /// Total shutdown time
    pub shutdown_duration: Option<Duration>,
    /// Connections drained during shutdown
    pub connections_drained: usize,
    /// Handlers executed
    pub handlers_executed: usize,
    /// Handlers that timed out
    pub handlers_timed_out: usize,
    /// Whether shutdown completed successfully
    pub completed_successfully: bool,
}

/// Shutdown handler for resource cleanup
#[derive(Debug)]
pub struct ShutdownHandler {
    /// Handler name for logging
    name: String,
    /// Handler priority (higher = executed first)
    priority: i32,
    /// Handler function
    handler: Box<dyn FnOnce() -> Result<(), ShutdownError> + Send + 'static>,
    /// Timeout for this handler
    timeout: Duration,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum ShutdownError {
    /// Handler timed out
    Timeout(String),
    /// Handler failed with error
    HandlerError(String),
    /// Resource cleanup failed
    ResourceCleanupError(String),
}

impl GracefulShutdown {
    /// Create a new graceful shutdown coordinator
    pub fn new(max_shutdown_timeout: Duration) -> Self {
        let (shutdown_tx, shutdown_rx) = broadcast::channel(1);

        Self {
            shutdown_tx,
            shutdown_rx,
            shutdown_initiated: AtomicBool::new(false),
            shutdown_start: RwLock::new(None),
            max_shutdown_timeout,
            handlers: RwLock::new(Vec::new()),
            active_connections: AtomicUsize::new(0),
            stats: ShutdownStats::default(),
        }
    }

    /// Register a shutdown handler
    ///
    /// Handlers are executed in priority order (highest first) during shutdown.
    /// Higher priority handlers are executed before lower priority ones.
    pub fn register_handler<F>(
        &self,
        name: impl Into<String>,
        priority: i32,
        timeout: Duration,
        handler: F,
    ) where
        F: FnOnce() -> Result<(), ShutdownError> + Send + 'static,
    {
        let handler = ShutdownHandler {
            name: name.into(),
            priority,
            handler: Box::new(handler),
            timeout,
        };

        if let Ok(mut handlers) = self.handlers.write() {
            handlers.push(handler);
            // Sort by priority (highest first)
            handlers.sort_by(|a, b| b.priority.cmp(&a.priority));
        }
    }

    /// Increment active connection count
    pub fn increment_connections(&self) {
        self.active_connections.fetch_add(1, Ordering::Relaxed);
    }

    /// Decrement active connection count
    pub fn decrement_connections(&self) {
        self.active_connections.fetch_sub(1, Ordering::Relaxed);
    }

    /// Get current active connection count
    pub fn active_connections(&self) -> usize {
        self.active_connections.load(Ordering::Relaxed)
    }

    /// Initiate graceful shutdown
    ///
    /// This will:
    /// 1. Stop accepting new connections
    /// 2. Wait for existing connections to drain
    /// 3. Execute shutdown handlers in priority order
    /// 4. Force shutdown after timeout
    pub async fn initiate_shutdown(&self) -> Result<(), ShutdownError> {
        if self.shutdown_initiated.swap(true, Ordering::Relaxed) {
            return Ok(()); // Already shutting down
        }

        let shutdown_start = Instant::now();
        *self.shutdown_start.write().unwrap() = Some(shutdown_start);

        tracing::info!("Initiating graceful shutdown with {} active connections",
                      self.active_connections());

        // Phase 1: Stop accepting new connections
        self.stop_accepting_connections().await?;

        // Phase 2: Wait for connection draining
        self.wait_for_connection_draining().await?;

        // Phase 3: Execute shutdown handlers
        self.execute_shutdown_handlers().await?;

        // Phase 4: Final cleanup
        self.final_cleanup().await?;

        let shutdown_duration = shutdown_start.elapsed();
        self.stats.shutdown_duration = Some(shutdown_duration);
        self.stats.completed_successfully = true;

        tracing::info!("Graceful shutdown completed in {:?}", shutdown_duration);
        Ok(())
    }

    /// Wait for shutdown signal
    pub async fn wait_for_shutdown(&self) {
        let _ = self.shutdown_rx.recv().await;
    }

    /// Get shutdown receiver for components to listen for shutdown signal
    pub fn shutdown_signal(&self) -> broadcast::Receiver<()> {
        self.shutdown_tx.subscribe()
    }

    /// Check if shutdown has been initiated
    pub fn is_shutdown_initiated(&self) -> bool {
        self.shutdown_initiated.load(Ordering::Relaxed)
    }

    /// Get shutdown statistics
    pub fn stats(&self) -> &ShutdownStats {
        &self.stats
    }

    /// Phase 1: Stop accepting new connections
    async fn stop_accepting_connections(&self) -> Result<(), ShutdownError> {
        tracing::info!("Phase 1: Stopping acceptance of new connections");

        // Send shutdown signal to all listeners
        let _ = self.shutdown_tx.send(());

        // Give listeners a moment to react
        tokio::time::sleep(Duration::from_millis(100)).await;

        Ok(())
    }

    /// Phase 2: Wait for existing connections to drain
    async fn wait_for_connection_draining(&self) -> Result<(), ShutdownError> {
        tracing::info!("Phase 2: Waiting for connection draining");

        let drain_start = Instant::now();
        let max_drain_time = Duration::from_secs(30); // Configurable

        loop {
            let active = self.active_connections();
            if active == 0 {
                break;
            }

            if drain_start.elapsed() > max_drain_time {
                tracing::warn!("Connection draining timed out with {} connections still active", active);
                break;
            }

            tracing::info!("Waiting for {} connections to drain...", active);
            tokio::time::sleep(Duration::from_millis(500)).await;
        }

        self.stats.connections_drained = self.active_connections();
        Ok(())
    }

    /// Phase 3: Execute shutdown handlers in priority order
    async fn execute_shutdown_handlers(&self) -> Result<(), ShutdownError> {
        tracing::info!("Phase 3: Executing shutdown handlers");

        let handlers = {
            let mut handlers = self.handlers.write().unwrap();
            std::mem::take(&mut handlers)
        };

        for handler in handlers {
            tracing::info!("Executing shutdown handler: {}", handler.name);

            let handler_future = tokio::task::spawn_blocking(move || {
                (handler.handler)()
            });

            match tokio::time::timeout(handler.timeout, handler_future).await {
                Ok(Ok(Ok(()))) => {
                    self.stats.handlers_executed += 1;
                    tracing::info!("Shutdown handler '{}' completed successfully", handler.name);
                }
                Ok(Ok(Err(e))) => {
                    self.stats.handlers_timed_out += 1;
                    tracing::error!("Shutdown handler '{}' failed: {:?}", handler.name, e);
                    return Err(e);
                }
                Ok(Err(e)) => {
                    self.stats.handlers_timed_out += 1;
                    tracing::error!("Shutdown handler '{}' panicked: {:?}", handler.name, e);
                    return Err(ShutdownError::HandlerError(
                        format!("Handler '{}' panicked: {:?}", handler.name, e)
                    ));
                }
                Err(_) => {
                    self.stats.handlers_timed_out += 1;
                    tracing::error!("Shutdown handler '{}' timed out after {:?}", handler.name, handler.timeout);
                    return Err(ShutdownError::Timeout(handler.name));
                }
            }
        }

        Ok(())
    }

    /// Phase 4: Final cleanup and forced shutdown if needed
    async fn final_cleanup(&self) -> Result<(), ShutdownError> {
        tracing::info!("Phase 4: Final cleanup");

        // Check if we've exceeded the maximum shutdown timeout
        if let Some(start) = *self.shutdown_start.read().unwrap() {
            if start.elapsed() > self.max_shutdown_timeout {
                tracing::error!("Maximum shutdown timeout exceeded, forcing shutdown");
                return Err(ShutdownError::Timeout("Maximum shutdown timeout".to_string()));
            }
        }

        // Final resource cleanup would go here
        // - Flush any remaining buffers
        // - Close any remaining connections forcefully
        // - Clean up temporary files
        // - Release system resources

        Ok(())
    }
}

/// Connection draining manager for graceful shutdown
///
/// Manages the lifecycle of connections during shutdown to ensure
/// zero-downtime deployments.
#[derive(Debug)]
pub struct ConnectionDrainer {
    /// Graceful shutdown coordinator
    shutdown: Arc<GracefulShutdown>,
    /// Maximum time to wait for individual connections to close
    connection_timeout: Duration,
    /// Connection close statistics
    stats: ConnectionDrainStats,
}

#[derive(Debug, Clone, Default)]
pub struct ConnectionDrainStats {
    /// Total connections that were drained
    pub connections_drained: usize,
    /// Connections that had to be force-closed
    pub connections_force_closed: usize,
    /// Total drain time
    pub drain_duration: Option<Duration>,
}

impl ConnectionDrainer {
    /// Create a new connection drainer
    pub fn new(shutdown: Arc<GracefulShutdown>, connection_timeout: Duration) -> Self {
        Self {
            shutdown,
            connection_timeout,
            stats: ConnectionDrainStats::default(),
        }
    }

    /// Drain a single connection gracefully
    ///
    /// Waits for the connection to close naturally, then forces closure if needed.
    pub async fn drain_connection(&mut self, connection_id: &str) -> Result<(), ShutdownError> {
        tracing::debug!("Draining connection: {}", connection_id);

        let drain_start = Instant::now();

        // Wait for connection to close naturally
        let timeout_result = tokio::time::timeout(
            self.connection_timeout,
            self.wait_for_connection_close(connection_id)
        ).await;

        match timeout_result {
            Ok(()) => {
                // Connection closed naturally
                self.stats.connections_drained += 1;
                tracing::debug!("Connection {} drained successfully", connection_id);
            }
            Err(_) => {
                // Timeout - force close connection
                self.force_close_connection(connection_id).await?;
                self.stats.connections_force_closed += 1;
                tracing::warn!("Connection {} force-closed after timeout", connection_id);
            }
        }

        Ok(())
    }

    /// Wait for a connection to close naturally
    async fn wait_for_connection_close(&self, _connection_id: &str) {
        // In a real implementation, this would:
        // - Monitor connection state
        // - Wait for pending operations to complete
        // - Allow in-flight requests to finish
        // For now, simulate waiting
        tokio::time::sleep(Duration::from_millis(100)).await;
    }

    /// Force close a connection
    async fn force_close_connection(&self, connection_id: &str) -> Result<(), ShutdownError> {
        // In a real implementation, this would:
        // - Send connection close signal
        // - Cancel pending operations
        // - Clean up connection resources
        tracing::warn!("Force closing connection: {}", connection_id);
        Ok(())
    }

    /// Get connection draining statistics
    pub fn stats(&self) -> &ConnectionDrainStats {
        &self.stats
    }
}

/// Signal handler for graceful shutdown
///
/// Handles Unix signals (SIGTERM, SIGINT) and initiates graceful shutdown.
#[derive(Debug)]
pub struct SignalHandler {
    /// Graceful shutdown coordinator
    shutdown: Arc<GracefulShutdown>,
}

impl SignalHandler {
    /// Create a new signal handler
    pub fn new(shutdown: Arc<GracefulShutdown>) -> Self {
        Self { shutdown }
    }

    /// Start signal handling (spawns background task)
    pub fn start(&self) {
        let shutdown = Arc::clone(&self.shutdown);

        tokio::spawn(async move {
            Self::handle_signals(shutdown).await;
        });
    }

    /// Handle Unix signals for graceful shutdown
    async fn handle_signals(shutdown: Arc<GracefulShutdown>) {
        use tokio::signal::unix::{signal, SignalKind};

        let mut sigterm = signal(SignalKind::terminate()).unwrap();
        let mut sigint = signal(SignalKind::interrupt()).unwrap();

        tokio::select! {
            _ = sigterm.recv() => {
                tracing::info!("Received SIGTERM, initiating graceful shutdown");
                if let Err(e) = shutdown.initiate_shutdown().await {
                    tracing::error!("Shutdown failed: {:?}", e);
                    std::process::exit(1);
                }
            }
            _ = sigint.recv() => {
                tracing::info!("Received SIGINT, initiating graceful shutdown");
                if let Err(e) = shutdown.initiate_shutdown().await {
                    tracing::error!("Shutdown failed: {:?}", e);
                    std::process::exit(1);
                }
            }
        }
    }
}

/// Health check manager for shutdown coordination
///
/// Provides health endpoints that change during shutdown to coordinate
/// with load balancers and orchestrators.
#[derive(Debug)]
pub struct HealthCheckManager {
    /// Graceful shutdown coordinator
    shutdown: Arc<GracefulShutdown>,
    /// Current health status
    health_status: RwLock<HealthStatus>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum HealthStatus {
    /// Service is healthy and accepting traffic
    Healthy,
    /// Service is shutting down but still processing requests
    Draining,
    /// Service is unhealthy and should not receive traffic
    Unhealthy,
}

impl HealthCheckManager {
    /// Create a new health check manager
    pub fn new(shutdown: Arc<GracefulShutdown>) -> Self {
        Self {
            shutdown,
            health_status: RwLock::new(HealthStatus::Healthy),
        }
    }

    /// Get current health status
    pub fn health_status(&self) -> HealthStatus {
        let mut status = *self.health_status.read().unwrap();

        // Update status based on shutdown state
        if self.shutdown.is_shutdown_initiated() {
            if self.shutdown.active_connections() > 0 {
                status = HealthStatus::Draining;
            } else {
                status = HealthStatus::Unhealthy;
            }
            *self.health_status.write().unwrap() = status;
        }

        status
    }

    /// Check if service is ready to accept traffic
    pub fn is_ready(&self) -> bool {
        self.health_status() == HealthStatus::Healthy
    }

    /// Check if service is live (but may be draining)
    pub fn is_live(&self) -> bool {
        let status = self.health_status();
        status == HealthStatus::Healthy || status == HealthStatus::Draining
    }
}
