#!/bin/bash
#
# Bash Fundamentals: Pipes and Redirection
# Production-Grade Script for Fintech Applications
#
# This script demonstrates advanced pipe operations, redirection,
# and data flow management for financial data processing.
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
# BASIC PIPE OPERATIONS
# =============================================================================

# Simple pipe for data filtering
filter_financial_data() {
    local input_file="$1"
    local output_file="$2"
    local filter_symbol="${3:-}"
    
    log_message $LOG_LEVEL_INFO "Filtering financial data"
    
    if [[ -n "$filter_symbol" ]]; then
        # Filter by specific symbol
        cat "$input_file" | grep "$filter_symbol" > "$output_file"
    else
        # Filter out header and comments
        cat "$input_file" | grep -v "^#" | grep -v "^symbol" > "$output_file"
    fi
    
    local filtered_count=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Filtered $filtered_count records"
}

# Pipe with multiple transformations
transform_financial_data() {
    local input_file="$1"
    local output_file="$2"
    
    log_message $LOG_LEVEL_INFO "Transforming financial data through pipe"
    
    # Multi-step transformation pipeline
    cat "$input_file" | \
    grep -v "^#" | \
    awk -F',' 'NR>1 {print $1 "," $2 "," $3}' | \
    awk -F',' '{
        symbol = $1
        price = $2
        volume = $3
        total_value = price * volume
        change_percent = (price - 100) / 100 * 100  # Simplified calculation
        print symbol "," price "," volume "," total_value "," change_percent
    }' | \
    sort -t',' -k4 -nr | \
    head -10 > "$output_file"
    
    local transformed_count=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Transformed $transformed_count records"
}

# =============================================================================
# ADVANCED PIPE OPERATIONS
# =============================================================================

# Parallel processing with pipes
parallel_data_processing() {
    local input_file="$1"
    local output_dir="$2"
    local num_processes="${3:-4}"
    
    log_message $LOG_LEVEL_INFO "Starting parallel data processing with $num_processes processes"
    
    # Split input data
    local total_lines=$(wc -l < "$input_file")
    local lines_per_process=$((total_lines / num_processes))
    
    for ((i=0; i<num_processes; i++)); do
        local start_line=$((i * lines_per_process + 1))
        local end_line=$(((i + 1) * lines_per_process))
        
        # Handle last process to include remaining lines
        if [[ $i -eq $((num_processes - 1)) ]]; then
            end_line=$total_lines
        fi
        
        # Process chunk in background
        (
            local chunk_file="$output_dir/chunk_$i.csv"
            sed -n "${start_line},${end_line}p" "$input_file" > "$chunk_file"
            
            # Process the chunk
            local processed_file="$output_dir/processed_chunk_$i.csv"
            process_chunk "$chunk_file" "$processed_file"
            
            log_message $LOG_LEVEL_DEBUG "Processed chunk $i: $((end_line - start_line + 1)) lines"
        ) &
    done
    
    # Wait for all background processes
    wait
    
    # Combine results
    local combined_output="$output_dir/combined_output.csv"
    cat "$output_dir"/processed_chunk_*.csv > "$combined_output"
    
    log_message $LOG_LEVEL_INFO "Parallel processing completed"
}

# Process individual chunk
process_chunk() {
    local input_file="$1"
    local output_file="$2"
    
    # Simple processing for demonstration
    awk -F',' '{
        if (NF >= 3) {
            symbol = $1
            price = $2
            volume = $3
            total_value = price * volume
            print symbol "," price "," volume "," total_value
        }
    }' "$input_file" > "$output_file"
}

# Pipe with error handling and logging
robust_pipe_processing() {
    local input_file="$1"
    local output_file="$2"
    local error_file="$3"
    
    log_message $LOG_LEVEL_INFO "Starting robust pipe processing with error handling"
    
    # Create temporary files for intermediate steps
    local temp_file1="/tmp/pipe_step1_$$"
    local temp_file2="/tmp/pipe_step2_$$"
    local temp_file3="/tmp/pipe_step3_$$"
    
    # Step 1: Initial filtering with error capture
    if ! cat "$input_file" 2>"$error_file" | grep -v "^#" > "$temp_file1" 2>>"$error_file"; then
        log_message $LOG_LEVEL_ERROR "Step 1 failed: Initial filtering"
        return 1
    fi
    
    # Step 2: Data validation and processing
    if ! awk -F',' '
    BEGIN { error_count = 0 }
    NR>1 {
        if (NF >= 3 && $2 > 0 && $3 > 0) {
            print $1 "," $2 "," $3 "," ($2 * $3)
        } else {
            print "ERROR: Invalid data - " $0 > "/dev/stderr"
            error_count++
        }
    }
    END { 
        if (error_count > 0) {
            print "Validation errors: " error_count > "/dev/stderr"
            exit 1
        }
    }' "$temp_file1" > "$temp_file2" 2>>"$error_file"; then
        log_message $LOG_LEVEL_ERROR "Step 2 failed: Data validation"
        return 1
    fi
    
    # Step 3: Sorting and formatting
    if ! sort -t',' -k4 -nr "$temp_file2" > "$temp_file3" 2>>"$error_file"; then
        log_message $LOG_LEVEL_ERROR "Step 3 failed: Sorting"
        return 1
    fi
    
    # Step 4: Final formatting
    {
        echo "symbol,price,volume,total_value"
        cat "$temp_file3"
    } > "$output_file"
    
    # Cleanup temporary files
    rm -f "$temp_file1" "$temp_file2" "$temp_file3"
    
    local processed_lines=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Robust pipe processing completed: $processed_lines records"
}

# =============================================================================
# REDIRECTION OPERATIONS
# =============================================================================

# Basic redirection examples
demonstrate_redirection() {
    local output_file="$1"
    
    log_message $LOG_LEVEL_INFO "Demonstrating redirection operations"
    
    # Standard output redirection
    echo "Financial data processing started at $(date)" > "$output_file"
    
    # Append to file
    echo "Processing step 1: Data validation" >> "$output_file"
    
    # Redirect both stdout and stderr
    {
        echo "Processing step 2: Data transformation"
        echo "Error simulation" >&2
    } >> "$output_file" 2>&1
    
    # Redirect stderr only
    echo "Processing step 3: Data sorting" >> "$output_file" 2>/dev/null
    
    # Discard output
    echo "Processing step 4: Final formatting" >/dev/null
    
    log_message $LOG_LEVEL_INFO "Redirection demonstration completed"
}

# Advanced redirection with file descriptors
advanced_redirection() {
    local input_file="$1"
    local output_file="$2"
    local error_file="$3"
    local log_file="$4"
    
    log_message $LOG_LEVEL_INFO "Demonstrating advanced redirection"
    
    # Open file descriptors
    exec 3>"$output_file"
    exec 4>"$error_file"
    exec 5>"$log_file"
    
    # Process data with multiple outputs
    while IFS=',' read -r symbol price volume; do
        # Skip header
        [[ "$symbol" == "symbol" ]] && continue
        
        # Log processing
        echo "Processing $symbol" >&5
        
        # Validate data
        if [[ "$price" =~ ^[0-9]+\.?[0-9]*$ ]] && [[ "$volume" =~ ^[0-9]+$ ]]; then
            # Valid data - send to output
            local total_value=$(echo "scale=2; $price * $volume" | bc -l)
            echo "$symbol,$price,$volume,$total_value" >&3
        else
            # Invalid data - send to error
            echo "ERROR: Invalid data for $symbol - price: $price, volume: $volume" >&4
        fi
    done < "$input_file"
    
    # Close file descriptors
    exec 3>&-
    exec 4>&-
    exec 5>&-
    
    log_message $LOG_LEVEL_INFO "Advanced redirection completed"
}

# =============================================================================
# PIPE WITH CONDITIONAL PROCESSING
# =============================================================================

# Conditional pipe processing based on data type
conditional_pipe_processing() {
    local input_file="$1"
    local output_file="$2"
    local data_type="$3"
    
    log_message $LOG_LEVEL_INFO "Conditional pipe processing for data type: $data_type"
    
    case "$data_type" in
        "stocks")
            # Process stock data
            cat "$input_file" | \
            grep -v "^#" | \
            awk -F',' 'NR>1 {
                symbol = $1
                price = $2
                volume = $3
                market_cap = price * volume
                print symbol "," price "," volume "," market_cap
            }' | \
            sort -t',' -k4 -nr > "$output_file"
            ;;
        "bonds")
            # Process bond data
            cat "$input_file" | \
            grep -v "^#" | \
            awk -F',' 'NR>1 {
                symbol = $1
                price = $2
                face_value = $3
                yield = (face_value - price) / price * 100
                print symbol "," price "," face_value "," yield
            }' | \
            sort -t',' -k4 -nr > "$output_file"
            ;;
        "options")
            # Process options data
            cat "$input_file" | \
            grep -v "^#" | \
            awk -F',' 'NR>1 {
                symbol = $1
                strike = $2
                premium = $3
                intrinsic_value = (strike > 0) ? strike : 0
                print symbol "," strike "," premium "," intrinsic_value
            }' | \
            sort -t',' -k4 -nr > "$output_file"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown data type: $data_type"
            return 1
            ;;
    esac
    
    local processed_count=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Processed $processed_count $data_type records"
}

# =============================================================================
# PIPE PERFORMANCE OPTIMIZATION
# =============================================================================

# Optimized pipe processing for large datasets
optimized_pipe_processing() {
    local input_file="$1"
    local output_file="$2"
    local batch_size="${3:-1000}"
    
    log_message $LOG_LEVEL_INFO "Optimized pipe processing with batch size: $batch_size"
    
    # Process data in batches to optimize memory usage
    local temp_dir="/tmp/pipe_batches_$$"
    mkdir -p "$temp_dir"
    
    # Split input into batches
    split -l "$batch_size" "$input_file" "$temp_dir/batch_"
    
    # Process each batch
    local batch_count=0
    for batch_file in "$temp_dir"/batch_*; do
        local processed_batch="$temp_dir/processed_$(basename "$batch_file")"
        
        # Process batch through pipe
        cat "$batch_file" | \
        grep -v "^#" | \
        awk -F',' 'NR>1 {
            if (NF >= 3 && $2 > 0 && $3 > 0) {
                print $1 "," $2 "," $3 "," ($2 * $3)
            }
        }' | \
        sort -t',' -k4 -nr > "$processed_batch"
        
        ((batch_count++))
        log_message $LOG_LEVEL_DEBUG "Processed batch $batch_count"
    done
    
    # Merge all processed batches
    {
        echo "symbol,price,volume,total_value"
        cat "$temp_dir"/processed_batch_* | sort -t',' -k4 -nr
    } > "$output_file"
    
    # Cleanup
    rm -rf "$temp_dir"
    
    local total_processed=$(wc -l < "$output_file")
    log_message $LOG_LEVEL_INFO "Optimized processing completed: $total_processed records in $batch_count batches"
}

# =============================================================================
# PIPE MONITORING AND DEBUGGING
# =============================================================================

# Monitor pipe performance
monitor_pipe_performance() {
    local input_file="$1"
    local output_file="$2"
    
    log_message $LOG_LEVEL_INFO "Monitoring pipe performance"
    
    local start_time=$(date +%s.%3N)
    local start_memory=$(ps -o rss= -p $$)
    
    # Process data with monitoring
    cat "$input_file" | \
    tee >(wc -l > /tmp/input_count_$$) | \
    grep -v "^#" | \
    tee >(wc -l > /tmp/filtered_count_$$) | \
    awk -F',' 'NR>1 {
        if (NF >= 3) {
            print $1 "," $2 "," $3 "," ($2 * $3)
        }
    }' | \
    tee >(wc -l > /tmp/processed_count_$$) | \
    sort -t',' -k4 -nr > "$output_file"
    
    local end_time=$(date +%s.%3N)
    local end_memory=$(ps -o rss= -p $$)
    
    # Calculate performance metrics
    local duration=$(echo "scale=3; $end_time - $start_time" | bc -l)
    local memory_used=$((end_memory - start_memory))
    
    local input_count=$(cat /tmp/input_count_$$)
    local filtered_count=$(cat /tmp/filtered_count_$$)
    local processed_count=$(cat /tmp/processed_count_$$)
    
    # Report performance
    log_message $LOG_LEVEL_INFO "Pipe Performance Metrics:"
    log_message $LOG_LEVEL_INFO "  Duration: ${duration}s"
    log_message $LOG_LEVEL_INFO "  Memory used: ${memory_used}KB"
    log_message $LOG_LEVEL_INFO "  Input records: $input_count"
    log_message $LOG_LEVEL_INFO "  Filtered records: $filtered_count"
    log_message $LOG_LEVEL_INFO "  Processed records: $processed_count"
    
    # Calculate throughput
    local throughput=$(echo "scale=0; $processed_count / $duration" | bc -l)
    log_message $LOG_LEVEL_INFO "  Throughput: ${throughput} records/second"
    
    # Cleanup
    rm -f /tmp/*_count_$$
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting pipes and redirection demonstration"
    
    # Create test data
    local test_data="/tmp/test_financial_data.csv"
    cat > "$test_data" << EOF
symbol,price,volume
AAPL,150.25,1000000
GOOGL,2800.50,500000
MSFT,300.75,750000
TSLA,800.00,200000
AMZN,3200.00,300000
META,350.00,400000
NVDA,450.00,600000
NFLX,500.00,250000
EOF
    
    # Test basic pipe operations
    log_message $LOG_LEVEL_INFO "Testing basic pipe operations"
    local filtered_output="/tmp/filtered_data.csv"
    filter_financial_data "$test_data" "$filtered_output" "AAPL"
    
    local transformed_output="/tmp/transformed_data.csv"
    transform_financial_data "$test_data" "$transformed_output"
    
    # Test advanced pipe operations
    log_message $LOG_LEVEL_INFO "Testing advanced pipe operations"
    local parallel_output_dir="/tmp/parallel_output"
    mkdir -p "$parallel_output_dir"
    parallel_data_processing "$test_data" "$parallel_output_dir" 2
    
    local robust_output="/tmp/robust_output.csv"
    local error_output="/tmp/error_output.log"
    robust_pipe_processing "$test_data" "$robust_output" "$error_output"
    
    # Test redirection
    log_message $LOG_LEVEL_INFO "Testing redirection operations"
    local redirection_output="/tmp/redirection_demo.txt"
    demonstrate_redirection "$redirection_output"
    
    local advanced_output="/tmp/advanced_output.csv"
    local advanced_error="/tmp/advanced_error.log"
    local advanced_log="/tmp/advanced_log.log"
    advanced_redirection "$test_data" "$advanced_output" "$advanced_error" "$advanced_log"
    
    # Test conditional processing
    log_message $LOG_LEVEL_INFO "Testing conditional processing"
    local stocks_output="/tmp/stocks_output.csv"
    conditional_pipe_processing "$test_data" "$stocks_output" "stocks"
    
    # Test optimized processing
    log_message $LOG_LEVEL_INFO "Testing optimized processing"
    local optimized_output="/tmp/optimized_output.csv"
    optimized_pipe_processing "$test_data" "$optimized_output" 3
    
    # Test performance monitoring
    log_message $LOG_LEVEL_INFO "Testing performance monitoring"
    local monitored_output="/tmp/monitored_output.csv"
    monitor_pipe_performance "$test_data" "$monitored_output"
    
    # Display results
    log_message $LOG_LEVEL_INFO "Processing Results:"
    log_message $LOG_LEVEL_INFO "  Filtered data: $(wc -l < "$filtered_output") records"
    log_message $LOG_LEVEL_INFO "  Transformed data: $(wc -l < "$transformed_output") records"
    log_message $LOG_LEVEL_INFO "  Robust processing: $(wc -l < "$robust_output") records"
    log_message $LOG_LEVEL_INFO "  Advanced processing: $(wc -l < "$advanced_output") records"
    log_message $LOG_LEVEL_INFO "  Conditional processing: $(wc -l < "$stocks_output") records"
    log_message $LOG_LEVEL_INFO "  Optimized processing: $(wc -l < "$optimized_output") records"
    log_message $LOG_LEVEL_INFO "  Monitored processing: $(wc -l < "$monitored_output") records"
    
    # Cleanup
    rm -f "$test_data" "$filtered_output" "$transformed_output" "$robust_output" "$error_output"
    rm -f "$redirection_output" "$advanced_output" "$advanced_error" "$advanced_log"
    rm -f "$stocks_output" "$optimized_output" "$monitored_output"
    rm -rf "$parallel_output_dir"
    
    log_message $LOG_LEVEL_INFO "Pipes and redirection demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    main "$@"
    exit 0
fi
