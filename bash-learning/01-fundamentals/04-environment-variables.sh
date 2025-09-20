#!/bin/bash
#
# Bash Fundamentals: Environment Variables and Configuration
# Production-Grade Script for Fintech Applications
#
# This script demonstrates environment variable handling, configuration
# management, and system integration for financial applications.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#

# =============================================================================
# SCRIPT CONFIGURATION AND SAFETY SETTINGS
# =============================================================================

set -euo pipefail
IFS=$'\n\t'

# =============================================================================
# LOGGING CONFIGURATION
# =============================================================================

readonly LOG_LEVEL_ERROR=1
readonly LOG_LEVEL_WARN=2
readonly LOG_LEVEL_INFO=3
readonly LOG_LEVEL_DEBUG=4
readonly CURRENT_LOG_LEVEL="${LOG_LEVEL:-$LOG_LEVEL_INFO}"

log_message() {
    local level="$1"
    local message="$2"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S.%3N')
    
    if [[ "$level" -le "$CURRENT_LOG_LEVEL" ]]; then
        case "$level" in
            "$LOG_LEVEL_ERROR") echo "[ERROR] $timestamp: $message" >&2 ;;
            "$LOG_LEVEL_WARN")  echo "[WARN]  $timestamp: $message" >&2 ;;
            "$LOG_LEVEL_INFO")  echo "[INFO]  $timestamp: $message" >&2 ;;
            "$LOG_LEVEL_DEBUG") echo "[DEBUG] $timestamp: $message" >&2 ;;
        esac
    fi
}

# =============================================================================
# ENVIRONMENT VARIABLE MANAGEMENT
# =============================================================================

# Load environment configuration
load_environment_config() {
    local env_file="${1:-/etc/fintech/environment.conf}"
    
    log_message $LOG_LEVEL_INFO "Loading environment configuration from: $env_file"
    
    if [[ -f "$env_file" ]]; then
        # Source the environment file
        set -a  # Automatically export all variables
        source "$env_file"
        set +a  # Turn off automatic export
        
        log_message $LOG_LEVEL_INFO "Environment configuration loaded successfully"
    else
        log_message $LOG_LEVEL_WARN "Environment file not found: $env_file"
        log_message $LOG_LEVEL_INFO "Using default environment configuration"
    fi
}

# Set default environment variables
set_default_environment() {
    # Application configuration
    export FINANCIAL_APP_NAME="${FINANCIAL_APP_NAME:-FinancialProcessor}"
    export FINANCIAL_APP_VERSION="${FINANCIAL_APP_VERSION:-1.0.0}"
    export FINANCIAL_APP_ENVIRONMENT="${FINANCIAL_APP_ENVIRONMENT:-development}"
    
    # Logging configuration
    export LOG_LEVEL="${LOG_LEVEL:-INFO}"
    export LOG_FILE="${LOG_FILE:-/var/log/fintech/application.log}"
    export AUDIT_LOG_FILE="${AUDIT_LOG_FILE:-/var/log/fintech/audit.log}"
    
    # Database configuration
    export DB_HOST="${DB_HOST:-localhost}"
    export DB_PORT="${DB_PORT:-5432}"
    export DB_NAME="${DB_NAME:-fintech}"
    export DB_USER="${DB_USER:-fintech_user}"
    export DB_PASSWORD="${DB_PASSWORD:-}"
    
    # API configuration
    export API_BASE_URL="${API_BASE_URL:-https://api.financial-data.com}"
    export API_TIMEOUT="${API_TIMEOUT:-30}"
    export API_RETRY_COUNT="${API_RETRY_COUNT:-3}"
    
    # Security configuration
    export ENCRYPTION_KEY="${ENCRYPTION_KEY:-}"
    export JWT_SECRET="${JWT_SECRET:-}"
    export SESSION_TIMEOUT="${SESSION_TIMEOUT:-3600}"
    
    # File paths
    export CONFIG_DIR="${CONFIG_DIR:-/etc/fintech}"
    export DATA_DIR="${DATA_DIR:-/var/data/fintech}"
    export TEMP_DIR="${TEMP_DIR:-/tmp/fintech}"
    export BACKUP_DIR="${BACKUP_DIR:-/var/backups/fintech}"
    
    # Performance configuration
    export MAX_WORKERS="${MAX_WORKERS:-4}"
    export BATCH_SIZE="${BATCH_SIZE:-1000}"
    export CACHE_TTL="${CACHE_TTL:-300}"
    
    log_message $LOG_LEVEL_DEBUG "Default environment variables set"
}

# Validate required environment variables
validate_environment() {
    local required_vars=(
        "FINANCIAL_APP_NAME"
        "FINANCIAL_APP_VERSION"
        "FINANCIAL_APP_ENVIRONMENT"
        "LOG_LEVEL"
        "DB_HOST"
        "DB_PORT"
        "DB_NAME"
        "DB_USER"
    )
    
    local missing_vars=()
    
    for var in "${required_vars[@]}"; do
        if [[ -z "${!var:-}" ]]; then
            missing_vars+=("$var")
        fi
    done
    
    if [[ ${#missing_vars[@]} -gt 0 ]]; then
        log_message $LOG_LEVEL_ERROR "Missing required environment variables: ${missing_vars[*]}"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Environment validation passed"
    return 0
}

# =============================================================================
# CONFIGURATION MANAGEMENT
# =============================================================================

# Load configuration from file
load_configuration() {
    local config_file="$1"
    local config_type="${2:-ini}"
    
    log_message $LOG_LEVEL_INFO "Loading configuration from: $config_file (type: $config_type)"
    
    if [[ ! -f "$config_file" ]]; then
        log_message $LOG_LEVEL_ERROR "Configuration file not found: $config_file"
        return 1
    fi
    
    case "$config_type" in
        "ini")
            load_ini_config "$config_file"
            ;;
        "json")
            load_json_config "$config_file"
            ;;
        "yaml")
            load_yaml_config "$config_file"
            ;;
        "env")
            load_env_config "$config_file"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unsupported configuration type: $config_type"
            return 1
            ;;
    esac
}

# Load INI configuration
load_ini_config() {
    local config_file="$1"
    
    log_message $LOG_LEVEL_DEBUG "Loading INI configuration from: $config_file"
    
    while IFS= read -r line; do
        # Skip empty lines and comments
        [[ -z "$line" || "$line" =~ ^[[:space:]]*# ]] && continue
        
        # Skip section headers
        [[ "$line" =~ ^\[.*\]$ ]] && continue
        
        # Parse key=value pairs
        if [[ "$line" =~ ^([^=]+)=(.*)$ ]]; then
            local key="${BASH_REMATCH[1]}"
            local value="${BASH_REMATCH[2]}"
            
            # Remove leading/trailing whitespace
            key="${key%"${key##*[![:space:]]}"}"
            key="${key#"${key%%[![:space:]]*}"}"
            value="${value%"${value##*[![:space:]]}"}"
            value="${value#"${value%%[![:space:]]*}"}"
            
            # Remove quotes if present
            value="${value%\"}"
            value="${value#\"}"
            
            # Export the variable
            export "$key"="$value"
            log_message $LOG_LEVEL_DEBUG "Loaded config: $key=$value"
        fi
    done < "$config_file"
}

# Load JSON configuration
load_json_config() {
    local config_file="$1"
    
    log_message $LOG_LEVEL_DEBUG "Loading JSON configuration from: $config_file"
    
    # Check if jq is available
    if ! command -v jq >/dev/null 2>&1; then
        log_message $LOG_LEVEL_ERROR "jq is required for JSON configuration but not installed"
        return 1
    fi
    
    # Extract key-value pairs from JSON and export them
    jq -r 'to_entries[] | "\(.key)=\(.value)"' "$config_file" | while IFS='=' read -r key value; do
        export "$key"="$value"
        log_message $LOG_LEVEL_DEBUG "Loaded config: $key=$value"
    done
}

# Load YAML configuration
load_yaml_config() {
    local config_file="$1"
    
    log_message $LOG_LEVEL_DEBUG "Loading YAML configuration from: $config_file"
    
    # Check if yq is available
    if ! command -v yq >/dev/null 2>&1; then
        log_message $LOG_LEVEL_ERROR "yq is required for YAML configuration but not installed"
        return 1
    fi
    
    # Extract key-value pairs from YAML and export them
    yq eval 'to_entries[] | "\(.key)=\(.value)"' "$config_file" | while IFS='=' read -r key value; do
        export "$key"="$value"
        log_message $LOG_LEVEL_DEBUG "Loaded config: $key=$value"
    done
}

# Load ENV configuration
load_env_config() {
    local config_file="$1"
    
    log_message $LOG_LEVEL_DEBUG "Loading ENV configuration from: $config_file"
    
    # Source the environment file
    set -a
    source "$config_file"
    set +a
    
    log_message $LOG_LEVEL_DEBUG "ENV configuration loaded"
}

# =============================================================================
# SECURE ENVIRONMENT HANDLING
# =============================================================================

# Mask sensitive environment variables in logs
mask_sensitive_vars() {
    local -a sensitive_vars=(
        "DB_PASSWORD"
        "API_KEY"
        "ENCRYPTION_KEY"
        "JWT_SECRET"
        "SECRET_KEY"
        "PASSWORD"
        "TOKEN"
    )
    
    for var in "${sensitive_vars[@]}"; do
        if [[ -n "${!var:-}" ]]; then
            local value="${!var}"
            local masked_value="${value:0:4}****${value: -4}"
            log_message $LOG_LEVEL_DEBUG "Environment variable $var: $masked_value"
        fi
    done
}

# Encrypt sensitive environment variables
encrypt_sensitive_vars() {
    local encryption_key="${ENCRYPTION_KEY:-}"
    
    if [[ -z "$encryption_key" ]]; then
        log_message $LOG_LEVEL_ERROR "Encryption key not set"
        return 1
    fi
    
    local -a sensitive_vars=(
        "DB_PASSWORD"
        "API_KEY"
        "JWT_SECRET"
    )
    
    for var in "${sensitive_vars[@]}"; do
        if [[ -n "${!var:-}" ]]; then
            local encrypted_value=$(echo "${!var}" | openssl enc -aes-256-cbc -a -k "$encryption_key")
            export "${var}_ENCRYPTED"="$encrypted_value"
            unset "$var"
            log_message $LOG_LEVEL_DEBUG "Encrypted environment variable: $var"
        fi
    done
}

# Decrypt sensitive environment variables
decrypt_sensitive_vars() {
    local encryption_key="${ENCRYPTION_KEY:-}"
    
    if [[ -z "$encryption_key" ]]; then
        log_message $LOG_LEVEL_ERROR "Encryption key not set"
        return 1
    fi
    
    local -a sensitive_vars=(
        "DB_PASSWORD"
        "API_KEY"
        "JWT_SECRET"
    )
    
    for var in "${sensitive_vars[@]}"; do
        local encrypted_var="${var}_ENCRYPTED"
        if [[ -n "${!encrypted_var:-}" ]]; then
            local decrypted_value=$(echo "${!encrypted_var}" | openssl enc -aes-256-cbc -d -a -k "$encryption_key")
            export "$var"="$decrypted_value"
            unset "$encrypted_var"
            log_message $LOG_LEVEL_DEBUG "Decrypted environment variable: $var"
        fi
    done
}

# =============================================================================
# ENVIRONMENT-SPECIFIC CONFIGURATION
# =============================================================================

# Load environment-specific configuration
load_environment_specific_config() {
    local environment="${FINANCIAL_APP_ENVIRONMENT:-development}"
    local config_dir="${CONFIG_DIR:-/etc/fintech}"
    
    log_message $LOG_LEVEL_INFO "Loading configuration for environment: $environment"
    
    # Load common configuration
    local common_config="$config_dir/common.conf"
    if [[ -f "$common_config" ]]; then
        load_configuration "$common_config" "ini"
    fi
    
    # Load environment-specific configuration
    local env_config="$config_dir/${environment}.conf"
    if [[ -f "$env_config" ]]; then
        load_configuration "$env_config" "ini"
    else
        log_message $LOG_LEVEL_WARN "Environment-specific configuration not found: $env_config"
    fi
    
    # Load secrets if available
    local secrets_file="$config_dir/secrets.conf"
    if [[ -f "$secrets_file" ]]; then
        load_configuration "$secrets_file" "ini"
        log_message $LOG_LEVEL_DEBUG "Secrets loaded from: $secrets_file"
    fi
}

# =============================================================================
# CONFIGURATION VALIDATION
# =============================================================================

# Validate configuration values
validate_configuration() {
    local validation_errors=0
    
    log_message $LOG_LEVEL_INFO "Validating configuration"
    
    # Validate database configuration
    if ! validate_database_config; then
        ((validation_errors++))
    fi
    
    # Validate API configuration
    if ! validate_api_config; then
        ((validation_errors++))
    fi
    
    # Validate security configuration
    if ! validate_security_config; then
        ((validation_errors++))
    fi
    
    # Validate file paths
    if ! validate_file_paths; then
        ((validation_errors++))
    fi
    
    if [[ $validation_errors -eq 0 ]]; then
        log_message $LOG_LEVEL_INFO "Configuration validation passed"
        return 0
    else
        log_message $LOG_LEVEL_ERROR "Configuration validation failed: $validation_errors errors"
        return 1
    fi
}

# Validate database configuration
validate_database_config() {
    local errors=0
    
    # Check required database variables
    local -a required_db_vars=("DB_HOST" "DB_PORT" "DB_NAME" "DB_USER")
    for var in "${required_db_vars[@]}"; do
        if [[ -z "${!var:-}" ]]; then
            log_message $LOG_LEVEL_ERROR "Missing required database variable: $var"
            ((errors++))
        fi
    done
    
    # Validate port number
    if [[ -n "${DB_PORT:-}" ]] && ! [[ "$DB_PORT" =~ ^[0-9]+$ ]] || [[ "$DB_PORT" -lt 1 ]] || [[ "$DB_PORT" -gt 65535 ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid database port: $DB_PORT"
        ((errors++))
    fi
    
    return $errors
}

# Validate API configuration
validate_api_config() {
    local errors=0
    
    # Check API URL format
    if [[ -n "${API_BASE_URL:-}" ]] && ! [[ "$API_BASE_URL" =~ ^https?:// ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid API base URL format: $API_BASE_URL"
        ((errors++))
    fi
    
    # Validate timeout values
    if [[ -n "${API_TIMEOUT:-}" ]] && ! [[ "$API_TIMEOUT" =~ ^[0-9]+$ ]] || [[ "$API_TIMEOUT" -lt 1 ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid API timeout: $API_TIMEOUT"
        ((errors++))
    fi
    
    return $errors
}

# Validate security configuration
validate_security_config() {
    local errors=0
    
    # Check if encryption key is set
    if [[ -z "${ENCRYPTION_KEY:-}" ]]; then
        log_message $LOG_LEVEL_WARN "Encryption key not set - sensitive data will not be encrypted"
    fi
    
    # Validate session timeout
    if [[ -n "${SESSION_TIMEOUT:-}" ]] && ! [[ "$SESSION_TIMEOUT" =~ ^[0-9]+$ ]] || [[ "$SESSION_TIMEOUT" -lt 60 ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid session timeout: $SESSION_TIMEOUT (minimum 60 seconds)"
        ((errors++))
    fi
    
    return $errors
}

# Validate file paths
validate_file_paths() {
    local errors=0
    
    # Check if directories exist or can be created
    local -a required_dirs=("CONFIG_DIR" "DATA_DIR" "TEMP_DIR")
    for var in "${required_dirs[@]}"; do
        local dir="${!var:-}"
        if [[ -n "$dir" ]]; then
            if [[ ! -d "$dir" ]]; then
                if mkdir -p "$dir" 2>/dev/null; then
                    log_message $LOG_LEVEL_DEBUG "Created directory: $dir"
                else
                    log_message $LOG_LEVEL_ERROR "Cannot create directory: $dir"
                    ((errors++))
                fi
            fi
        fi
    done
    
    return $errors
}

# =============================================================================
# ENVIRONMENT INFORMATION
# =============================================================================

# Display environment information
display_environment_info() {
    log_message $LOG_LEVEL_INFO "Environment Information:"
    log_message $LOG_LEVEL_INFO "  Application: $FINANCIAL_APP_NAME v$FINANCIAL_APP_VERSION"
    log_message $LOG_LEVEL_INFO "  Environment: $FINANCIAL_APP_ENVIRONMENT"
    log_message $LOG_LEVEL_INFO "  Log Level: $LOG_LEVEL"
    log_message $LOG_LEVEL_INFO "  Database: $DB_HOST:$DB_PORT/$DB_NAME"
    log_message $LOG_LEVEL_INFO "  API Base URL: $API_BASE_URL"
    log_message $LOG_LEVEL_INFO "  Config Directory: $CONFIG_DIR"
    log_message $LOG_LEVEL_INFO "  Data Directory: $DATA_DIR"
    log_message $LOG_LEVEL_INFO "  Max Workers: $MAX_WORKERS"
    log_message $LOG_LEVEL_INFO "  Batch Size: $BATCH_SIZE"
}

# Export environment to file
export_environment() {
    local output_file="${1:-/tmp/fintech_environment_$(date +%Y%m%d_%H%M%S).env}"
    
    log_message $LOG_LEVEL_INFO "Exporting environment to: $output_file"
    
    # Get all environment variables starting with FINANCIAL_ or DB_ or API_
    env | grep -E '^(FINANCIAL_|DB_|API_|LOG_|CONFIG_|DATA_|TEMP_|BACKUP_|MAX_|BATCH_|CACHE_|SESSION_|ENCRYPTION_|JWT_)' > "$output_file"
    
    log_message $LOG_LEVEL_INFO "Environment exported successfully"
    echo "$output_file"
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting environment variables demonstration"
    
    # Set default environment
    set_default_environment
    
    # Load environment-specific configuration
    load_environment_specific_config
    
    # Validate environment
    if ! validate_environment; then
        log_message $LOG_LEVEL_ERROR "Environment validation failed"
        exit 1
    fi
    
    # Validate configuration
    if ! validate_configuration; then
        log_message $LOG_LEVEL_ERROR "Configuration validation failed"
        exit 1
    fi
    
    # Display environment information
    display_environment_info
    
    # Mask sensitive variables
    mask_sensitive_vars
    
    # Test configuration loading
    local test_config="/tmp/test_config.conf"
    cat > "$test_config" << EOF
# Test configuration file
[general]
app_name=TestApp
app_version=1.0.0
debug_mode=true

[database]
host=test-db.example.com
port=5432
name=test_db

[api]
base_url=https://test-api.example.com
timeout=60
retry_count=5
EOF
    
    log_message $LOG_LEVEL_INFO "Testing configuration loading"
    load_configuration "$test_config" "ini"
    
    # Test JSON configuration
    local test_json_config="/tmp/test_config.json"
    cat > "$test_json_config" << EOF
{
    "app_name": "TestApp",
    "app_version": "1.0.0",
    "debug_mode": true,
    "database": {
        "host": "test-db.example.com",
        "port": 5432,
        "name": "test_db"
    },
    "api": {
        "base_url": "https://test-api.example.com",
        "timeout": 60,
        "retry_count": 5
    }
}
EOF
    
    if command -v jq >/dev/null 2>&1; then
        log_message $LOG_LEVEL_INFO "Testing JSON configuration loading"
        load_configuration "$test_json_config" "json"
    else
        log_message $LOG_LEVEL_WARN "jq not available - skipping JSON configuration test"
    fi
    
    # Export environment
    local env_file=$(export_environment)
    log_message $LOG_LEVEL_INFO "Environment exported to: $env_file"
    
    # Cleanup
    rm -f "$test_config" "$test_json_config"
    
    log_message $LOG_LEVEL_INFO "Environment variables demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    main "$@"
    exit 0
fi
