# AuroraDB Production Docker Image
# Multi-stage build for optimal image size and security

# Build stage
FROM rust:1.75-slim as builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy dependency manifests
COPY Cargo.toml Cargo.lock ./

# Create dummy src to cache dependencies
RUN mkdir src && echo "fn main() {}" > src/main.rs
RUN cargo build --release --target x86_64-unknown-linux-gnu
RUN rm -rf src

# Copy source code
COPY src ./src

# Build the application
RUN touch src/main.rs && cargo build --release --target x86_64-unknown-linux-gnu

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    curl \
    jq \
    && rm -rf /var/lib/apt/lists/*

# Create aurora user for security
RUN groupadd -r aurora && useradd -r -g aurora aurora

# Create necessary directories with proper permissions
RUN mkdir -p /var/lib/aurora /var/log/aurora /etc/aurora /app \
    && chown -R aurora:aurora /var/lib/aurora /var/log/aurora /etc/aurora /app

# Copy the binary
COPY --from=builder /app/target/x86_64-unknown-linux-gnu/release/aurora-db /app/aurora-db

# Copy default configuration
COPY deployment/config/production.toml /etc/aurora/config.toml

# Set proper permissions
RUN chmod +x /app/aurora-db && chown aurora:aurora /app/aurora-db

# Switch to non-root user
USER aurora

# Set working directory
WORKDIR /app

# Expose ports
EXPOSE 5433 8080 9090 9091

# Environment variables
ENV AURORA_CONFIG=/etc/aurora/config.toml
ENV AURORA_DATA_DIR=/var/lib/aurora
ENV AURORA_LOG_DIR=/var/log/aurora
ENV RUST_LOG=info
ENV RUST_BACKTRACE=1

# Health check with proper endpoint
HEALTHCHECK --interval=30s --timeout=10s --start-period=60s --retries=3 \
    CMD curl -f -s http://localhost:8080/health > /dev/null || exit 1

# Volume mounts for persistence
VOLUME ["/var/lib/aurora", "/var/log/aurora", "/etc/aurora"]

# Default command with graceful shutdown handling
CMD ["/app/aurora-db"]
