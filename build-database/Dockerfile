# AuroraDB Docker Image
# Production-ready container for AuroraDB with REST API

FROM rust:1.75-slim AS builder

# Install system dependencies
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy Cargo files
COPY Cargo.toml Cargo.lock ./

# Copy source code
COPY src ./src

# Build the application in release mode
RUN cargo build --release

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
RUN mkdir -p /app/data
WORKDIR /app

# Copy the binary from builder stage
COPY --from=builder /app/target/release/auroradb-server /app/auroradb-server

# Copy configuration files
COPY config/ /app/config/

# Create non-root user
RUN useradd -r -s /bin/false auroradb
RUN chown -R auroradb:auroradb /app
USER auroradb

# Expose ports
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Default command
CMD ["./auroradb-server", "--config", "/app/config/server.toml"]
