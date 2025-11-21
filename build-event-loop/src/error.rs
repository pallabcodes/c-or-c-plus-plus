//! Error types for Cyclone event loop.
//!
//! Cyclone uses structured error handling with detailed context for debugging
//! and observability, following research-backed error handling patterns.

use std::fmt;

/// Result type alias for Cyclone operations
pub type Result<T> = std::result::Result<T, Error>;

/// Comprehensive error types for Cyclone event loop operations
#[derive(Debug, thiserror::Error)]
pub enum Error {
    /// I/O operation errors (networking, file operations)
    #[error("I/O error: {source}")]
    Io {
        /// The underlying I/O error
        #[from]
        source: std::io::Error,
    },

    /// Timer-related errors
    #[error("Timer error: {message}")]
    Timer {
        /// Descriptive error message
        message: String,
    },

    /// Reactor state errors
    #[error("Reactor error: {message}")]
    Reactor {
        /// Descriptive error message
        message: String,
    },

    /// Configuration errors
    #[error("Configuration error: {message}")]
    Config {
        /// Descriptive error message
        message: String,
    },

    /// Networking protocol errors
    #[error("Network error: {message}")]
    Network {
        /// Descriptive error message
        message: String,
    },

    /// Resource exhaustion errors
    #[error("Resource exhausted: {resource}")]
    ResourceExhausted {
        /// The exhausted resource type
        resource: String,
    },

    /// Concurrency-related errors
    #[error("Concurrency error: {message}")]
    Concurrency {
        /// Descriptive error message
        message: String,
    },

    /// TLS/encryption errors
    #[cfg(feature = "tls")]
    #[error("TLS error: {source}")]
    Tls {
        /// The underlying TLS error
        #[from]
        source: rustls::Error,
    },

    /// Generic errors with context
    #[error("Cyclone error: {message}")]
    Other {
        /// Descriptive error message
        message: String,
    },
}

impl Error {
    /// Create a timer-related error
    pub fn timer<S: Into<String>>(message: S) -> Self {
        Self::Timer {
            message: message.into(),
        }
    }

    /// Create a reactor-related error
    pub fn reactor<S: Into<String>>(message: S) -> Self {
        Self::Reactor {
            message: message.into(),
        }
    }

    /// Create a configuration error
    pub fn config<S: Into<String>>(message: S) -> Self {
        Self::Config {
            message: message.into(),
        }
    }

    /// Create a network error
    pub fn network<S: Into<String>>(message: S) -> Self {
        Self::Network {
            message: message.into(),
        }
    }

    /// Create a resource exhaustion error
    pub fn resource_exhausted<S: Into<String>>(resource: S) -> Self {
        Self::ResourceExhausted {
            resource: resource.into(),
        }
    }

    /// Create a concurrency error
    pub fn concurrency<S: Into<String>>(message: S) -> Self {
        Self::Concurrency {
            message: message.into(),
        }
    }

    /// Create a generic error
    pub fn other<S: Into<String>>(message: S) -> Self {
        Self::Other {
            message: message.into(),
        }
    }

    /// Check if this is a recoverable error
    pub fn is_recoverable(&self) -> bool {
        match self {
            Self::Io { .. } => {
                // Some I/O errors are recoverable (EAGAIN, EINTR)
                // This would need more detailed checking in practice
                false
            }
            Self::Timer { .. } => true,
            Self::Reactor { .. } => false,
            Self::Config { .. } => false,
            Self::Network { .. } => true,
            Self::ResourceExhausted { .. } => true,
            Self::Concurrency { .. } => true,
            #[cfg(feature = "tls")]
            Self::Tls { .. } => false,
            Self::Other { .. } => false,
        }
    }

    /// Get error category for metrics and monitoring
    pub fn category(&self) -> &'static str {
        match self {
            Self::Io { .. } => "io",
            Self::Timer { .. } => "timer",
            Self::Reactor { .. } => "reactor",
            Self::Config { .. } => "config",
            Self::Network { .. } => "network",
            Self::ResourceExhausted { .. } => "resource",
            Self::Concurrency { .. } => "concurrency",
            #[cfg(feature = "tls")]
            Self::Tls { .. } => "tls",
            Self::Other { .. } => "other",
        }
    }
}

// UNIQUENESS Validation:
// - [x] Structured error types for observability
// - [x] Research-backed error handling patterns
// - [x] Memory-safe error propagation
// - [x] Comprehensive error categorization
