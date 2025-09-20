#!/bin/bash
#
# Bash Fundamentals: Functions and Modularity
# Production-Grade Script for Fintech Applications
#
# This script demonstrates function definition, scope, return values,
# and modular programming techniques essential for financial applications.
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
# FUNCTION DEFINITION AND SCOPE
# =============================================================================

# Basic function definition
calculate_simple_interest() {
    local principal="$1"
    local rate="$2"
    local time="$3"
    
    # Input validation
    if ! [[ "$principal" =~ ^[0-9]+\.?[0-9]*$ ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid principal amount: $principal"
        return 1
    fi
    
    if ! [[ "$rate" =~ ^[0-9]+\.?[0-9]*$ ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid interest rate: $rate"
        return 1
    fi
    
    if ! [[ "$time" =~ ^[0-9]+\.?[0-9]*$ ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid time period: $time"
        return 1
    fi
    
    # Calculate simple interest
    local interest=$(echo "scale=2; $principal * $rate * $time" | bc -l)
    
    log_message $LOG_LEVEL_DEBUG "Simple interest calculation: $principal * $rate * $time = $interest"
    echo "$interest"
}

# Function with return value using exit codes
validate_financial_data() {
    local data_type="$1"
    local value="$2"
    
    case "$data_type" in
        "amount")
            if [[ "$value" =~ ^[0-9]+\.?[0-9]*$ ]] && (( $(echo "$value > 0" | bc -l) )); then
                log_message $LOG_LEVEL_DEBUG "Valid amount: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid amount: $value"
                return 1
            fi
            ;;
        "currency")
            if [[ "$value" =~ ^[A-Z]{3}$ ]]; then
                log_message $LOG_LEVEL_DEBUG "Valid currency: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid currency: $value"
                return 1
            fi
            ;;
        "date")
            if date -d "$value" >/dev/null 2>&1; then
                log_message $LOG_LEVEL_DEBUG "Valid date: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid date: $value"
                return 1
            fi
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown data type: $data_type"
            return 1
            ;;
    esac
}

# Function with local and global variables
process_portfolio_data() {
    local portfolio_file="$1"
    local -a symbols=()
    local -a prices=()
    local -a quantities=()
    
    # Local variables (function scope)
    local total_value=0
    local position_count=0
    
    log_message $LOG_LEVEL_INFO "Processing portfolio data from: $portfolio_file"
    
    # Read portfolio data
    while IFS=',' read -r symbol price quantity; do
        # Skip header line
        [[ "$symbol" == "symbol" ]] && continue
        
        # Validate data
        if ! validate_financial_data "amount" "$price"; then
            log_message $LOG_LEVEL_WARN "Skipping invalid price for $symbol: $price"
            continue
        fi
        
        if ! validate_financial_data "amount" "$quantity"; then
            log_message $LOG_LEVEL_WARN "Skipping invalid quantity for $symbol: $quantity"
            continue
        fi
        
        # Store data in arrays
        symbols+=("$symbol")
        prices+=("$price")
        quantities+=("$quantity")
        
        # Calculate position value
        local position_value=$(echo "scale=2; $price * $quantity" | bc -l)
        total_value=$(echo "scale=2; $total_value + $position_value" | bc -l)
        
        ((position_count++))
        log_message $LOG_LEVEL_DEBUG "Position $position_count: $symbol - $quantity @ $price = $position_value"
        
    done < "$portfolio_file"
    
    # Update global variables
    PORTFOLIO_SYMBOLS=("${symbols[@]}")
    PORTFOLIO_PRICES=("${prices[@]}")
    PORTFOLIO_QUANTITIES=("${quantities[@]}")
    PORTFOLIO_TOTAL_VALUE="$total_value"
    PORTFOLIO_POSITION_COUNT="$position_count"
    
    log_message $LOG_LEVEL_INFO "Portfolio processing completed: $position_count positions, total value: $total_value"
}

# =============================================================================
# FUNCTION PARAMETERS AND ARGUMENTS
# =============================================================================

# Function with multiple parameters
calculate_portfolio_metrics() {
    local -r portfolio_value="$1"
    local -r risk_free_rate="$2"
    local -r market_return="$3"
    local -r portfolio_return="$4"
    
    # Calculate Sharpe ratio
    local excess_return=$(echo "scale=4; $portfolio_return - $risk_free_rate" | bc -l)
    local portfolio_volatility=0.15  # Simplified - would be calculated from historical data
    local sharpe_ratio=$(echo "scale=4; $excess_return / $portfolio_volatility" | bc -l)
    
    # Calculate portfolio beta (simplified)
    local portfolio_beta=1.2  # Simplified - would be calculated from historical data
    
    # Calculate alpha
    local expected_return=$(echo "scale=4; $risk_free_rate + $portfolio_beta * ($market_return - $risk_free_rate)" | bc -l)
    local alpha=$(echo "scale=4; $portfolio_return - $expected_return" | bc -l)
    
    # Return metrics as JSON-like string
    cat << EOF
{
    "portfolio_value": $portfolio_value,
    "sharpe_ratio": $sharpe_ratio,
    "portfolio_beta": $portfolio_beta,
    "alpha": $alpha,
    "excess_return": $excess_return,
    "expected_return": $expected_return
}
EOF
}

# Function with variable number of arguments
process_multiple_symbols() {
    local -a symbols=("$@")
    local processed_count=0
    local error_count=0
    
    log_message $LOG_LEVEL_INFO "Processing ${#symbols[@]} symbols"
    
    for symbol in "${symbols[@]}"; do
        if [[ ${#symbol} -ge 1 && ${#symbol} -le 5 ]]; then
            log_message $LOG_LEVEL_DEBUG "Processing symbol: $symbol"
            ((processed_count++))
        else
            log_message $LOG_LEVEL_WARN "Invalid symbol format: $symbol"
            ((error_count++))
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Symbol processing completed: $processed_count processed, $error_count errors"
    echo "$processed_count,$error_count"
}

# =============================================================================
# FUNCTION RETURN VALUES AND OUTPUT
# =============================================================================

# Function that returns data via stdout
get_market_data() {
    local symbol="$1"
    local data_type="${2:-price}"
    
    # Simulate market data retrieval
    case "$symbol" in
        "AAPL")
            case "$data_type" in
                "price") echo "150.25" ;;
                "volume") echo "1000000" ;;
                "change") echo "2.50" ;;
                *) echo "0.00" ;;
            esac
            ;;
        "GOOGL")
            case "$data_type" in
                "price") echo "2800.50" ;;
                "volume") echo "500000" ;;
                "change") echo "-15.75" ;;
                *) echo "0.00" ;;
            esac
            ;;
        "MSFT")
            case "$data_type" in
                "price") echo "300.75" ;;
                "volume") echo "750000" ;;
                "change") echo "5.25" ;;
                *) echo "0.00" ;;
            esac
            ;;
        *)
            log_message $LOG_LEVEL_WARN "Unknown symbol: $symbol"
            echo "0.00"
            ;;
    esac
}

# Function that returns data via global variables
analyze_portfolio_risk() {
    local -r portfolio_value="$1"
    local -r risk_tolerance="$2"
    
    # Calculate risk metrics
    local var_95=$(echo "scale=2; $portfolio_value * 0.05" | bc -l)
    local max_drawdown=$(echo "scale=2; $portfolio_value * 0.15" | bc -l)
    local concentration_risk=0.0
    
    # Calculate concentration risk (simplified)
    if [[ ${#PORTFOLIO_SYMBOLS[@]} -gt 0 ]]; then
        local max_position=0
        for i in "${!PORTFOLIO_SYMBOLS[@]}"; do
            local position_value=$(echo "scale=2; ${PORTFOLIO_PRICES[i]} * ${PORTFOLIO_QUANTITIES[i]}" | bc -l)
            if (( $(echo "$position_value > $max_position" | bc -l) )); then
                max_position="$position_value"
            fi
        done
        concentration_risk=$(echo "scale=4; $max_position / $portfolio_value" | bc -l)
    fi
    
    # Set global risk variables
    PORTFOLIO_VAR_95="$var_95"
    PORTFOLIO_MAX_DRAWDOWN="$max_drawdown"
    PORTFOLIO_CONCENTRATION_RISK="$concentration_risk"
    
    # Determine risk level
    local risk_level="low"
    if (( $(echo "$concentration_risk > 0.3" | bc -l) )); then
        risk_level="high"
    elif (( $(echo "$concentration_risk > 0.1" | bc -l) )); then
        risk_level="medium"
    fi
    
    PORTFOLIO_RISK_LEVEL="$risk_level"
    
    log_message $LOG_LEVEL_INFO "Portfolio risk analysis: $risk_level risk level, VaR: $var_95, Concentration: $concentration_risk"
}

# =============================================================================
# RECURSIVE FUNCTIONS
# =============================================================================

# Recursive function for compound interest calculation
calculate_compound_interest_recursive() {
    local principal="$1"
    local rate="$2"
    local time="$3"
    local current_time="${4:-0}"
    
    # Base case
    if (( $(echo "$current_time >= $time" | bc -l) )); then
        echo "$principal"
        return 0
    fi
    
    # Recursive case
    local new_principal=$(echo "scale=2; $principal * (1 + $rate)" | bc -l)
    local next_time=$(echo "scale=0; $current_time + 1" | bc -l)
    
    calculate_compound_interest_recursive "$new_principal" "$rate" "$time" "$next_time"
}

# Recursive function for Fibonacci sequence (financial modeling)
calculate_fibonacci() {
    local n="$1"
    
    # Base cases
    if [[ $n -eq 0 ]]; then
        echo "0"
        return 0
    elif [[ $n -eq 1 ]]; then
        echo "1"
        return 0
    fi
    
    # Recursive case
    local fib_n_minus_1=$(calculate_fibonacci $((n-1)))
    local fib_n_minus_2=$(calculate_fibonacci $((n-2)))
    local fib_n=$(echo "scale=0; $fib_n_minus_1 + $fib_n_minus_2" | bc -l)
    
    echo "$fib_n"
}

# =============================================================================
# FUNCTION LIBRARIES AND MODULARITY
# =============================================================================

# Financial calculation library
financial_calculations() {
    local operation="$1"
    shift
    
    case "$operation" in
        "present_value")
            local future_value="$1"
            local rate="$2"
            local time="$3"
            local pv=$(echo "scale=2; $future_value / (1 + $rate) ^ $time" | bc -l)
            echo "$pv"
            ;;
        "future_value")
            local present_value="$1"
            local rate="$2"
            local time="$3"
            local fv=$(echo "scale=2; $present_value * (1 + $rate) ^ $time" | bc -l)
            echo "$fv"
            ;;
        "annuity_payment")
            local principal="$1"
            local rate="$2"
            local time="$3"
            local payment=$(echo "scale=2; $principal * $rate / (1 - (1 + $rate) ^ -$time)" | bc -l)
            echo "$payment"
            ;;
        "bond_price")
            local face_value="$1"
            local coupon_rate="$2"
            local market_rate="$3"
            local time_to_maturity="$4"
            local coupon_payment=$(echo "scale=2; $face_value * $coupon_rate" | bc -l)
            local bond_price=$(echo "scale=2; $coupon_payment * (1 - (1 + $market_rate) ^ -$time_to_maturity) / $market_rate + $face_value / (1 + $market_rate) ^ $time_to_maturity" | bc -l)
            echo "$bond_price"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown financial calculation: $operation"
            return 1
            ;;
    esac
}

# Data validation library
data_validation() {
    local validation_type="$1"
    local value="$2"
    
    case "$validation_type" in
        "email")
            if [[ "$value" =~ ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$ ]]; then
                return 0
            else
                return 1
            fi
            ;;
        "phone")
            if [[ "$value" =~ ^\+?[1-9]\d{1,14}$ ]]; then
                return 0
            else
                return 1
            fi
            ;;
        "ssn")
            if [[ "$value" =~ ^[0-9]{3}-[0-9]{2}-[0-9]{4}$ ]]; then
                return 0
            else
                return 1
            fi
            ;;
        "account_number")
            if [[ "$value" =~ ^[0-9]{8,12}$ ]]; then
                return 0
            else
                return 1
            fi
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown validation type: $validation_type"
            return 1
            ;;
    esac
}

# =============================================================================
# FUNCTION TESTING AND DEBUGGING
# =============================================================================

# Test function for financial calculations
test_financial_calculations() {
    log_message $LOG_LEVEL_INFO "Testing financial calculations"
    
    # Test simple interest
    local simple_interest=$(calculate_simple_interest 1000 0.05 2)
    local expected_simple=100.00
    if [[ "$simple_interest" == "$expected_simple" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: Simple interest calculation"
    else
        log_message $LOG_LEVEL_ERROR "FAIL: Simple interest calculation - Expected: $expected_simple, Got: $simple_interest"
    fi
    
    # Test compound interest (recursive)
    local compound_interest=$(calculate_compound_interest_recursive 1000 0.05 2)
    local expected_compound=1102.50
    if [[ "$compound_interest" == "$expected_compound" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: Compound interest calculation"
    else
        log_message $LOG_LEVEL_ERROR "FAIL: Compound interest calculation - Expected: $expected_compound, Got: $compound_interest"
    fi
    
    # Test present value
    local present_value=$(financial_calculations "present_value" 1100 0.05 1)
    local expected_pv=1047.62
    if [[ "$present_value" == "$expected_pv" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: Present value calculation"
    else
        log_message $LOG_LEVEL_ERROR "FAIL: Present value calculation - Expected: $expected_pv, Got: $present_value"
    fi
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting functions and modularity demonstration"
    
    # Test basic functions
    log_message $LOG_LEVEL_INFO "Testing basic financial calculations"
    local interest=$(calculate_simple_interest 1000 0.05 2)
    log_message $LOG_LEVEL_INFO "Simple interest for $1000 at 5% for 2 years: $interest"
    
    # Test data validation
    log_message $LOG_LEVEL_INFO "Testing data validation"
    if validate_financial_data "amount" "1500.50"; then
        log_message $LOG_LEVEL_INFO "Amount validation passed"
    else
        log_message $LOG_LEVEL_ERROR "Amount validation failed"
    fi
    
    # Test portfolio processing
    log_message $LOG_LEVEL_INFO "Testing portfolio processing"
    local portfolio_file="/tmp/test_portfolio.csv"
    cat > "$portfolio_file" << EOF
symbol,price,quantity
AAPL,150.25,100
GOOGL,2800.50,50
MSFT,300.75,200
EOF
    
    process_portfolio_data "$portfolio_file"
    log_message $LOG_LEVEL_INFO "Portfolio total value: $PORTFOLIO_TOTAL_VALUE"
    log_message $LOG_LEVEL_INFO "Portfolio position count: $PORTFOLIO_POSITION_COUNT"
    
    # Test portfolio risk analysis
    analyze_portfolio_risk "$PORTFOLIO_TOTAL_VALUE" "moderate"
    log_message $LOG_LEVEL_INFO "Portfolio risk level: $PORTFOLIO_RISK_LEVEL"
    log_message $LOG_LEVEL_INFO "Portfolio VaR (95%): $PORTFOLIO_VAR_95"
    
    # Test multiple symbol processing
    local results=$(process_multiple_symbols "AAPL" "GOOGL" "MSFT" "INVALID" "TSLA")
    log_message $LOG_LEVEL_INFO "Symbol processing results: $results"
    
    # Test market data retrieval
    local aapl_price=$(get_market_data "AAPL" "price")
    log_message $LOG_LEVEL_INFO "AAPL current price: $aapl_price"
    
    # Test recursive functions
    local fib_10=$(calculate_fibonacci 10)
    log_message $LOG_LEVEL_INFO "Fibonacci(10): $fib_10"
    
    # Test financial calculations library
    local bond_price=$(financial_calculations "bond_price" 1000 0.05 0.04 10)
    log_message $LOG_LEVEL_INFO "Bond price: $bond_price"
    
    # Test data validation library
    if data_validation "email" "user@company.com"; then
        log_message $LOG_LEVEL_INFO "Email validation passed"
    else
        log_message $LOG_LEVEL_ERROR "Email validation failed"
    fi
    
    # Run function tests
    test_financial_calculations
    
    # Cleanup
    rm -f "$portfolio_file"
    
    log_message $LOG_LEVEL_INFO "Functions and modularity demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    main "$@"
    exit 0
fi
