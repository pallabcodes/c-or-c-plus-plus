#!/bin/bash
#
# Bash Fundamentals: Sleep, Process Management, and Pipes
# Production-Grade Script for Fintech Applications
#
# This script demonstrates sleep functionality, process management,
# pipes, and inter-process communication for financial applications.
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
# SLEEP AND TIMING FUNCTIONS
# =============================================================================

# Precise sleep with microsecond accuracy
precise_sleep() {
    local duration="$1"
    local unit="${2:-seconds}"
    
    case "$unit" in
        "seconds"|"s")
            sleep "$duration"
            ;;
        "milliseconds"|"ms")
            local ms_duration=$(echo "scale=3; $duration / 1000" | bc -l)
            sleep "$ms_duration"
            ;;
        "microseconds"|"μs"|"us")
            local us_duration=$(echo "scale=6; $duration / 1000000" | bc -l)
            sleep "$us_duration"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unsupported time unit: $unit"
            return 1
            ;;
    esac
}

# Sleep with progress indicator
sleep_with_progress() {
    local duration="$1"
    local interval="${2:-1}"
    local message="${3:-Processing}"
    
    log_message $LOG_LEVEL_INFO "$message for $duration seconds..."
    
    local elapsed=0
    while [[ $elapsed -lt $duration ]]; do
        local remaining=$((duration - elapsed))
        printf "\r%s: %d seconds remaining..." "$message" "$remaining"
        sleep "$interval"
        elapsed=$((elapsed + interval))
    done
    
    printf "\r%s: Complete!                    \n" "$message"
}

# Conditional sleep based on market hours
sleep_until_market_open() {
    local market_open_hour="${1:-9}"
    local market_open_minute="${2:-30}"
    
    log_message $LOG_LEVEL_INFO "Waiting for market to open at ${market_open_hour}:${market_open_minute}"
    
    while true; do
        local current_hour=$(date +%H)
        local current_minute=$(date +%M)
        local current_second=$(date +%S)
        
        # Check if market is open
        if [[ $current_hour -gt $market_open_hour ]] || 
           ([[ $current_hour -eq $market_open_hour ]] && [[ $current_minute -ge $market_open_minute ]]); then
            log_message $LOG_LEVEL_INFO "Market is now open!"
            break
        fi
        
        # Calculate seconds until market opens
        local current_total_seconds=$((current_hour * 3600 + current_minute * 60 + current_second))
        local market_open_total_seconds=$((market_open_hour * 3600 + market_open_minute * 60))
        local seconds_until_open=$((market_open_total_seconds - current_total_seconds))
        
        # Handle case where market opens next day
        if [[ $seconds_until_open -lt 0 ]]; then
            seconds_until_open=$((seconds_until_open + 86400))  # Add 24 hours
        fi
        
        log_message $LOG_LEVEL_DEBUG "Current time: ${current_hour}:${current_minute}:${current_second}"
        log_message $LOG_LEVEL_DEBUG "Market opens in: $seconds_until_open seconds"
        
        # Sleep for 1 minute
        sleep 60
    done
}

# =============================================================================
# PROCESS MANAGEMENT
# =============================================================================

# Start background process with PID tracking
start_background_process() {
    local command="$1"
    local process_name="${2:-background_process}"
    local log_file="${3:-/tmp/${process_name}_$$.log}"
    
    log_message $LOG_LEVEL_INFO "Starting background process: $process_name"
    log_message $LOG_LEVEL_DEBUG "Command: $command"
    log_message $LOG_LEVEL_DEBUG "Log file: $log_file"
    
    # Start process in background
    (
        echo "Process started: $(date)" > "$log_file"
        eval "$command" >> "$log_file" 2>&1
        echo "Process completed: $(date)" >> "$log_file"
    ) &
    
    local pid=$!
    echo "$pid" > "/tmp/${process_name}_$$.pid"
    
    log_message $LOG_LEVEL_INFO "Background process started with PID: $pid"
    echo "$pid"
}

# Monitor background process
monitor_background_process() {
    local pid="$1"
    local process_name="${2:-background_process}"
    local timeout="${3:-300}"  # 5 minutes default timeout
    
    log_message $LOG_LEVEL_INFO "Monitoring process $process_name (PID: $pid)"
    
    local start_time=$(date +%s)
    local is_running=true
    
    while [[ "$is_running" == "true" ]]; do
        # Check if process is still running
        if ! kill -0 "$pid" 2>/dev/null; then
            log_message $LOG_LEVEL_INFO "Process $process_name (PID: $pid) has completed"
            is_running=false
            break
        fi
        
        # Check timeout
        local current_time=$(date +%s)
        local elapsed=$((current_time - start_time))
        
        if [[ $elapsed -gt $timeout ]]; then
            log_message $LOG_LEVEL_WARN "Process $process_name (PID: $pid) timed out after $timeout seconds"
            kill "$pid" 2>/dev/null || true
            is_running=false
            break
        fi
        
        # Sleep before next check
        sleep 5
        
        local remaining=$((timeout - elapsed))
        log_message $LOG_LEVEL_DEBUG "Process $process_name still running, $remaining seconds remaining"
    done
    
    # Get exit code
    wait "$pid" 2>/dev/null
    local exit_code=$?
    
    log_message $LOG_LEVEL_INFO "Process $process_name completed with exit code: $exit_code"
    return $exit_code
}

# Kill background process gracefully
kill_background_process() {
    local pid="$1"
    local process_name="${2:-background_process}"
    local force="${3:-false}"
    
    log_message $LOG_LEVEL_INFO "Stopping process $process_name (PID: $pid)"
    
    # Try graceful termination first
    if kill -TERM "$pid" 2>/dev/null; then
        log_message $LOG_LEVEL_DEBUG "Sent TERM signal to process $pid"
        
        # Wait for graceful shutdown
        local count=0
        while kill -0 "$pid" 2>/dev/null && [[ $count -lt 10 ]]; do
            sleep 1
            ((count++))
        done
        
        # Check if process is still running
        if kill -0 "$pid" 2>/dev/null; then
            if [[ "$force" == "true" ]]; then
                log_message $LOG_LEVEL_WARN "Process $pid did not respond to TERM, forcing kill"
                kill -KILL "$pid" 2>/dev/null || true
            else
                log_message $LOG_LEVEL_WARN "Process $pid did not respond to TERM signal"
                return 1
            fi
        else
            log_message $LOG_LEVEL_INFO "Process $pid terminated gracefully"
        fi
    else
        log_message $LOG_LEVEL_WARN "Process $pid not found or already terminated"
    fi
}

# =============================================================================
# PIPE OPERATIONS
# =============================================================================

# Process data through pipes
process_financial_data_pipe() {
    local input_file="$1"
    local output_file="$2"
    
    log_message $LOG_LEVEL_INFO "Processing financial data through pipes"
    
    # Create a pipeline for data processing
    cat "$input_file" | \
    grep -v "^#" | \
    awk -F',' 'NR>1 {print $1 "," $2 "," $3}' | \
    sort | \
    awk -F',' '{
        symbol=$1; price=$2; volume=$3
        total_value = price * volume
        print symbol "," price "," volume "," total_value
    }' | \
    sort -t',' -k4 -nr > "$output_file"
    
    local processed_lines=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Processed $processed_lines records through pipe"
}

# Advanced pipe processing with error handling
advanced_pipe_processing() {
    local input_file="$1"
    local output_file="$2"
    
    log_message $LOG_LEVEL_INFO "Advanced pipe processing with error handling"
    
    # Create temporary files for intermediate steps
    local temp_file1="/tmp/pipe_step1_$$"
    local temp_file2="/tmp/pipe_step2_$$"
    local temp_file3="/tmp/pipe_step3_$$"
    
    # Step 1: Filter and validate data
    if ! cat "$input_file" | grep -v "^#" > "$temp_file1" 2>/dev/null; then
        log_message $LOG_LEVEL_ERROR "Failed to filter input data"
        return 1
    fi
    
    # Step 2: Process and calculate
    if ! awk -F',' 'NR>1 {
        if (NF >= 3 && $2 > 0 && $3 > 0) {
            print $1 "," $2 "," $3 "," ($2 * $3)
        } else {
            print "ERROR: Invalid data - " $0 > "/dev/stderr"
        }
    }' "$temp_file1" > "$temp_file2" 2>/dev/null; then
        log_message $LOG_LEVEL_ERROR "Failed to process data"
        return 1
    fi
    
    # Step 3: Sort by value
    if ! sort -t',' -k4 -nr "$temp_file2" > "$temp_file3" 2>/dev/null; then
        log_message $LOG_LEVEL_ERROR "Failed to sort data"
        return 1
    fi
    
    # Step 4: Add headers and format output
    {
        echo "symbol,price,volume,total_value"
        cat "$temp_file3"
    } > "$output_file"
    
    # Cleanup temporary files
    rm -f "$temp_file1" "$temp_file2" "$temp_file3"
    
    local processed_lines=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Advanced pipe processing completed: $processed_lines records"
}

# =============================================================================
# INTER-PROCESS COMMUNICATION
# =============================================================================

# Create named pipe for communication
create_named_pipe() {
    local pipe_name="$1"
    
    log_message $LOG_LEVEL_INFO "Creating named pipe: $pipe_name"
    
    if [[ -p "$pipe_name" ]]; then
        log_message $LOG_LEVEL_WARN "Named pipe already exists: $pipe_name"
        return 0
    fi
    
    if mkfifo "$pipe_name" 2>/dev/null; then
        log_message $LOG_LEVEL_INFO "Named pipe created successfully: $pipe_name"
        return 0
    else
        log_message $LOG_LEVEL_ERROR "Failed to create named pipe: $pipe_name"
        return 1
    fi
}

# Producer process for named pipe
producer_process() {
    local pipe_name="$1"
    local data_file="$2"
    local interval="${3:-1}"
    
    log_message $LOG_LEVEL_INFO "Starting producer process for pipe: $pipe_name"
    
    # Open pipe for writing
    exec 3>"$pipe_name"
    
    # Send data through pipe
    while IFS= read -r line; do
        echo "$line" >&3
        log_message $LOG_LEVEL_DEBUG "Sent data: $line"
        sleep "$interval"
    done < "$data_file"
    
    # Close pipe
    exec 3>&-
    
    log_message $LOG_LEVEL_INFO "Producer process completed"
}

# Consumer process for named pipe
consumer_process() {
    local pipe_name="$1"
    local output_file="$2"
    
    log_message $LOG_LEVEL_INFO "Starting consumer process for pipe: $pipe_name"
    
    # Open pipe for reading
    exec 3<"$pipe_name"
    
    # Read data from pipe
    local count=0
    while IFS= read -r line <&3; do
        echo "$line" >> "$output_file"
        ((count++))
        log_message $LOG_LEVEL_DEBUG "Received data: $line"
    done
    
    # Close pipe
    exec 3<&-
    
    log_message $LOG_LEVEL_INFO "Consumer process completed: $count records processed"
}

# =============================================================================
# PROCESS MONITORING AND HEALTH CHECKS
# =============================================================================

# Monitor system processes
monitor_system_processes() {
    local process_pattern="$1"
    local max_cpu="${2:-80}"
    local max_memory="${3:-80}"
    
    log_message $LOG_LEVEL_INFO "Monitoring processes matching pattern: $process_pattern"
    
    # Get process information
    local processes=$(ps aux | grep "$process_pattern" | grep -v grep)
    
    if [[ -z "$processes" ]]; then
        log_message $LOG_LEVEL_WARN "No processes found matching pattern: $process_pattern"
        return 1
    fi
    
    echo "$processes" | while IFS= read -r line; do
        local pid=$(echo "$line" | awk '{print $2}')
        local cpu=$(echo "$line" | awk '{print $3}')
        local memory=$(echo "$line" | awk '{print $4}')
        local command=$(echo "$line" | awk '{for(i=11;i<=NF;i++) printf "%s ", $i; print ""}')
        
        # Check CPU usage
        if (( $(echo "$cpu > $max_cpu" | bc -l) )); then
            log_message $LOG_LEVEL_WARN "High CPU usage: PID $pid ($cpu%) - $command"
        fi
        
        # Check memory usage
        if (( $(echo "$memory > $max_memory" | bc -l) )); then
            log_message $LOG_LEVEL_WARN "High memory usage: PID $pid ($memory%) - $command"
        fi
        
        log_message $LOG_LEVEL_DEBUG "Process $pid: CPU=$cpu%, Memory=$memory%"
    done
}

# Health check for financial processes
health_check_financial_processes() {
    local health_status="healthy"
    local issues=()
    
    log_message $LOG_LEVEL_INFO "Performing health check on financial processes"
    
    # Check if critical processes are running
    local critical_processes=("financial_processor" "risk_monitor" "data_ingestion")
    
    for process in "${critical_processes[@]}"; do
        if ! pgrep -f "$process" >/dev/null 2>&1; then
            health_status="unhealthy"
            issues+=("Process $process is not running")
        fi
    done
    
    # Check system resources
    local cpu_usage=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | sed 's/%us,//')
    if (( $(echo "$cpu_usage > 90" | bc -l) )); then
        health_status="degraded"
        issues+=("High CPU usage: $cpu_usage%")
    fi
    
    local memory_usage=$(free | awk 'NR==2{printf "%.1f", $3*100/$2}')
    if (( $(echo "$memory_usage > 90" | bc -l) )); then
        health_status="degraded"
        issues+=("High memory usage: $memory_usage%")
    fi
    
    # Report health status
    case "$health_status" in
        "healthy")
            log_message $LOG_LEVEL_INFO "Health check passed: All systems healthy"
            ;;
        "degraded")
            log_message $LOG_LEVEL_WARN "Health check warning: System degraded"
            for issue in "${issues[@]}"; do
                log_message $LOG_LEVEL_WARN "  - $issue"
            done
            ;;
        "unhealthy")
            log_message $LOG_LEVEL_ERROR "Health check failed: System unhealthy"
            for issue in "${issues[@]}"; do
                log_message $LOG_LEVEL_ERROR "  - $issue"
            done
            ;;
    esac
    
    echo "$health_status"
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting sleep and process management demonstration"
    
    # Test sleep functions
    log_message $LOG_LEVEL_INFO "Testing sleep functions"
    precise_sleep 1 "seconds"
    precise_sleep 100 "milliseconds"
    
    # Test sleep with progress
    sleep_with_progress 3 1 "Data processing"
    
    # Test market hours sleep (shortened for demo)
    log_message $LOG_LEVEL_INFO "Testing market hours sleep (shortened demo)"
    local current_hour=$(date +%H)
    local current_minute=$(date +%M)
    local next_minute=$((current_minute + 1))
    if [[ $next_minute -ge 60 ]]; then
        next_minute=0
        current_hour=$((current_hour + 1))
    fi
    sleep_until_market_open "$current_hour" "$next_minute"
    
    # Test background process management
    log_message $LOG_LEVEL_INFO "Testing background process management"
    
    # Create test data
    local test_data="/tmp/test_financial_data.csv"
    cat > "$test_data" << EOF
symbol,price,volume
AAPL,150.25,1000000
GOOGL,2800.50,500000
MSFT,300.75,750000
TSLA,800.00,200000
EOF
    
    # Start background process
    local bg_pid=$(start_background_process "sleep 10 && echo 'Background process completed'" "test_process")
    
    # Monitor the process
    monitor_background_process "$bg_pid" "test_process" 15
    
    # Test pipe processing
    log_message $LOG_LEVEL_INFO "Testing pipe processing"
    local output_file="/tmp/processed_data.csv"
    process_financial_data_pipe "$test_data" "$output_file"
    
    # Test advanced pipe processing
    local advanced_output="/tmp/advanced_processed_data.csv"
    advanced_pipe_processing "$test_data" "$advanced_output"
    
    # Test named pipes
    log_message $LOG_LEVEL_INFO "Testing named pipes"
    local pipe_name="/tmp/financial_pipe_$$"
    create_named_pipe "$pipe_name"
    
    # Start producer and consumer in background
    producer_process "$pipe_name" "$test_data" 0.5 &
    local producer_pid=$!
    
    local consumer_output="/tmp/consumer_output.csv"
    consumer_process "$pipe_name" "$consumer_output" &
    local consumer_pid=$!
    
    # Wait for processes to complete
    wait "$producer_pid"
    wait "$consumer_pid"
    
    # Test process monitoring
    log_message $LOG_LEVEL_INFO "Testing process monitoring"
    monitor_system_processes "bash" 50 50
    
    # Test health check
    health_check_financial_processes
    
    # Cleanup
    rm -f "$test_data" "$output_file" "$advanced_output" "$consumer_output" "$pipe_name"
    rm -f "/tmp/test_process_$$.pid" "/tmp/test_process_$$.log"
    
    log_message $LOG_LEVEL_INFO "Sleep and process management demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    main "$@"
    exit 0
fi
