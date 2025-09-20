#!/bin/bash
#
# Bash Fundamentals: Syntax and Variables
# Production-Grade Script for Fintech Applications
#
# This script demonstrates core bash syntax and variable handling
# with emphasis on security, reliability, and fintech requirements.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#

# =============================================================================
# SCRIPT CONFIGURATION AND SAFETY SETTINGS
# =============================================================================

# Enable strict error handling for production environments
set -euo pipefail

# Set IFS to prevent word splitting issues with spaces
IFS=$'\n\t'

# Enable extended globbing for pattern matching
shopt -s extglob

# Enable null globbing to prevent errors with empty patterns
shopt -s nullglob

# =============================================================================
# LOGGING AND DEBUGGING CONFIGURATION
# =============================================================================

# Define log levels for production logging
readonly LOG_LEVEL_ERROR=1
readonly LOG_LEVEL_WARN=2
readonly LOG_LEVEL_INFO=3
readonly LOG_LEVEL_DEBUG=4

# Current log level (can be overridden by environment variable)
readonly CURRENT_LOG_LEVEL="${LOG_LEVEL:-$LOG_LEVEL_INFO}"

# Logging function for production environments
log_message() {
    local level="$1"
    local message="$2"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S.%3N')
    
    # Only log if current level allows it
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
# VARIABLE DECLARATION AND INITIALIZATION
# =============================================================================

# Basic variable types and best practices
declare -r SCRIPT_NAME="$(basename "$0")"
declare -r SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
declare -r SCRIPT_VERSION="1.0.0"

# Financial data variables (using appropriate data types)
declare -i trade_count=0
declare -f exchange_rate=0.0
declare -a currency_pairs=("USD/EUR" "GBP/USD" "JPY/USD" "AUD/USD")
declare -A market_data=(
    ["NASDAQ"]="Technology"
    ["NYSE"]="Diversified"
    ["LSE"]="International"
    ["TSE"]="Asian"
)

# Environment-specific configuration
declare -r ENVIRONMENT="${ENV:-production}"
declare -r CONFIG_DIR="${CONFIG_DIR:-/etc/fintech}"

# =============================================================================
# PARAMETER EXPANSION AND MANIPULATION
# =============================================================================

# Demonstrate various parameter expansion techniques
demonstrate_parameter_expansion() {
    log_message $LOG_LEVEL_INFO "Demonstrating parameter expansion techniques"
    
    local sample_string="  Hello World  "
    local empty_string=""
    local null_string=""
    
    # String length
    log_message $LOG_LEVEL_DEBUG "String length: ${#sample_string}"
    
    # Substring extraction
    log_message $LOG_LEVEL_DEBUG "Substring (0,5): ${sample_string:0:5}"
    
    # Pattern matching and replacement
    log_message $LOG_LEVEL_DEBUG "Replace 'World' with 'Fintech': ${sample_string//World/Fintech}"
    
    # Default value assignment
    local default_value="${empty_string:-default_value}"
    log_message $LOG_LEVEL_DEBUG "Default value assignment: $default_value"
    
    # Error if unset or empty
    local required_value="${null_string:?Error: This variable is required}"
    
    # Remove leading/trailing whitespace
    local trimmed="${sample_string## }"  # Remove leading spaces
    trimmed="${trimmed%% }"              # Remove trailing spaces
    log_message $LOG_LEVEL_DEBUG "Trimmed string: '$trimmed'"
}

# =============================================================================
# ARRAY OPERATIONS FOR FINANCIAL DATA
# =============================================================================

# Demonstrate array operations for financial data processing
demonstrate_array_operations() {
    log_message $LOG_LEVEL_INFO "Demonstrating array operations for financial data"
    
    # Indexed array operations
    log_message $LOG_LEVEL_DEBUG "Currency pairs: ${currency_pairs[*]}"
    log_message $LOG_LEVEL_DEBUG "Number of currency pairs: ${#currency_pairs[@]}"
    
    # Array iteration with proper indexing
    for i in "${!currency_pairs[@]}"; do
        log_message $LOG_LEVEL_DEBUG "Pair $((i+1)): ${currency_pairs[i]}"
    done
    
    # Associative array operations
    log_message $LOG_LEVEL_DEBUG "Market data keys: ${!market_data[*]}"
    log_message $LOG_LEVEL_DEBUG "Market data values: ${market_data[*]}"
    
    # Iterate through associative array
    for exchange in "${!market_data[@]}"; do
        log_message $LOG_LEVEL_DEBUG "Exchange: $exchange, Type: ${market_data[$exchange]}"
    done
}

# =============================================================================
# FINANCIAL CALCULATIONS AND VALIDATION
# =============================================================================

# Validate financial data input
validate_financial_input() {
    local amount="$1"
    local currency="$2"
    
    # Check if amount is numeric and positive
    if ! [[ "$amount" =~ ^[0-9]+\.?[0-9]*$ ]] || (( $(echo "$amount <= 0" | bc -l) )); then
        log_message $LOG_LEVEL_ERROR "Invalid amount: $amount"
        return 1
    fi
    
    # Validate currency format (3-letter code)
    if ! [[ "$currency" =~ ^[A-Z]{3}$ ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid currency format: $currency"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Financial input validation passed: $amount $currency"
    return 0
}

# Calculate exchange rate with proper error handling
calculate_exchange_rate() {
    local base_currency="$1"
    local target_currency="$2"
    local amount="$3"
    
    # Validate inputs
    if ! validate_financial_input "$amount" "$base_currency"; then
        return 1
    fi
    
    if ! validate_financial_input "1" "$target_currency"; then
        return 1
    fi
    
    # Simulate exchange rate calculation (in production, this would call an API)
    local rate=0.0
    case "${base_currency}_${target_currency}" in
        "USD_EUR") rate=0.85 ;;
        "EUR_USD") rate=1.18 ;;
        "GBP_USD") rate=1.25 ;;
        "USD_GBP") rate=0.80 ;;
        *) 
            log_message $LOG_LEVEL_ERROR "Unsupported currency pair: $base_currency/$target_currency"
            return 1
            ;;
    esac
    
    # Calculate converted amount with precision
    local converted_amount=$(echo "scale=2; $amount * $rate" | bc -l)
    
    log_message $LOG_LEVEL_INFO "Exchange calculation: $amount $base_currency = $converted_amount $target_currency (rate: $rate)"
    echo "$converted_amount"
}

# =============================================================================
# SECURE VARIABLE HANDLING FOR SENSITIVE DATA
# =============================================================================

# Secure handling of sensitive financial data
handle_sensitive_data() {
    log_message $LOG_LEVEL_INFO "Demonstrating secure variable handling"
    
    # Example of handling API keys (never log these in production)
    local api_key="${FINANCIAL_API_KEY:-}"
    if [[ -z "$api_key" ]]; then
        log_message $LOG_LEVEL_ERROR "Financial API key not set"
        return 1
    fi
    
    # Mask sensitive data in logs
    local masked_key="${api_key:0:4}****${api_key: -4}"
    log_message $LOG_LEVEL_DEBUG "Using API key: $masked_key"
    
    # Clear sensitive variables after use
    unset api_key
    
    # Use readonly for constants
    readonly -a SENSITIVE_FIELDS=("account_number" "routing_number" "ssn" "tax_id")
    
    log_message $LOG_LEVEL_INFO "Sensitive data handling completed"
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting bash fundamentals demonstration"
    log_message $LOG_LEVEL_INFO "Script: $SCRIPT_NAME v$SCRIPT_VERSION"
    log_message $LOG_LEVEL_INFO "Environment: $ENVIRONMENT"
    
    # Execute demonstration functions
    demonstrate_parameter_expansion
    demonstrate_array_operations
    
    # Test financial calculations
    if calculate_exchange_rate "USD" "EUR" "1000" >/dev/null; then
        log_message $LOG_LEVEL_INFO "Financial calculations working correctly"
    else
        log_message $LOG_LEVEL_ERROR "Financial calculations failed"
        exit 1
    fi
    
    # Test input validation
    if validate_financial_input "500.50" "USD"; then
        log_message $LOG_LEVEL_INFO "Input validation working correctly"
    else
        log_message $LOG_LEVEL_ERROR "Input validation failed"
        exit 1
    fi
    
    handle_sensitive_data
    
    log_message $LOG_LEVEL_INFO "Bash fundamentals demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

# Only run main function if script is executed directly (not sourced)
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    # Set up signal handlers for graceful shutdown
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    
    # Execute main function
    main "$@"
    
    # Exit with success code
    exit 0
fi
