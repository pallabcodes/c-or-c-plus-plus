# Cyclone Event Loop - Production Docker Image
# Research-backed, enterprise-grade containerization

FROM rust:1.75-slim AS builder

# Install system dependencies for Cyclone
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /usr/src/cyclone

# Copy Cargo.toml and Cargo.lock first for better caching
COPY Cargo.toml Cargo.lock ./

# Copy source code
COPY src ./src

# Build Cyclone with full optimizations
RUN cargo build --release --features full-optimization

# Production stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user for security
RUN groupadd -r cyclone && useradd -r -g cyclone cyclone

# Create necessary directories
RUN mkdir -p /etc/cyclone /var/log/cyclone /var/lib/cyclone && \
    chown -R cyclone:cyclone /var/log/cyclone /var/lib/cyclone

# Copy the built binary
COPY --from=builder /usr/src/cyclone/target/release/cyclone /usr/local/bin/cyclone

# Copy default configuration
COPY config/production.toml /etc/cyclone/config.toml

# Switch to non-root user
USER cyclone

# Expose ports
EXPOSE 8080 9090

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Set environment variables
ENV RUST_LOG=info \
    CYCLONE_CONFIG=/etc/cyclone/config.toml

# Run Cyclone
CMD ["cyclone", "--config", "/etc/cyclone/config.toml"]
