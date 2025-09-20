#!/bin/bash
#
# Bash Fundamentals: Control Structures
# Production-Grade Script for Fintech Applications
#
# This script demonstrates control structures in bash with emphasis on
# financial calculations, risk management, and robust decision making.
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
# FINANCIAL DATA STRUCTURES
# =============================================================================

# Portfolio data structure
declare -A portfolio=(
    ["AAPL"]="150.25"
    ["GOOGL"]="2800.50"
    ["MSFT"]="300.75"
    ["TSLA"]="800.00"
)

# Risk thresholds for different asset classes
declare -A risk_thresholds=(
    ["conservative"]="0.05"
    ["moderate"]="0.10"
    ["aggressive"]="0.20"
)

# Market conditions
declare -r MARKET_OPEN="09:30"
declare -r MARKET_CLOSE="16:00"

# =============================================================================
# CONDITIONAL STATEMENTS FOR FINANCIAL LOGIC
# =============================================================================

# Check if market is open
is_market_open() {
    local current_time=$(date +%H:%M)
    local current_day=$(date +%u)  # 1=Monday, 7=Sunday
    
    # Market is closed on weekends
    if [[ $current_day -eq 6 || $current_day -eq 7 ]]; then
        log_message $LOG_LEVEL_INFO "Market is closed (weekend)"
        return 1
    fi
    
    # Check if current time is within market hours
    if [[ "$current_time" > "$MARKET_OPEN" && "$current_time" < "$MARKET_CLOSE" ]]; then
        log_message $LOG_LEVEL_INFO "Market is open"
        return 0
    else
        log_message $LOG_LEVEL_INFO "Market is closed"
        return 1
    fi
}

# Risk assessment based on portfolio volatility
assess_portfolio_risk() {
    local portfolio_value="$1"
    local risk_tolerance="$2"
    
    # Input validation
    if ! [[ "$portfolio_value" =~ ^[0-9]+\.?[0-9]*$ ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid portfolio value: $portfolio_value"
        return 1
    fi
    
    if ! [[ "$risk_tolerance" =~ ^(conservative|moderate|aggressive)$ ]]; then
        log_message $LOG_LEVEL_ERROR "Invalid risk tolerance: $risk_tolerance"
        return 1
    fi
    
    # Get risk threshold for the given tolerance
    local threshold="${risk_thresholds[$risk_tolerance]}"
    
    # Simulate volatility calculation (in production, this would be more complex)
    local volatility=0.0
    case "$risk_tolerance" in
        "conservative")
            volatility=0.03
            ;;
        "moderate")
            volatility=0.08
            ;;
        "aggressive")
            volatility=0.15
            ;;
    esac
    
    # Calculate risk metrics
    local value_at_risk=$(echo "scale=2; $portfolio_value * $volatility" | bc -l)
    local risk_percentage=$(echo "scale=2; $volatility * 100" | bc -l)
    
    log_message $LOG_LEVEL_INFO "Portfolio Risk Assessment:"
    log_message $LOG_LEVEL_INFO "  Portfolio Value: \$${portfolio_value}"
    log_message $LOG_LEVEL_INFO "  Risk Tolerance: $risk_tolerance"
    log_message $LOG_LEVEL_INFO "  Volatility: ${risk_percentage}%"
    log_message $LOG_LEVEL_INFO "  Value at Risk: \$${value_at_risk}"
    
    # Return risk level as exit code
    if (( $(echo "$volatility > $threshold" | bc -l) )); then
        return 2  # High risk
    elif (( $(echo "$volatility > $(echo "$threshold / 2" | bc -l)" | bc -l) )); then
        return 1  # Medium risk
    else
        return 0  # Low risk
    fi
}

# =============================================================================
# LOOP STRUCTURES FOR FINANCIAL PROCESSING
# =============================================================================

# Process portfolio positions
process_portfolio_positions() {
    log_message $LOG_LEVEL_INFO "Processing portfolio positions"
    
    local total_value=0
    local position_count=0
    
    # For loop with array iteration
    for symbol in "${!portfolio[@]}"; do
        local price="${portfolio[$symbol]}"
        local shares=100  # Simulated shares
        
        # Calculate position value
        local position_value=$(echo "scale=2; $shares * $price" | bc -l)
        total_value=$(echo "scale=2; $total_value + $position_value" | bc -l)
        
        log_message $LOG_LEVEL_DEBUG "Position: $symbol - $shares shares @ \$${price} = \$${position_value}"
        
        ((position_count++))
    done
    
    log_message $LOG_LEVEL_INFO "Portfolio Summary:"
    log_message $LOG_LEVEL_INFO "  Total Positions: $position_count"
    log_message $LOG_LEVEL_INFO "  Total Value: \$${total_value}"
    
    echo "$total_value"
}

# Process trading orders with while loop
process_trading_orders() {
    local orders_file="${1:-/tmp/trading_orders.txt}"
    
    # Create sample orders file if it doesn't exist
    if [[ ! -f "$orders_file" ]]; then
        cat > "$orders_file" << EOF
BUY,AAPL,100,150.25
SELL,GOOGL,50,2800.50
BUY,MSFT,200,300.75
SELL,TSLA,25,800.00
EOF
        log_message $LOG_LEVEL_DEBUG "Created sample orders file: $orders_file"
    fi
    
    log_message $LOG_LEVEL_INFO "Processing trading orders from: $orders_file"
    
    local order_count=0
    local total_buy_value=0
    local total_sell_value=0
    
    # Process orders line by line
    while IFS=',' read -r action symbol quantity price; do
        # Skip empty lines
        [[ -z "$action" ]] && continue
        
        # Validate order data
        if ! [[ "$action" =~ ^(BUY|SELL)$ ]]; then
            log_message $LOG_LEVEL_WARN "Invalid action in order: $action"
            continue
        fi
        
        if ! [[ "$quantity" =~ ^[0-9]+$ ]]; then
            log_message $LOG_LEVEL_WARN "Invalid quantity in order: $quantity"
            continue
        fi
        
        if ! [[ "$price" =~ ^[0-9]+\.?[0-9]*$ ]]; then
            log_message $LOG_LEVEL_WARN "Invalid price in order: $price"
            continue
        fi
        
        # Calculate order value
        local order_value=$(echo "scale=2; $quantity * $price" | bc -l)
        
        # Update totals based on action
        if [[ "$action" == "BUY" ]]; then
            total_buy_value=$(echo "scale=2; $total_buy_value + $order_value" | bc -l)
            log_message $LOG_LEVEL_DEBUG "BUY order: $symbol $quantity @ \$${price} = \$${order_value}"
        else
            total_sell_value=$(echo "scale=2; $total_sell_value + $order_value" | bc -l)
            log_message $LOG_LEVEL_DEBUG "SELL order: $symbol $quantity @ \$${price} = \$${order_value}"
        fi
        
        ((order_count++))
        
    done < "$orders_file"
    
    log_message $LOG_LEVEL_INFO "Trading Orders Summary:"
    log_message $LOG_LEVEL_INFO "  Total Orders: $order_count"
    log_message $LOG_LEVEL_INFO "  Total BUY Value: \$${total_buy_value}"
    log_message $LOG_LEVEL_INFO "  Total SELL Value: \$${total_sell_value}"
    
    # Calculate net position
    local net_value=$(echo "scale=2; $total_sell_value - $total_buy_value" | bc -l)
    log_message $LOG_LEVEL_INFO "  Net Position: \$${net_value}"
}

# =============================================================================
# CASE STATEMENTS FOR COMPLEX DECISION MAKING
# =============================================================================

# Process different types of financial instruments
process_financial_instrument() {
    local instrument_type="$1"
    local symbol="$2"
    local quantity="$3"
    local price="$4"
    
    case "$instrument_type" in
        "STOCK")
            log_message $LOG_LEVEL_INFO "Processing stock trade: $symbol"
            # Stock-specific processing
            local commission=9.99
            local total_cost=$(echo "scale=2; ($quantity * $price) + $commission" | bc -l)
            log_message $LOG_LEVEL_DEBUG "Stock trade cost: \$${total_cost} (including \$${commission} commission)"
            ;;
        "OPTION")
            log_message $LOG_LEVEL_INFO "Processing option trade: $symbol"
            # Option-specific processing
            local option_fee=0.65
            local total_cost=$(echo "scale=2; ($quantity * $price) + ($quantity * $option_fee)" | bc -l)
            log_message $LOG_LEVEL_DEBUG "Option trade cost: \$${total_cost} (including \$${option_fee} per contract fee)"
            ;;
        "BOND")
            log_message $LOG_LEVEL_INFO "Processing bond trade: $symbol"
            # Bond-specific processing
            local bond_fee=1.00
            local total_cost=$(echo "scale=2; ($quantity * $price) + $bond_fee" | bc -l)
            log_message $LOG_LEVEL_DEBUG "Bond trade cost: \$${total_cost} (including \$${bond_fee} bond fee)"
            ;;
        "ETF")
            log_message $LOG_LEVEL_INFO "Processing ETF trade: $symbol"
            # ETF-specific processing
            local etf_fee=0.50
            local total_cost=$(echo "scale=2; ($quantity * $price) + $etf_fee" | bc -l)
            log_message $LOG_LEVEL_DEBUG "ETF trade cost: \$${total_cost} (including \$${etf_fee} ETF fee)"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown instrument type: $instrument_type"
            return 1
            ;;
    esac
}

# Market data processing based on exchange
process_market_data() {
    local exchange="$1"
    local data_file="$2"
    
    case "$exchange" in
        "NASDAQ"|"NYSE")
            log_message $LOG_LEVEL_INFO "Processing US market data from $exchange"
            # US market specific processing
            process_us_market_data "$data_file"
            ;;
        "LSE")
            log_message $LOG_LEVEL_INFO "Processing London market data"
            # London market specific processing
            process_london_market_data "$data_file"
            ;;
        "TSE")
            log_message $LOG_LEVEL_INFO "Processing Tokyo market data"
            # Tokyo market specific processing
            process_tokyo_market_data "$data_file"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unsupported exchange: $exchange"
            return 1
            ;;
    esac
}

# Placeholder functions for market data processing
process_us_market_data() {
    local data_file="$1"
    log_message $LOG_LEVEL_DEBUG "Processing US market data from: $data_file"
    # Implementation would parse US market data format
}

process_london_market_data() {
    local data_file="$1"
    log_message $LOG_LEVEL_DEBUG "Processing London market data from: $data_file"
    # Implementation would parse London market data format
}

process_tokyo_market_data() {
    local data_file="$1"
    log_message $LOG_LEVEL_DEBUG "Processing Tokyo market data from: $data_file"
    # Implementation would parse Tokyo market data format
}

# =============================================================================
# NESTED CONTROL STRUCTURES FOR COMPLEX LOGIC
# =============================================================================

# Portfolio rebalancing logic
rebalance_portfolio() {
    local target_allocation="$1"
    local current_portfolio_value="$2"
    
    log_message $LOG_LEVEL_INFO "Starting portfolio rebalancing"
    
    # Parse target allocation (format: "AAPL:30,GOOGL:40,MSFT:30")
    IFS=',' read -ra allocations <<< "$target_allocation"
    
    for allocation in "${allocations[@]}"; do
        IFS=':' read -r symbol target_percentage <<< "$allocation"
        
        # Calculate target value for this symbol
        local target_value=$(echo "scale=2; $current_portfolio_value * $target_percentage / 100" | bc -l)
        
        # Get current value for this symbol
        local current_price="${portfolio[$symbol]:-0}"
        local current_shares=100  # Simulated
        local current_value=$(echo "scale=2; $current_shares * $current_price" | bc -l)
        
        # Calculate difference
        local difference=$(echo "scale=2; $target_value - $current_value" | bc -l)
        
        # Determine action based on difference
        if (( $(echo "$difference > 100" | bc -l) )); then
            # Need to buy more
            local shares_to_buy=$(echo "scale=0; $difference / $current_price" | bc -l)
            log_message $LOG_LEVEL_INFO "BUY $shares_to_buy shares of $symbol (target: \$${target_value}, current: \$${current_value})"
        elif (( $(echo "$difference < -100" | bc -l) )); then
            # Need to sell some
            local shares_to_sell=$(echo "scale=0; -$difference / $current_price" | bc -l)
            log_message $LOG_LEVEL_INFO "SELL $shares_to_sell shares of $symbol (target: \$${target_value}, current: \$${current_value})"
        else
            log_message $LOG_LEVEL_DEBUG "$symbol is within rebalancing threshold (difference: \$${difference})"
        fi
    done
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting control structures demonstration"
    
    # Test market hours
    if is_market_open; then
        log_message $LOG_LEVEL_INFO "Market is currently open - proceeding with trades"
    else
        log_message $LOG_LEVEL_WARN "Market is closed - trades will be queued"
    fi
    
    # Process portfolio
    local portfolio_value
    portfolio_value=$(process_portfolio_positions)
    
    # Assess risk
    local risk_tolerance="moderate"
    assess_portfolio_risk "$portfolio_value" "$risk_tolerance"
    local risk_level=$?
    
    case $risk_level in
        0) log_message $LOG_LEVEL_INFO "Portfolio risk level: LOW" ;;
        1) log_message $LOG_LEVEL_WARN "Portfolio risk level: MEDIUM" ;;
        2) log_message $LOG_LEVEL_ERROR "Portfolio risk level: HIGH" ;;
    esac
    
    # Process trading orders
    process_trading_orders
    
    # Test financial instrument processing
    process_financial_instrument "STOCK" "AAPL" "100" "150.25"
    process_financial_instrument "OPTION" "AAPL" "10" "5.50"
    
    # Test market data processing
    process_market_data "NASDAQ" "/tmp/nasdaq_data.csv"
    
    # Test portfolio rebalancing
    rebalance_portfolio "AAPL:30,GOOGL:40,MSFT:30" "$portfolio_value"
    
    log_message $LOG_LEVEL_INFO "Control structures demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    main "$@"
    exit 0
fi
