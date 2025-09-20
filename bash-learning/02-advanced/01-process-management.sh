#!/bin/bash
#
# Advanced Bash: Process Management and Job Control
# Production-Grade Script for Fintech Applications
#
# This script demonstrates advanced process management techniques
# essential for high-frequency trading systems and financial data processing.
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
# LOGGING AND MONITORING CONFIGURATION
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
    local pid=$$
    
    if [[ "$level" -le "$CURRENT_LOG_LEVEL" ]]; then
        case "$level" in
            "$LOG_LEVEL_ERROR") echo "[ERROR] $timestamp [PID:$pid]: $message" >&2 ;;
            "$LOG_LEVEL_WARN")  echo "[WARN]  $timestamp [PID:$pid]: $message" >&2 ;;
            "$LOG_LEVEL_INFO")  echo "[INFO]  $timestamp [PID:$pid]: $message" >&2 ;;
            "$LOG_LEVEL_DEBUG") echo "[DEBUG] $timestamp [PID:$pid]: $message" >&2 ;;
        esac
    fi
}

# =============================================================================
# PROCESS MANAGEMENT CONFIGURATION
# =============================================================================

# Process pool configuration for high-frequency trading
readonly MAX_CONCURRENT_PROCESSES=10
readonly PROCESS_TIMEOUT=30
readonly PROCESS_RETRY_COUNT=3

# Trading system process types
declare -A PROCESS_TYPES=(
    ["market_data"]="Market Data Processor"
    ["order_execution"]="Order Execution Engine"
    ["risk_management"]="Risk Management System"
    ["portfolio_management"]="Portfolio Management"
    ["compliance"]="Compliance Monitor"
)

# Process status tracking
declare -A PROCESS_STATUS=()
declare -A PROCESS_PIDS=()
declare -A PROCESS_START_TIME=()

# =============================================================================
# SIGNAL HANDLING AND GRACEFUL SHUTDOWN
# =============================================================================

# Signal handler for graceful shutdown
cleanup_and_exit() {
    local signal="$1"
    log_message $LOG_LEVEL_INFO "Received signal $signal - initiating graceful shutdown"
    
    # Stop all child processes
    stop_all_processes
    
    # Cleanup temporary files
    cleanup_temp_files
    
    # Exit with appropriate code
    case "$signal" in
        "TERM"|"INT") exit 130 ;;
        "QUIT") exit 131 ;;
        *) exit 1 ;;
    esac
}

# Register signal handlers
trap 'cleanup_and_exit TERM' TERM
trap 'cleanup_and_exit INT' INT
trap 'cleanup_and_exit QUIT' QUIT

# =============================================================================
# PROCESS POOL MANAGEMENT
# =============================================================================

# Initialize process pool
init_process_pool() {
    log_message $LOG_LEVEL_INFO "Initializing process pool for trading system"
    
    # Create named pipes for inter-process communication
    local pipe_dir="/tmp/trading_system_$$"
    mkdir -p "$pipe_dir"
    
    # Create pipes for each process type
    for process_type in "${!PROCESS_TYPES[@]}"; do
        local pipe_file="$pipe_dir/${process_type}.pipe"
        mkfifo "$pipe_file"
        log_message $LOG_LEVEL_DEBUG "Created pipe: $pipe_file"
    done
    
    echo "$pipe_dir"
}

# Start a trading system process
start_trading_process() {
    local process_type="$1"
    local process_name="$2"
    local pipe_dir="$3"
    
    log_message $LOG_LEVEL_INFO "Starting $process_name (type: $process_type)"
    
    # Check if we've reached the process limit
    local current_process_count=$(jobs -r | wc -l)
    if [[ $current_process_count -ge $MAX_CONCURRENT_PROCESSES ]]; then
        log_message $LOG_LEVEL_WARN "Process limit reached ($MAX_CONCURRENT_PROCESSES), queuing $process_name"
        return 1
    fi
    
    # Start the process in background
    (
        # Set up process-specific environment
        export PROCESS_TYPE="$process_type"
        export PROCESS_NAME="$process_name"
        export PIPE_DIR="$pipe_dir"
        
        # Execute the trading process
        execute_trading_process "$process_type" "$process_name" "$pipe_dir"
    ) &
    
    local pid=$!
    PROCESS_PIDS["$process_name"]=$pid
    PROCESS_STATUS["$process_name"]="running"
    PROCESS_START_TIME["$process_name"]=$(date +%s)
    
    log_message $LOG_LEVEL_INFO "Started $process_name with PID: $pid"
    return 0
}

# Execute a specific trading process
execute_trading_process() {
    local process_type="$1"
    local process_name="$2"
    local pipe_dir="$3"
    
    # Set up process-specific signal handling
    trap 'log_message $LOG_LEVEL_INFO "$process_name received termination signal"; exit 0' TERM INT
    
    log_message $LOG_LEVEL_DEBUG "$process_name starting execution"
    
    # Process-specific logic based on type
    case "$process_type" in
        "market_data")
            process_market_data "$pipe_dir"
            ;;
        "order_execution")
            process_order_execution "$pipe_dir"
            ;;
        "risk_management")
            process_risk_management "$pipe_dir"
            ;;
        "portfolio_management")
            process_portfolio_management "$pipe_dir"
            ;;
        "compliance")
            process_compliance_monitoring "$pipe_dir"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown process type: $process_type"
            exit 1
            ;;
    esac
}

# =============================================================================
# TRADING SYSTEM PROCESS IMPLEMENTATIONS
# =============================================================================

# Market data processing
process_market_data() {
    local pipe_dir="$1"
    local pipe_file="$pipe_dir/market_data.pipe"
    
    log_message $LOG_LEVEL_INFO "Market data processor started"
    
    # Simulate market data processing
    local tick_count=0
    while true; do
        # Generate simulated market data
        local timestamp=$(date +%s.%3N)
        local symbol="AAPL"
        local price=$(echo "scale=2; 150 + (RANDOM % 10) - 5" | bc -l)
        local volume=$((RANDOM % 10000 + 1000))
        
        # Write to pipe
        echo "$timestamp,$symbol,$price,$volume" > "$pipe_file" &
        
        ((tick_count++))
        log_message $LOG_LEVEL_DEBUG "Processed market data tick $tick_count: $symbol @ \$${price}"
        
        # Simulate processing delay
        sleep 0.1
        
        # Check for termination signal
        if ! kill -0 $$ 2>/dev/null; then
            break
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Market data processor stopped after $tick_count ticks"
}

# Order execution processing
process_order_execution() {
    local pipe_dir="$1"
    local pipe_file="$pipe_dir/order_execution.pipe"
    
    log_message $LOG_LEVEL_INFO "Order execution engine started"
    
    local order_count=0
    while true; do
        # Read orders from pipe (simplified)
        if [[ -p "$pipe_file" ]]; then
            local order_data
            if read -t 1 order_data < "$pipe_file" 2>/dev/null; then
                # Process order
                IFS=',' read -r order_id symbol action quantity price <<< "$order_data"
                
                # Simulate order execution
                local execution_price=$(echo "scale=2; $price + (RANDOM % 2 - 1) * 0.01" | bc -l)
                local execution_time=$(date +%s.%3N)
                
                log_message $LOG_LEVEL_DEBUG "Executed order $order_id: $action $quantity $symbol @ \$${execution_price}"
                ((order_count++))
            fi
        fi
        
        # Check for termination signal
        if ! kill -0 $$ 2>/dev/null; then
            break
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Order execution engine stopped after $order_count orders"
}

# Risk management processing
process_risk_management() {
    local pipe_dir="$1"
    local pipe_file="$pipe_dir/risk_management.pipe"
    
    log_message $LOG_LEVEL_INFO "Risk management system started"
    
    local risk_checks=0
    while true; do
        # Perform risk checks
        local portfolio_value=1000000  # Simulated portfolio value
        local risk_limit=100000       # Risk limit
        
        # Calculate current risk (simplified)
        local current_risk=$((RANDOM % 200000))
        
        if [[ $current_risk -gt $risk_limit ]]; then
            log_message $LOG_LEVEL_WARN "Risk limit exceeded: $current_risk > $risk_limit"
            # In production, this would trigger risk management actions
        fi
        
        ((risk_checks++))
        log_message $LOG_LEVEL_DEBUG "Risk check $risk_checks: current risk = $current_risk"
        
        sleep 1
        
        # Check for termination signal
        if ! kill -0 $$ 2>/dev/null; then
            break
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Risk management system stopped after $risk_checks checks"
}

# Portfolio management processing
process_portfolio_management() {
    local pipe_dir="$1"
    local pipe_file="$pipe_dir/portfolio_management.pipe"
    
    log_message $LOG_LEVEL_INFO "Portfolio management started"
    
    local rebalance_count=0
    while true; do
        # Simulate portfolio rebalancing
        local rebalance_threshold=0.05  # 5% threshold
        
        # Check if rebalancing is needed (simplified logic)
        if [[ $((RANDOM % 100)) -lt 10 ]]; then  # 10% chance of rebalancing
            log_message $LOG_LEVEL_INFO "Portfolio rebalancing triggered"
            ((rebalance_count++))
        fi
        
        sleep 5
        
        # Check for termination signal
        if ! kill -0 $$ 2>/dev/null; then
            break
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Portfolio management stopped after $rebalance_count rebalances"
}

# Compliance monitoring processing
process_compliance_monitoring() {
    local pipe_dir="$1"
    local pipe_file="$pipe_dir/compliance.pipe"
    
    log_message $LOG_LEVEL_INFO "Compliance monitoring started"
    
    local compliance_checks=0
    while true; do
        # Perform compliance checks
        local position_limit=1000000
        local current_position=$((RANDOM % 2000000))
        
        if [[ $current_position -gt $position_limit ]]; then
            log_message $LOG_LEVEL_WARN "Position limit exceeded: $current_position > $position_limit"
        fi
        
        ((compliance_checks++))
        log_message $LOG_LEVEL_DEBUG "Compliance check $compliance_checks: position = $current_position"
        
        sleep 2
        
        # Check for termination signal
        if ! kill -0 $$ 2>/dev/null; then
            break
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Compliance monitoring stopped after $compliance_checks checks"
}

# =============================================================================
# PROCESS MONITORING AND HEALTH CHECKS
# =============================================================================

# Monitor process health
monitor_process_health() {
    log_message $LOG_LEVEL_INFO "Starting process health monitoring"
    
    while true; do
        for process_name in "${!PROCESS_PIDS[@]}"; do
            local pid="${PROCESS_PIDS[$process_name]}"
            local status="${PROCESS_STATUS[$process_name]}"
            local start_time="${PROCESS_START_TIME[$process_name]}"
            
            # Check if process is still running
            if ! kill -0 "$pid" 2>/dev/null; then
                log_message $LOG_LEVEL_ERROR "Process $process_name (PID: $pid) has died"
                PROCESS_STATUS["$process_name"]="dead"
                
                # Restart process if needed
                if [[ "$status" == "running" ]]; then
                    log_message $LOG_LEVEL_INFO "Attempting to restart $process_name"
                    # In production, this would restart the process
                fi
            else
                # Check process runtime
                local current_time=$(date +%s)
                local runtime=$((current_time - start_time))
                
                if [[ $runtime -gt $PROCESS_TIMEOUT ]]; then
                    log_message $LOG_LEVEL_WARN "Process $process_name has been running for $runtime seconds"
                fi
            fi
        done
        
        sleep 5
    done
}

# Get process statistics
get_process_statistics() {
    log_message $LOG_LEVEL_INFO "Process Statistics:"
    
    for process_name in "${!PROCESS_PIDS[@]}"; do
        local pid="${PROCESS_PIDS[$process_name]}"
        local status="${PROCESS_STATUS[$process_name]}"
        local start_time="${PROCESS_START_TIME[$process_name]}"
        
        if [[ "$status" == "running" && -n "$pid" ]]; then
            local current_time=$(date +%s)
            local runtime=$((current_time - start_time))
            
            # Get memory usage
            local memory_usage="N/A"
            if command -v ps >/dev/null 2>&1; then
                memory_usage=$(ps -o rss= -p "$pid" 2>/dev/null || echo "N/A")
            fi
            
            log_message $LOG_LEVEL_INFO "  $process_name: PID=$pid, Status=$status, Runtime=${runtime}s, Memory=${memory_usage}KB"
        else
            log_message $LOG_LEVEL_INFO "  $process_name: Status=$status"
        fi
    done
}

# =============================================================================
# PROCESS LIFECYCLE MANAGEMENT
# =============================================================================

# Stop all processes
stop_all_processes() {
    log_message $LOG_LEVEL_INFO "Stopping all trading system processes"
    
    for process_name in "${!PROCESS_PIDS[@]}"; do
        local pid="${PROCESS_PIDS[$process_name]}"
        
        if [[ -n "$pid" && "$pid" -gt 0 ]]; then
            log_message $LOG_LEVEL_INFO "Stopping $process_name (PID: $pid)"
            
            # Send TERM signal first
            if kill -TERM "$pid" 2>/dev/null; then
                # Wait for graceful shutdown
                local count=0
                while kill -0 "$pid" 2>/dev/null && [[ $count -lt 10 ]]; do
                    sleep 1
                    ((count++))
                done
                
                # Force kill if still running
                if kill -0 "$pid" 2>/dev/null; then
                    log_message $LOG_LEVEL_WARN "Force killing $process_name (PID: $pid)"
                    kill -KILL "$pid" 2>/dev/null || true
                fi
            fi
            
            PROCESS_STATUS["$process_name"]="stopped"
        fi
    done
    
    log_message $LOG_LEVEL_INFO "All processes stopped"
}

# Cleanup temporary files
cleanup_temp_files() {
    local pipe_dir="$1"
    
    if [[ -n "$pipe_dir" && -d "$pipe_dir" ]]; then
        log_message $LOG_LEVEL_DEBUG "Cleaning up temporary files in $pipe_dir"
        rm -rf "$pipe_dir"
    fi
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting advanced process management demonstration"
    
    # Initialize process pool
    local pipe_dir
    pipe_dir=$(init_process_pool)
    
    # Start trading system processes
    for process_type in "${!PROCESS_TYPES[@]}"; do
        local process_name="${PROCESS_TYPES[$process_type]}"
        start_trading_process "$process_type" "$process_name" "$pipe_dir"
        sleep 1  # Stagger process starts
    done
    
    # Start process monitoring in background
    monitor_process_health &
    local monitor_pid=$!
    
    # Run for demonstration period
    log_message $LOG_LEVEL_INFO "Trading system running for 30 seconds..."
    sleep 30
    
    # Display process statistics
    get_process_statistics
    
    # Stop all processes
    stop_all_processes
    
    # Stop monitoring
    kill "$monitor_pid" 2>/dev/null || true
    
    # Cleanup
    cleanup_temp_files "$pipe_dir"
    
    log_message $LOG_LEVEL_INFO "Advanced process management demonstration completed"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
    exit 0
fi
