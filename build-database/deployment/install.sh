#!/bin/bash
# AuroraDB Production Installation Script
# This script installs AuroraDB on Linux systems for production use

set -e

# Configuration
AURORA_VERSION="${AURORA_VERSION:-latest}"
INSTALL_DIR="/usr/local/bin"
CONFIG_DIR="/etc/aurora"
DATA_DIR="/var/lib/aurora"
LOG_DIR="/var/log/aurora"
USER="aurora"
GROUP="aurora"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        log_error "This script must be run as root"
        exit 1
    fi
}

# Detect OS and architecture
detect_platform() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="darwin"
    else
        log_error "Unsupported OS: $OSTYPE"
        exit 1
    fi

    ARCH=$(uname -m)
    case $ARCH in
        x86_64)
            ARCH="x86_64"
            ;;
        aarch64)
            ARCH="aarch64"
            ;;
        *)
            log_error "Unsupported architecture: $ARCH"
            exit 1
            ;;
    esac

    log_info "Detected platform: $OS-$ARCH"
}

# Download AuroraDB binary
download_binary() {
    local binary_name="aurora-db-$AURORA_VERSION-$OS-$ARCH.tar.gz"
    local download_url="https://github.com/aurora-db/aurora/releases/download/$AURORA_VERSION/$binary_name"

    log_info "Downloading AuroraDB $AURORA_VERSION..."

    if ! curl -L -o "/tmp/$binary_name" "$download_url"; then
        log_error "Failed to download AuroraDB binary"
        exit 1
    fi

    log_info "Extracting AuroraDB..."
    tar -xzf "/tmp/$binary_name" -C /tmp

    # Find the binary
    AURORA_BINARY=$(find /tmp -name "aurora-db" -type f 2>/dev/null | head -1)
    if [[ -z "$AURORA_BINARY" ]]; then
        log_error "AuroraDB binary not found in archive"
        exit 1
    fi
}

# Create system user and group
create_user() {
    log_info "Creating aurora user and group..."

    if ! getent group "$GROUP" >/dev/null 2>&1; then
        groupadd -r "$GROUP"
    fi

    if ! getent passwd "$USER" >/dev/null 2>&1; then
        useradd -r -g "$GROUP" -s /bin/false -d "$DATA_DIR" "$USER"
    fi
}

# Install binary and configuration
install_files() {
    log_info "Installing AuroraDB..."

    # Install binary
    install -o root -g root -m 755 "$AURORA_BINARY" "$INSTALL_DIR/aurora-db"

    # Create directories
    mkdir -p "$CONFIG_DIR"
    mkdir -p "$DATA_DIR"
    mkdir -p "$LOG_DIR"

    # Install configuration
    if [[ -f "deployment/config/production.toml" ]]; then
        cp "deployment/config/production.toml" "$CONFIG_DIR/config.toml"
        chmod 644 "$CONFIG_DIR/config.toml"
    else
        log_warn "Production config not found, creating default"
        cat > "$CONFIG_DIR/config.toml" << EOF
[database]
max_connections = 1000
buffer_pool_size = 1073741824
data_directory = "$DATA_DIR"
temp_directory = "/tmp/aurora"

[server]
postgresql_port = 5433
http_port = 8080
binary_port = 9090

[storage]
selection_strategy = "workload_based"

[security]
enable_authentication = true
enable_authorization = true

[logging]
level = "info"
format = "json"
file = "$LOG_DIR/aurora.log"

[monitoring]
enable_prometheus = true
prometheus_port = 9091
EOF
    fi

    # Set permissions
    chown -R "$USER:$GROUP" "$CONFIG_DIR"
    chown -R "$USER:$GROUP" "$DATA_DIR"
    chown -R "$USER:$GROUP" "$LOG_DIR"
}

# Install systemd service
install_service() {
    log_info "Installing systemd service..."

    if [[ -f "deployment/systemd/aurora-db.service" ]]; then
        cp "deployment/systemd/aurora-db.service" "/etc/systemd/system/aurora-db.service"
        systemctl daemon-reload
        systemctl enable aurora-db
    else
        log_warn "Systemd service file not found, skipping service installation"
    fi
}

# Setup firewall rules
setup_firewall() {
    log_info "Setting up firewall rules..."

    if command -v ufw >/dev/null 2>&1; then
        ufw allow 5433/tcp comment "AuroraDB PostgreSQL"
        ufw allow 8080/tcp comment "AuroraDB HTTP API"
        ufw allow 9090/tcp comment "AuroraDB Binary Protocol"
        ufw allow 9091/tcp comment "AuroraDB Prometheus Metrics"
        ufw --force reload
    elif command -v firewall-cmd >/dev/null 2>&1; then
        firewall-cmd --permanent --add-port=5433/tcp --add-port=8080/tcp --add-port=9090/tcp --add-port=9091/tcp
        firewall-cmd --reload
    else
        log_warn "No supported firewall detected (ufw/firewalld). Please manually configure firewall rules for ports 5433, 8080, 9090, 9091."
    fi
}

# Setup log rotation
setup_logrotate() {
    log_info "Setting up log rotation..."

    cat > "/etc/logrotate.d/aurora-db" << EOF
$LOG_DIR/*.log {
    daily
    missingok
    rotate 30
    compress
    delaycompress
    notifempty
    create 644 $USER $GROUP
    postrotate
        systemctl reload aurora-db
    endscript
}
EOF
}

# Start service
start_service() {
    log_info "Starting AuroraDB service..."

    systemctl start aurora-db

    # Wait for service to be healthy
    local retries=30
    local count=0

    while [[ $count -lt $retries ]]; do
        if curl -f -s http://localhost:8080/health >/dev/null 2>&1; then
            log_success "AuroraDB is healthy and running!"
            return 0
        fi

        sleep 2
        ((count++))
    done

    log_error "AuroraDB failed to start or is not healthy"
    log_info "Check logs with: journalctl -u aurora-db -f"
    exit 1
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."

    # Check binary
    if ! command -v aurora-db >/dev/null 2>&1; then
        log_error "aurora-db command not found in PATH"
        exit 1
    fi

    # Check service
    if ! systemctl is-active --quiet aurora-db; then
        log_error "AuroraDB service is not running"
        exit 1
    fi

    # Check health endpoint
    if ! curl -f -s http://localhost:8080/health >/dev/null 2>&1; then
        log_error "AuroraDB health check failed"
        exit 1
    fi

    # Check ports
    for port in 5433 8080 9090 9091; do
        if ! nc -z localhost $port >/dev/null 2>&1; then
            log_error "Port $port is not listening"
            exit 1
        fi
    done

    log_success "AuroraDB installation verified successfully!"
}

# Print usage information
print_usage() {
    cat << EOF
AuroraDB Production Installation Script

USAGE:
    $0 [OPTIONS]

OPTIONS:
    -v, --version VERSION    AuroraDB version to install (default: latest)
    -h, --help              Show this help message

EXAMPLES:
    $0                          # Install latest version
    $0 --version v1.0.0        # Install specific version

POST-INSTALLATION:
    • AuroraDB will be running on ports 5433 (PostgreSQL), 8080 (HTTP), 9090 (Binary), 9091 (Metrics)
    • Configuration: /etc/aurora/config.toml
    • Data: /var/lib/aurora
    • Logs: /var/log/aurora
    • Service: systemctl status aurora-db

CONNECTING:
    PostgreSQL: psql -h localhost -p 5433 -U aurora
    HTTP API: curl http://localhost:8080/health
    Metrics: curl http://localhost:9091/metrics

MONITORING:
    Logs: journalctl -u aurora-db -f
    Status: systemctl status aurora-db
    Restart: systemctl restart aurora-db
EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -v|--version)
                AURORA_VERSION="$2"
                shift 2
                ;;
            -h|--help)
                print_usage
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
}

# Main installation function
main() {
    log_info "Starting AuroraDB production installation..."

    parse_args "$@"
    check_root
    detect_platform
    download_binary
    create_user
    install_files
    install_service
    setup_firewall
    setup_logrotate
    start_service
    verify_installation

    log_success "AuroraDB production installation completed successfully!"
    log_info ""
    log_info "Next steps:"
    log_info "  1. Configure your application to connect to AuroraDB"
    log_info "  2. Set up monitoring and alerting"
    log_info "  3. Configure backup and recovery procedures"
    log_info "  4. Review security settings in /etc/aurora/config.toml"
    log_info ""
    log_info "Documentation: https://docs.aurora-db.com"
    log_info "Community: https://community.aurora-db.com"
}

# Run main function
main "$@"
