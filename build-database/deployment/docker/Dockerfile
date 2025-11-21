# AuroraDB Production Dockerfile
# Multi-stage build for optimal image size and security

# =============================================================================
# Stage 1: Builder - Compile AuroraDB with full toolchain
# =============================================================================
FROM rust:1.75-slim AS builder

# Install system dependencies for compilation
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libssl-dev \
    clang \
    llvm-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /aurora

# Copy dependency manifests for better caching
COPY Cargo.toml Cargo.lock ./

# Create dummy main.rs for dependency compilation
RUN mkdir src && echo "fn main() {}" > src/main.rs

# Build dependencies (cached layer)
RUN cargo build --release --target x86_64-unknown-linux-gnu

# Remove dummy source and copy real source
RUN rm -rf src
COPY src ./src

# Build AuroraDB with optimizations
RUN cargo build --release --target x86_64-unknown-linux-gnu

# Strip binary for smaller size
RUN strip target/x86_64-unknown-linux-gnu/release/aurora-db

# =============================================================================
# Stage 2: Runtime - Minimal production image
# =============================================================================
FROM debian:bookworm-slim AS runtime

# Install runtime dependencies only
RUN apt-get update && apt-get install -y \
    ca-certificates \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Create aurora user for security
RUN groupadd -r aurora && useradd -r -g aurora aurora

# Create necessary directories
RUN mkdir -p /aurora/data /aurora/logs /aurora/config && \
    chown -R aurora:aurora /aurora

# Copy binary from builder stage
COPY --from=builder /aurora/target/x86_64-unknown-linux-gnu/release/aurora-db /usr/local/bin/aurora-db

# Copy default configuration
COPY --from=builder /aurora/config/ /aurora/config/

# Set working directory
WORKDIR /aurora

# Switch to non-root user
USER aurora

# Expose ports
EXPOSE 5432 8080 9090

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:9090/health || exit 1

# Default command
CMD ["aurora-db", "--config", "/aurora/config/production.toml", "--data-dir", "/aurora/data"]

# =============================================================================
# Stage 3: Debug - Development and debugging image
# =============================================================================
FROM runtime AS debug

# Switch back to root for debug tools
USER root

# Install debug tools
RUN apt-get update && apt-get install -y \
    gdb \
    valgrind \
    strace \
    tcpdump \
    curl \
    jq \
    vim \
    && rm -rf /var/lib/apt/lists/*

# Copy debug symbols
COPY --from=builder /aurora/target/x86_64-unknown-linux-gnu/release/aurora-db /usr/local/bin/aurora-db-debug

# Switch back to aurora user
USER aurora

# Debug command
CMD ["gdb", "/usr/local/bin/aurora-db-debug"]
