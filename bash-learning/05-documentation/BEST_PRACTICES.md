# Bash Scripting Best Practices for Fintech Applications

## Table of Contents
1. [Code Quality Standards](#code-quality-standards)
2. [Security Best Practices](#security-best-practices)
3. [Performance Optimization](#performance-optimization)
4. [Error Handling and Logging](#error-handling-and-logging)
5. [Testing and Validation](#testing-and-validation)
6. [Documentation Standards](#documentation-standards)
7. [Production Deployment](#production-deployment)
8. [Monitoring and Observability](#monitoring-and-observability)
9. [Compliance and Audit](#compliance-and-audit)
10. [Code Review Guidelines](#code-review-guidelines)

## Code Quality Standards

### Script Structure and Organization

#### 1. Script Header
Every production bash script must include a comprehensive header:

```bash
#!/bin/bash
#
# Script Name: Financial Data Processor
# Description: Processes high-frequency market data for trading systems
# Author: System Engineering Team
# Version: 1.2.0
# Last Modified: 2024-01-15
# Dependencies: bc, jq, curl
# Requirements: Bash 4.0+, GNU coreutils
# License: Proprietary
# Security Classification: Confidential
#

# =============================================================================
# SCRIPT CONFIGURATION AND SAFETY SETTINGS
# =============================================================================

# Enable strict error handling for production environments
set -euo pipefail

# Set IFS to prevent word splitting issues
IFS=$'\n\t'

# Enable extended globbing for pattern matching
shopt -s extglob

# Enable null globbing to prevent errors with empty patterns
shopt -s nullglob
```

#### 2. Variable Naming Conventions
- Use UPPERCASE for constants and configuration variables
- Use lowercase_with_underscores for local variables
- Use descriptive names that indicate purpose
- Prefix sensitive variables with `SENSITIVE_`

```bash
# Good examples
readonly API_ENDPOINT="https://api.financial-data.com"
readonly MAX_RETRY_ATTEMPTS=3
local portfolio_value=0
local SENSITIVE_API_KEY="${FINANCIAL_API_KEY:-}"

# Bad examples
readonly url="https://api.com"  # Too generic
local x=5  # Not descriptive
local api_key="$KEY"  # Not indicating sensitivity
```

#### 3. Function Organization
- Group related functions together
- Use clear, descriptive function names
- Include comprehensive documentation
- Follow single responsibility principle

```bash
# =============================================================================
# FINANCIAL CALCULATIONS
# =============================================================================

# Calculate portfolio value with proper error handling
calculate_portfolio_value() {
    local positions_file="$1"
    local current_prices_file="$2"
    
    # Input validation
    if [[ ! -f "$positions_file" ]]; then
        log_error "Positions file not found: $positions_file"
        return 1
    fi
    
    if [[ ! -f "$current_prices_file" ]]; then
        log_error "Prices file not found: $current_prices_file"
        return 1
    fi
    
    # Calculate total portfolio value
    local total_value=0
    while IFS=',' read -r symbol quantity; do
        local price=$(get_current_price "$symbol" "$current_prices_file")
        local position_value=$(echo "scale=2; $quantity * $price" | bc -l)
        total_value=$(echo "scale=2; $total_value + $position_value" | bc -l)
    done < "$positions_file"
    
    echo "$total_value"
}
```

## Security Best Practices

### 1. Input Validation and Sanitization

#### Validate All Inputs
```bash
# Validate financial data inputs
validate_financial_input() {
    local amount="$1"
    local currency="$2"
    
    # Check if amount is numeric and positive
    if ! [[ "$amount" =~ ^[0-9]+\.?[0-9]*$ ]] || (( $(echo "$amount <= 0" | bc -l) )); then
        log_error "Invalid amount: $amount"
        return 1
    fi
    
    # Validate currency format (3-letter code)
    if ! [[ "$currency" =~ ^[A-Z]{3}$ ]]; then
        log_error "Invalid currency format: $currency"
        return 1
    fi
    
    return 0
}
```

#### Sanitize File Paths
```bash
# Sanitize file paths to prevent directory traversal
sanitize_file_path() {
    local input_path="$1"
    local base_dir="/secure/financial_data"
    
    # Remove any path traversal attempts
    local sanitized_path="${input_path//..\//}"
    sanitized_path="${sanitized_path//\/..\//}"
    
    # Ensure path is within base directory
    local full_path="$base_dir/$sanitized_path"
    if [[ "$full_path" != "$base_dir"* ]]; then
        log_error "Path traversal attempt detected: $input_path"
        return 1
    fi
    
    echo "$full_path"
}
```

### 2. Secure Data Handling

#### Handle Sensitive Data
```bash
# Secure handling of API keys and passwords
handle_sensitive_data() {
    local api_key="${FINANCIAL_API_KEY:-}"
    
    if [[ -z "$api_key" ]]; then
        log_error "Financial API key not set"
        return 1
    fi
    
    # Mask sensitive data in logs
    local masked_key="${api_key:0:4}****${api_key: -4}"
    log_debug "Using API key: $masked_key"
    
    # Clear sensitive variables after use
    unset api_key
    
    return 0
}
```

#### Encrypt Sensitive Files
```bash
# Encrypt sensitive data files
encrypt_sensitive_file() {
    local input_file="$1"
    local output_file="$2"
    local encryption_key="${ENCRYPTION_KEY:-}"
    
    if [[ -z "$encryption_key" ]]; then
        log_error "Encryption key not set"
        return 1
    fi
    
    # Encrypt file using AES-256
    openssl enc -aes-256-cbc -salt -in "$input_file" -out "$output_file" -k "$encryption_key"
    
    if [[ $? -eq 0 ]]; then
        log_info "File encrypted successfully: $output_file"
        # Remove original file
        rm "$input_file"
    else
        log_error "Failed to encrypt file: $input_file"
        return 1
    fi
}
```

### 3. Access Control and Permissions

#### Set Proper File Permissions
```bash
# Set secure file permissions
set_secure_permissions() {
    local file_path="$1"
    local file_type="$2"
    
    case "$file_type" in
        "config")
            chmod 600 "$file_path"  # Read/write for owner only
            ;;
        "log")
            chmod 644 "$file_path"  # Read for all, write for owner
            ;;
        "executable")
            chmod 750 "$file_path"  # Execute for owner and group
            ;;
        "sensitive")
            chmod 600 "$file_path"  # Read/write for owner only
            chown root:root "$file_path"
            ;;
    esac
    
    log_debug "Set permissions for $file_path: $file_type"
}
```

## Performance Optimization

### 1. Efficient Data Processing

#### Use Arrays for Large Datasets
```bash
# Process large datasets efficiently
process_market_data_efficiently() {
    local data_file="$1"
    
    # Read all data into array for faster processing
    local -a market_data
    mapfile -t market_data < "$data_file"
    
    # Process data in batches
    local batch_size=1000
    local total_records=${#market_data[@]}
    
    for ((i=0; i<total_records; i+=batch_size)); do
        local end_index=$((i + batch_size))
        [[ $end_index -gt $total_records ]] && end_index=$total_records
        
        # Process batch
        for ((j=i; j<end_index; j++)); do
            process_single_record "${market_data[j]}"
        done
    done
}
```

#### Minimize External Command Calls
```bash
# Inefficient - multiple external calls
calculate_portfolio_value_inefficient() {
    local total=0
    for symbol in "${symbols[@]}"; do
        local price=$(get_price_from_api "$symbol")  # External call
        local quantity=$(get_quantity_from_db "$symbol")  # External call
        local value=$(echo "$price * $quantity" | bc -l)  # External call
        total=$(echo "$total + $value" | bc -l)  # External call
    done
    echo "$total"
}

# Efficient - minimize external calls
calculate_portfolio_value_efficient() {
    local total=0
    local -a prices
    local -a quantities
    
    # Batch API calls
    get_prices_batch "${symbols[@]}" prices
    get_quantities_batch "${symbols[@]}" quantities
    
    # Calculate in memory
    for ((i=0; i<${#symbols[@]}; i++)); do
        local value=$(echo "scale=2; ${prices[i]} * ${quantities[i]}" | bc -l)
        total=$(echo "scale=2; $total + $value" | bc -l)
    done
    echo "$total"
}
```

### 2. Memory Management

#### Use Local Variables
```bash
# Use local variables to avoid memory leaks
process_financial_data() {
    local data_file="$1"
    local output_file="$2"
    
    # Local variables are automatically cleaned up
    local -a processed_data
    local temp_file="/tmp/processing_$$"
    
    # Process data
    while IFS=',' read -r symbol price volume; do
        local processed_record="${symbol},${price},${volume},$(date +%s)"
        processed_data+=("$processed_record")
    done < "$data_file"
    
    # Write results
    printf '%s\n' "${processed_data[@]}" > "$output_file"
    
    # Cleanup
    rm -f "$temp_file"
}
```

## Error Handling and Logging

### 1. Comprehensive Error Handling

#### Use Proper Exit Codes
```bash
# Define exit codes for different error types
readonly EXIT_SUCCESS=0
readonly EXIT_GENERAL_ERROR=1
readonly EXIT_MISUSE=2
readonly EXIT_CANNOT_EXECUTE=126
readonly EXIT_COMMAND_NOT_FOUND=127
readonly EXIT_INVALID_ARGUMENT=128
readonly EXIT_FATAL_ERROR=130
readonly EXIT_CONFIG_ERROR=131
readonly EXIT_DATA_ERROR=132
readonly EXIT_NETWORK_ERROR=133
readonly EXIT_PERMISSION_ERROR=134
```

#### Implement Retry Logic
```bash
# Retry function with exponential backoff
retry_with_backoff() {
    local max_attempts="$1"
    local delay="$2"
    local command="$3"
    local attempt=1
    
    while [[ $attempt -le $max_attempts ]]; do
        log_debug "Attempt $attempt/$max_attempts: $command"
        
        if eval "$command"; then
            log_debug "Command succeeded on attempt $attempt"
            return 0
        else
            local exit_code=$?
            log_warn "Command failed on attempt $attempt with exit code $exit_code"
            
            if [[ $attempt -lt $max_attempts ]]; then
                log_debug "Waiting $delay seconds before retry"
                sleep "$delay"
                delay=$((delay * 2))  # Exponential backoff
            fi
        fi
        
        ((attempt++))
    done
    
    log_error "Command failed after $max_attempts attempts"
    return 1
}
```

### 2. Structured Logging

#### Implement Logging Levels
```bash
# Logging configuration
readonly LOG_LEVEL_ERROR=1
readonly LOG_LEVEL_WARN=2
readonly LOG_LEVEL_INFO=3
readonly LOG_LEVEL_DEBUG=4
readonly CURRENT_LOG_LEVEL="${LOG_LEVEL:-$LOG_LEVEL_INFO}"

# Structured logging function
log_message() {
    local level="$1"
    local message="$2"
    local component="${3:-$(basename "$0")}"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S.%3N')
    local pid=$$
    local audit_id=$(uuidgen 2>/dev/null || echo "AUDIT-$(date +%s)")
    
    if [[ "$level" -le "$CURRENT_LOG_LEVEL" ]]; then
        case "$level" in
            "$LOG_LEVEL_ERROR")
                echo "{\"timestamp\":\"$timestamp\",\"level\":\"ERROR\",\"component\":\"$component\",\"pid\":$pid,\"audit_id\":\"$audit_id\",\"message\":\"$message\"}" >&2
                ;;
            "$LOG_LEVEL_WARN")
                echo "{\"timestamp\":\"$timestamp\",\"level\":\"WARN\",\"component\":\"$component\",\"pid\":$pid,\"audit_id\":\"$audit_id\",\"message\":\"$message\"}" >&2
                ;;
            "$LOG_LEVEL_INFO")
                echo "{\"timestamp\":\"$timestamp\",\"level\":\"INFO\",\"component\":\"$component\",\"pid\":$pid,\"audit_id\":\"$audit_id\",\"message\":\"$message\"}" >&2
                ;;
            "$LOG_LEVEL_DEBUG")
                echo "{\"timestamp\":\"$timestamp\",\"level\":\"DEBUG\",\"component\":\"$component\",\"pid\":$pid,\"audit_id\":\"$audit_id\",\"message\":\"$message\"}" >&2
                ;;
        esac
    fi
    
    # Write to audit log
    echo "$timestamp,$audit_id,$level,$component,$message" >> /var/log/fintech_audit.log
}
```

## Testing and Validation

### 1. Unit Testing

#### Test Structure
```bash
# Test function structure
test_function_name() {
    local test_name="test_function_name"
    log_debug "Running test: $test_name"
    
    # Setup
    local test_data="test_value"
    local expected_result="expected_value"
    
    # Execute
    local actual_result=$(function_under_test "$test_data")
    
    # Assert
    if [[ "$actual_result" == "$expected_result" ]]; then
        log_debug "PASS: $test_name"
        return 0
    else
        log_error "FAIL: $test_name - Expected: $expected_result, Actual: $actual_result"
        return 1
    fi
}
```

#### Test Coverage
```bash
# Calculate test coverage
calculate_test_coverage() {
    local source_file="$1"
    local test_file="$2"
    
    # Count total functions
    local total_functions=$(grep -c "^[a-zA-Z_][a-zA-Z0-9_]*()" "$source_file")
    
    # Count tested functions
    local tested_functions=$(grep -c "test_.*()" "$test_file")
    
    # Calculate coverage percentage
    local coverage=$(echo "scale=2; $tested_functions * 100 / $total_functions" | bc -l)
    
    log_info "Test coverage: $coverage% ($tested_functions/$total_functions functions)"
    
    if (( $(echo "$coverage < 80" | bc -l) )); then
        log_warn "Test coverage below 80% threshold"
        return 1
    fi
    
    return 0
}
```

### 2. Integration Testing

#### End-to-End Testing
```bash
# End-to-end test for financial data processing
test_end_to_end_processing() {
    local test_name="test_end_to_end_processing"
    log_debug "Running test: $test_name"
    
    # Setup test environment
    local test_dir="/tmp/e2e_test_$$"
    mkdir -p "$test_dir"
    
    # Create test data
    cat > "$test_dir/input.csv" << EOF
symbol,price,volume
AAPL,150.25,1000000
GOOGL,2800.50,500000
EOF
    
    # Execute main processing function
    process_financial_data "$test_dir/input.csv" "$test_dir/output.csv"
    
    # Verify output
    if [[ -f "$test_dir/output.csv" ]]; then
        local output_lines=$(wc -l < "$test_dir/output.csv")
        if [[ $output_lines -eq 3 ]]; then  # Header + 2 data lines
            log_debug "PASS: $test_name"
            rm -rf "$test_dir"
            return 0
        else
            log_error "FAIL: $test_name - Expected 3 lines, got $output_lines"
        fi
    else
        log_error "FAIL: $test_name - Output file not created"
    fi
    
    # Cleanup
    rm -rf "$test_dir"
    return 1
}
```

## Documentation Standards

### 1. Code Documentation

#### Function Documentation
```bash
# =============================================================================
# FINANCIAL CALCULATIONS
# =============================================================================

# Calculate portfolio value with proper error handling and validation
#
# This function calculates the total value of a portfolio by multiplying
# each position's quantity by its current market price and summing the results.
# It includes comprehensive input validation and error handling for production use.
#
# Arguments:
#   $1: positions_file - Path to CSV file containing position data
#       Format: symbol,quantity
#   $2: prices_file - Path to CSV file containing current market prices
#       Format: symbol,price
#
# Returns:
#   0: Success - Portfolio value calculated and printed to stdout
#   1: Error - Invalid input files or data format
#   2: Error - Calculation overflow or underflow
#
# Side Effects:
#   - Logs calculation details to audit log
#   - Updates portfolio statistics
#
# Example:
#   local portfolio_value=$(calculate_portfolio_value "positions.csv" "prices.csv")
#   if [[ $? -eq 0 ]]; then
#       echo "Portfolio value: $portfolio_value"
#   fi
#
# Dependencies:
#   - bc (for floating-point arithmetic)
#   - validate_financial_input function
#   - log_message function
#
# Performance:
#   - Time Complexity: O(n) where n is number of positions
#   - Space Complexity: O(1)
#   - Typical execution time: < 100ms for 1000 positions
#
# Security Considerations:
#   - Validates all input data to prevent injection attacks
#   - Sanitizes file paths to prevent directory traversal
#   - Logs all calculations for audit purposes
#
calculate_portfolio_value() {
    local positions_file="$1"
    local prices_file="$2"
    
    # Input validation
    if [[ ! -f "$positions_file" ]]; then
        log_error "Positions file not found: $positions_file"
        return 1
    fi
    
    if [[ ! -f "$prices_file" ]]; then
        log_error "Prices file not found: $prices_file"
        return 1
    fi
    
    # Calculate total portfolio value
    local total_value=0
    local position_count=0
    
    while IFS=',' read -r symbol quantity; do
        # Validate position data
        if ! validate_financial_input "$quantity" "USD"; then
            log_error "Invalid quantity for $symbol: $quantity"
            continue
        fi
        
        # Get current price
        local price=$(get_current_price "$symbol" "$prices_file")
        if [[ -z "$price" ]]; then
            log_warn "Price not found for $symbol, skipping"
            continue
        fi
        
        # Calculate position value
        local position_value=$(echo "scale=2; $quantity * $price" | bc -l)
        total_value=$(echo "scale=2; $total_value + $position_value" | bc -l)
        
        ((position_count++))
        log_debug "Position $position_count: $symbol - $quantity @ $price = $position_value"
        
    done < "$positions_file"
    
    log_info "Portfolio calculation completed: $position_count positions, total value: $total_value"
    echo "$total_value"
}
```

### 2. API Documentation

#### Script Interface Documentation
```bash
# =============================================================================
# SCRIPT INTERFACE DOCUMENTATION
# =============================================================================

# Script: financial_data_processor.sh
# Version: 1.2.0
# Purpose: Process high-frequency financial market data for trading systems
#
# SYNOPSIS:
#   financial_data_processor.sh [OPTIONS] INPUT_FILE OUTPUT_FILE
#
# DESCRIPTION:
#   This script processes financial market data from various sources and formats
#   it for consumption by trading systems. It includes data validation, cleaning,
#   and transformation capabilities with comprehensive error handling and logging.
#
# ARGUMENTS:
#   INPUT_FILE    Path to input data file (CSV, JSON, or XML format)
#   OUTPUT_FILE   Path to output processed data file (CSV format)
#
# OPTIONS:
#   -h, --help              Display this help message and exit
#   -v, --version           Display version information and exit
#   -c, --config FILE       Use configuration file (default: /etc/fintech/config.conf)
#   -l, --log-level LEVEL   Set logging level (ERROR, WARN, INFO, DEBUG)
#   -t, --test              Run in test mode (no actual processing)
#   -d, --dry-run           Show what would be done without executing
#   -r, --retry COUNT       Number of retry attempts for failed operations (default: 3)
#   -w, --workers COUNT     Number of parallel workers (default: 4)
#   -s, --strict            Enable strict mode (fail on any warning)
#   -q, --quiet             Suppress non-error output
#   -V, --verbose           Enable verbose output
#
# CONFIGURATION:
#   The script can be configured using a configuration file specified with -c.
#   Configuration file format (INI-style):
#   [general]
#   log_level = INFO
#   max_workers = 4
#   retry_count = 3
#   
#   [data]
#   input_format = csv
#   output_format = csv
#   validate_data = true
#   
#   [security]
#   encrypt_output = false
#   audit_log = true
#
# ENVIRONMENT VARIABLES:
#   FINANCIAL_API_KEY       API key for financial data services
#   LOG_LEVEL              Default logging level
#   CONFIG_DIR             Directory containing configuration files
#   TEMP_DIR               Directory for temporary files
#   AUDIT_LOG_FILE         Path to audit log file
#
# EXIT CODES:
#   0   Success
#   1   General error
#   2   Invalid arguments
#   3   Configuration error
#   4   Data processing error
#   5   Network error
#   6   Permission error
#   7   Resource error
#
# EXAMPLES:
#   # Basic usage
#   ./financial_data_processor.sh input.csv output.csv
#
#   # With configuration file
#   ./financial_data_processor.sh -c /etc/fintech/config.conf input.csv output.csv
#
#   # With custom logging level
#   ./financial_data_processor.sh -l DEBUG input.csv output.csv
#
#   # Dry run to see what would be processed
#   ./financial_data_processor.sh -d input.csv output.csv
#
#   # Test mode
#   ./financial_data_processor.sh -t input.csv output.csv
#
# FILES:
#   /etc/fintech/config.conf    Default configuration file
#   /var/log/fintech_audit.log  Audit log file
#   /tmp/financial_data_*       Temporary files (cleaned up on exit)
#
# BUGS:
#   Report bugs to system-engineering@company.com
#
# AUTHOR:
#   System Engineering Team <system-engineering@company.com>
#
# COPYRIGHT:
#   Copyright (c) 2024 Company Name. All rights reserved.
#
# LICENSE:
#   Proprietary software. Unauthorized copying prohibited.
#
# SEE ALSO:
#   financial_data_validator.sh(1)
#   trading_system_config.conf(5)
#   fintech_audit_log(5)
```

## Production Deployment

### 1. Deployment Checklist

#### Pre-Deployment Validation
```bash
# Pre-deployment validation script
validate_deployment() {
    local script_path="$1"
    local environment="$2"
    
    log_info "Starting pre-deployment validation for $script_path"
    
    # Check script syntax
    if ! bash -n "$script_path"; then
        log_error "Script syntax error in $script_path"
        return 1
    fi
    
    # Check for required dependencies
    check_dependencies "$script_path"
    
    # Check file permissions
    check_file_permissions "$script_path"
    
    # Run unit tests
    run_unit_tests "$script_path"
    
    # Run integration tests
    run_integration_tests "$script_path" "$environment"
    
    # Check security vulnerabilities
    check_security_vulnerabilities "$script_path"
    
    # Validate configuration
    validate_configuration "$environment"
    
    log_info "Pre-deployment validation completed successfully"
    return 0
}
```

#### Deployment Process
```bash
# Production deployment process
deploy_to_production() {
    local script_path="$1"
    local target_environment="$2"
    
    log_info "Starting production deployment"
    
    # Create backup
    create_backup "$script_path"
    
    # Deploy to staging first
    deploy_to_staging "$script_path"
    
    # Run smoke tests
    run_smoke_tests "$target_environment"
    
    # Deploy to production
    deploy_to_production_environment "$script_path" "$target_environment"
    
    # Verify deployment
    verify_deployment "$script_path" "$target_environment"
    
    # Update monitoring
    update_monitoring_configuration "$script_path"
    
    log_info "Production deployment completed successfully"
}
```

### 2. Rollback Procedures

#### Automated Rollback
```bash
# Automated rollback procedure
rollback_deployment() {
    local script_path="$1"
    local target_environment="$2"
    local rollback_reason="$3"
    
    log_warn "Starting rollback: $rollback_reason"
    
    # Stop current deployment
    stop_current_deployment "$script_path" "$target_environment"
    
    # Restore from backup
    restore_from_backup "$script_path"
    
    # Verify rollback
    verify_rollback "$script_path" "$target_environment"
    
    # Notify stakeholders
    notify_rollback "$rollback_reason"
    
    log_info "Rollback completed successfully"
}
```

## Monitoring and Observability

### 1. Health Checks

#### Application Health Check
```bash
# Health check function
check_application_health() {
    local health_status="healthy"
    local health_details=""
    
    # Check script execution
    if ! check_script_execution; then
        health_status="unhealthy"
        health_details="Script execution failed"
    fi
    
    # Check data processing
    if ! check_data_processing; then
        health_status="unhealthy"
        health_details="Data processing failed"
    fi
    
    # Check external dependencies
    if ! check_external_dependencies; then
        health_status="degraded"
        health_details="External dependencies unavailable"
    fi
    
    # Check resource usage
    if ! check_resource_usage; then
        health_status="degraded"
        health_details="High resource usage"
    fi
    
    # Return health status
    echo "{\"status\":\"$health_status\",\"details\":\"$health_details\",\"timestamp\":\"$(date -u +%Y-%m-%dT%H:%M:%S.%3NZ)\"}"
}
```

### 2. Metrics Collection

#### Performance Metrics
```bash
# Collect performance metrics
collect_performance_metrics() {
    local metrics_file="/tmp/performance_metrics_$$.json"
    
    cat > "$metrics_file" << EOF
{
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%S.%3NZ)",
  "script": "$(basename "$0")",
  "pid": $$,
  "metrics": {
    "execution_time": "$(get_execution_time)",
    "memory_usage": "$(get_memory_usage)",
    "cpu_usage": "$(get_cpu_usage)",
    "disk_usage": "$(get_disk_usage)",
    "network_io": "$(get_network_io)",
    "processed_records": "$(get_processed_records)",
    "error_count": "$(get_error_count)",
    "success_rate": "$(get_success_rate)"
  }
}
EOF
    
    # Send metrics to monitoring system
    send_metrics_to_monitoring "$metrics_file"
    
    # Cleanup
    rm "$metrics_file"
}
```

## Compliance and Audit

### 1. Audit Logging

#### Comprehensive Audit Trail
```bash
# Audit logging function
log_audit_event() {
    local event_type="$1"
    local event_description="$2"
    local user_id="${3:-$(whoami)}"
    local session_id="${4:-$(uuidgen 2>/dev/null || echo "SESSION-$(date +%s)")}"
    local timestamp=$(date -u +%Y-%m-%dT%H:%M:%S.%3NZ)
    local audit_id=$(uuidgen 2>/dev/null || echo "AUDIT-$(date +%s)")
    
    # Create audit log entry
    local audit_entry=$(cat << EOF
{
  "audit_id": "$audit_id",
  "timestamp": "$timestamp",
  "event_type": "$event_type",
  "event_description": "$event_description",
  "user_id": "$user_id",
  "session_id": "$session_id",
  "script": "$(basename "$0")",
  "pid": $$,
  "hostname": "$(hostname)",
  "ip_address": "$(get_ip_address)",
  "environment": "${ENVIRONMENT:-production}"
}
EOF
)
    
    # Write to audit log
    echo "$audit_entry" >> /var/log/fintech_audit.log
    
    # Send to centralized logging system
    send_to_centralized_logging "$audit_entry"
    
    log_debug "Audit event logged: $event_type - $event_description"
}
```

### 2. Compliance Validation

#### SOX Compliance Check
```bash
# SOX compliance validation
validate_sox_compliance() {
    local compliance_status="compliant"
    local compliance_issues=()
    
    # Check data retention
    if ! check_data_retention; then
        compliance_status="non_compliant"
        compliance_issues+=("Data retention policy violation")
    fi
    
    # Check access controls
    if ! check_access_controls; then
        compliance_status="non_compliant"
        compliance_issues+=("Access control violation")
    fi
    
    # Check audit trail
    if ! check_audit_trail; then
        compliance_status="non_compliant"
        compliance_issues+=("Audit trail incomplete")
    fi
    
    # Check data integrity
    if ! check_data_integrity; then
        compliance_status="non_compliant"
        compliance_issues+=("Data integrity violation")
    fi
    
    # Log compliance status
    log_audit_event "compliance_check" "SOX compliance validation - Status: $compliance_status"
    
    if [[ "$compliance_status" == "non_compliant" ]]; then
        log_error "SOX compliance violations detected: ${compliance_issues[*]}"
        return 1
    fi
    
    log_info "SOX compliance validation passed"
    return 0
}
```

## Code Review Guidelines

### 1. Review Checklist

#### Security Review
- [ ] Input validation implemented
- [ ] Output sanitization applied
- [ ] Sensitive data properly handled
- [ ] File permissions set correctly
- [ ] No hardcoded credentials
- [ ] Error messages don't leak sensitive information

#### Performance Review
- [ ] Efficient data structures used
- [ ] Minimal external command calls
- [ ] Proper memory management
- [ ] No unnecessary loops or operations
- [ ] Caching implemented where appropriate

#### Code Quality Review
- [ ] Clear and descriptive variable names
- [ ] Functions follow single responsibility principle
- [ ] Comprehensive error handling
- [ ] Proper logging and debugging
- [ ] Code is well-documented
- [ ] Follows established coding standards

### 2. Review Process

#### Automated Review
```bash
# Automated code review script
automated_code_review() {
    local script_path="$1"
    
    log_info "Starting automated code review for $script_path"
    
    # Check syntax
    check_syntax "$script_path"
    
    # Check for common issues
    check_common_issues "$script_path"
    
    # Check security vulnerabilities
    check_security_issues "$script_path"
    
    # Check performance issues
    check_performance_issues "$script_path"
    
    # Check compliance
    check_compliance_issues "$script_path"
    
    # Generate review report
    generate_review_report "$script_path"
    
    log_info "Automated code review completed"
}
```

This comprehensive best practices guide provides the foundation for writing production-grade bash scripts for fintech applications. Each section includes practical examples and real-world scenarios that demonstrate how to implement these practices effectively.
