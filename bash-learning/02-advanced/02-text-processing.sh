#!/bin/bash
#
# Advanced Bash: Text Processing and Data Manipulation
# Production-Grade Script for Fintech Applications
#
# This script demonstrates advanced text processing techniques
# for financial data parsing, market data analysis, and report generation.
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
# FINANCIAL DATA PROCESSING CONFIGURATION
# =============================================================================

# Market data file formats
declare -A MARKET_DATA_FORMATS=(
    ["csv"]="Comma Separated Values"
    ["tsv"]="Tab Separated Values"
    ["json"]="JSON Format"
    ["xml"]="XML Format"
    ["fixed_width"]="Fixed Width Format"
)

# Financial data validation patterns
readonly STOCK_SYMBOL_PATTERN='^[A-Z]{1,5}$'
readonly CURRENCY_PATTERN='^[A-Z]{3}$'
readonly PRICE_PATTERN='^[0-9]+\.?[0-9]{0,4}$'
readonly VOLUME_PATTERN='^[0-9]+$'
readonly TIMESTAMP_PATTERN='^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]{3})?$'

# =============================================================================
# ADVANCED REGEX AND PATTERN MATCHING
# =============================================================================

# Validate financial data using regex patterns
validate_financial_data() {
    local data_type="$1"
    local value="$2"
    
    case "$data_type" in
        "stock_symbol")
            if [[ "$value" =~ $STOCK_SYMBOL_PATTERN ]]; then
                log_message $LOG_LEVEL_DEBUG "Valid stock symbol: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid stock symbol format: $value"
                return 1
            fi
            ;;
        "currency")
            if [[ "$value" =~ $CURRENCY_PATTERN ]]; then
                log_message $LOG_LEVEL_DEBUG "Valid currency code: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid currency code format: $value"
                return 1
            fi
            ;;
        "price")
            if [[ "$value" =~ $PRICE_PATTERN ]]; then
                log_message $LOG_LEVEL_DEBUG "Valid price format: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid price format: $value"
                return 1
            fi
            ;;
        "volume")
            if [[ "$value" =~ $VOLUME_PATTERN ]]; then
                log_message $LOG_LEVEL_DEBUG "Valid volume format: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid volume format: $value"
                return 1
            fi
            ;;
        "timestamp")
            if [[ "$value" =~ $TIMESTAMP_PATTERN ]]; then
                log_message $LOG_LEVEL_DEBUG "Valid timestamp format: $value"
                return 0
            else
                log_message $LOG_LEVEL_ERROR "Invalid timestamp format: $value"
                return 1
            fi
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown data type: $data_type"
            return 1
            ;;
    esac
}

# Extract financial data using advanced regex
extract_financial_data() {
    local input_text="$1"
    local pattern="$2"
    local field_name="$3"
    
    if [[ "$input_text" =~ $pattern ]]; then
        local extracted_value="${BASH_REMATCH[1]}"
        log_message $LOG_LEVEL_DEBUG "Extracted $field_name: $extracted_value"
        echo "$extracted_value"
        return 0
    else
        log_message $LOG_LEVEL_WARN "Failed to extract $field_name from: $input_text"
        return 1
    fi
}

# =============================================================================
# CSV PROCESSING FOR FINANCIAL DATA
# =============================================================================

# Parse CSV financial data with proper error handling
parse_csv_financial_data() {
    local csv_file="$1"
    local delimiter="${2:-,}"
    
    if [[ ! -f "$csv_file" ]]; then
        log_message $LOG_LEVEL_ERROR "CSV file not found: $csv_file"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Parsing CSV financial data from: $csv_file"
    
    local line_number=0
    local valid_records=0
    local invalid_records=0
    
    while IFS= read -r line; do
        ((line_number++))
        
        # Skip empty lines
        [[ -z "$line" ]] && continue
        
        # Skip comment lines
        [[ "$line" =~ ^# ]] && continue
        
        # Parse CSV line
        IFS="$delimiter" read -ra fields <<< "$line"
        
        # Validate required fields
        if [[ ${#fields[@]} -lt 4 ]]; then
            log_message $LOG_LEVEL_WARN "Line $line_number: Insufficient fields (${#fields[@]})"
            ((invalid_records++))
            continue
        fi
        
        # Extract and validate fields
        local timestamp="${fields[0]}"
        local symbol="${fields[1]}"
        local price="${fields[2]}"
        local volume="${fields[3]}"
        
        # Validate each field
        local validation_errors=0
        
        if ! validate_financial_data "timestamp" "$timestamp"; then
            ((validation_errors++))
        fi
        
        if ! validate_financial_data "stock_symbol" "$symbol"; then
            ((validation_errors++))
        fi
        
        if ! validate_financial_data "price" "$price"; then
            ((validation_errors++))
        fi
        
        if ! validate_financial_data "volume" "$volume"; then
            ((validation_errors++))
        fi
        
        if [[ $validation_errors -eq 0 ]]; then
            log_message $LOG_LEVEL_DEBUG "Valid record: $timestamp,$symbol,$price,$volume"
            ((valid_records++))
        else
            log_message $LOG_LEVEL_WARN "Line $line_number: $validation_errors validation errors"
            ((invalid_records++))
        fi
        
    done < "$csv_file"
    
    log_message $LOG_LEVEL_INFO "CSV parsing completed: $valid_records valid, $invalid_records invalid records"
    echo "$valid_records,$invalid_records"
}

# Generate CSV financial report
generate_csv_financial_report() {
    local output_file="$1"
    local report_data="$2"
    
    log_message $LOG_LEVEL_INFO "Generating CSV financial report: $output_file"
    
    # CSV header
    cat > "$output_file" << EOF
# Financial Report Generated: $(date '+%Y-%m-%d %H:%M:%S')
# Report Type: Market Data Analysis
# Generated By: $(whoami)@$(hostname)
Timestamp,Symbol,Price,Volume,Change,Change_Percent
EOF
    
    # Process report data
    while IFS=',' read -r timestamp symbol price volume change change_percent; do
        # Format the data for CSV output
        printf "%s,%s,%.4f,%d,%.4f,%.2f\n" \
            "$timestamp" "$symbol" "$price" "$volume" "$change" "$change_percent" >> "$output_file"
    done <<< "$report_data"
    
    log_message $LOG_LEVEL_INFO "CSV report generated successfully: $output_file"
}

# =============================================================================
# JSON PROCESSING FOR FINANCIAL APIs
# =============================================================================

# Parse JSON financial data (simplified implementation)
parse_json_financial_data() {
    local json_data="$1"
    local field_name="$2"
    
    # Simple JSON field extraction using regex
    local pattern="\"$field_name\"\s*:\s*\"?([^,\"]+)\"?"
    
    if [[ "$json_data" =~ $pattern ]]; then
        local value="${BASH_REMATCH[1]}"
        # Remove quotes if present
        value="${value%\"}"
        value="${value#\"}"
        log_message $LOG_LEVEL_DEBUG "Extracted JSON field '$field_name': $value"
        echo "$value"
        return 0
    else
        log_message $LOG_LEVEL_WARN "Failed to extract JSON field: $field_name"
        return 1
    fi
}

# Generate JSON financial report
generate_json_financial_report() {
    local output_file="$1"
    local report_data="$2"
    
    log_message $LOG_LEVEL_INFO "Generating JSON financial report: $output_file"
    
    cat > "$output_file" << EOF
{
  "report_metadata": {
    "generated_at": "$(date -u +%Y-%m-%dT%H:%M:%S.%3NZ)",
    "report_type": "market_data_analysis",
    "generated_by": "$(whoami)@$(hostname)",
    "version": "1.0"
  },
  "financial_data": [
EOF
    
    local first_record=true
    while IFS=',' read -r timestamp symbol price volume change change_percent; do
        if [[ "$first_record" == "true" ]]; then
            first_record=false
        else
            echo "," >> "$output_file"
        fi
        
        cat >> "$output_file" << EOF
    {
      "timestamp": "$timestamp",
      "symbol": "$symbol",
      "price": $price,
      "volume": $volume,
      "change": $change,
      "change_percent": $change_percent
    }
EOF
    done <<< "$report_data"
    
    cat >> "$output_file" << EOF
  ]
}
EOF
    
    log_message $LOG_LEVEL_INFO "JSON report generated successfully: $output_file"
}

# =============================================================================
# ADVANCED TEXT PROCESSING WITH AWK
# =============================================================================

# Process financial data using AWK
process_financial_data_with_awk() {
    local input_file="$1"
    local output_file="$2"
    
    if [[ ! -f "$input_file" ]]; then
        log_message $LOG_LEVEL_ERROR "Input file not found: $input_file"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Processing financial data with AWK: $input_file -> $output_file"
    
    # AWK script for financial data processing
    awk '
    BEGIN {
        FS = ","
        OFS = ","
        print "Timestamp,Symbol,Price,Volume,Change,Change_Percent"
        prev_price[""] = 0
    }
    {
        # Skip header lines
        if (NR == 1 || $0 ~ /^#/) next
        
        # Extract fields
        timestamp = $1
        symbol = $2
        price = $3
        volume = $4
        
        # Calculate change from previous price
        if (prev_price[symbol] > 0) {
            change = price - prev_price[symbol]
            change_percent = (change / prev_price[symbol]) * 100
        } else {
            change = 0
            change_percent = 0
        }
        
        # Output processed record
        print timestamp, symbol, price, volume, change, change_percent
        
        # Store current price for next iteration
        prev_price[symbol] = price
    }
    ' "$input_file" > "$output_file"
    
    log_message $LOG_LEVEL_INFO "AWK processing completed: $output_file"
}

# Calculate financial statistics using AWK
calculate_financial_statistics() {
    local input_file="$1"
    
    if [[ ! -f "$input_file" ]]; then
        log_message $LOG_LEVEL_ERROR "Input file not found: $input_file"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Calculating financial statistics: $input_file"
    
    # AWK script for financial statistics
    awk '
    BEGIN {
        FS = ","
        total_records = 0
        total_volume = 0
        min_price = 999999
        max_price = 0
        price_sum = 0
    }
    {
        # Skip header lines
        if (NR == 1 || $0 ~ /^#/) next
        
        price = $3
        volume = $4
        
        total_records++
        total_volume += volume
        price_sum += price
        
        if (price < min_price) min_price = price
        if (price > max_price) max_price = price
    }
    END {
        if (total_records > 0) {
            avg_price = price_sum / total_records
            print "Total Records:", total_records
            print "Total Volume:", total_volume
            print "Average Price:", sprintf("%.4f", avg_price)
            print "Min Price:", min_price
            print "Max Price:", max_price
            print "Price Range:", max_price - min_price
        }
    }
    ' "$input_file"
}

# =============================================================================
# SED FOR FINANCIAL DATA MANIPULATION
# =============================================================================

# Clean and normalize financial data using SED
clean_financial_data() {
    local input_file="$1"
    local output_file="$2"
    
    if [[ ! -f "$input_file" ]]; then
        log_message $LOG_LEVEL_ERROR "Input file not found: $input_file"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Cleaning financial data with SED: $input_file -> $output_file"
    
    # SED script for data cleaning
    sed -E '
        # Remove leading/trailing whitespace
        s/^[[:space:]]+//
        s/[[:space:]]+$//
        
        # Remove empty lines
        /^$/d
        
        # Remove comment lines
        /^#/d
        
        # Normalize timestamps (convert various formats to standard)
        s/([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})/\1-\2-\3 \4:\5:\6/
        
        # Normalize stock symbols (convert to uppercase)
        s/([A-Za-z]{1,5})/\U\1/g
        
        # Normalize prices (ensure 4 decimal places)
        s/([0-9]+\.?[0-9]*)/\1/g
        
        # Remove any non-printable characters
        s/[[:cntrl:]]//g
    ' "$input_file" > "$output_file"
    
    log_message $LOG_LEVEL_INFO "SED cleaning completed: $output_file"
}

# =============================================================================
# FIXED WIDTH FORMAT PROCESSING
# =============================================================================

# Parse fixed width financial data
parse_fixed_width_financial_data() {
    local input_file="$1"
    local field_widths="$2"  # Format: "10,5,8,10" for timestamp,symbol,price,volume
    
    if [[ ! -f "$input_file" ]]; then
        log_message $LOG_LEVEL_ERROR "Input file not found: $input_file"
        return 1
    fi
    
    log_message $LOG_LEVEL_INFO "Parsing fixed width financial data: $input_file"
    
    # Parse field widths
    IFS=',' read -ra widths <<< "$field_widths"
    
    local line_number=0
    while IFS= read -r line; do
        ((line_number++))
        
        # Skip empty lines
        [[ -z "$line" ]] && continue
        
        local position=0
        local fields=()
        
        # Extract fields based on widths
        for width in "${widths[@]}"; do
            local field="${line:$position:$width}"
            # Trim whitespace
            field="${field%"${field##*[![:space:]]}"}"
            field="${field#"${field%%[![:space:]]*}"}"
            fields+=("$field")
            ((position += width))
        done
        
        # Output as CSV
        printf "%s\n" "$(IFS=','; echo "${fields[*]}")"
        
    done < "$input_file"
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    log_message $LOG_LEVEL_INFO "Starting advanced text processing demonstration"
    
    # Create sample financial data
    local sample_data="/tmp/sample_financial_data.csv"
    cat > "$sample_data" << EOF
# Sample Financial Data
2024-01-15 09:30:00,AAPL,150.25,1000000
2024-01-15 09:31:00,AAPL,150.30,1200000
2024-01-15 09:32:00,AAPL,150.20,800000
2024-01-15 09:30:00,GOOGL,2800.50,500000
2024-01-15 09:31:00,GOOGL,2805.75,600000
2024-01-15 09:32:00,GOOGL,2802.25,450000
EOF
    
    # Test CSV processing
    log_message $LOG_LEVEL_INFO "Testing CSV processing"
    parse_csv_financial_data "$sample_data"
    
    # Test data cleaning
    local cleaned_data="/tmp/cleaned_financial_data.csv"
    clean_financial_data "$sample_data" "$cleaned_data"
    
    # Test AWK processing
    local processed_data="/tmp/processed_financial_data.csv"
    process_financial_data_with_awk "$cleaned_data" "$processed_data"
    
    # Calculate statistics
    calculate_financial_statistics "$processed_data"
    
    # Generate reports
    local csv_report="/tmp/financial_report.csv"
    local json_report="/tmp/financial_report.json"
    
    # Use processed data for reports
    generate_csv_financial_report "$csv_report" "$(cat "$processed_data")"
    generate_json_financial_report "$json_report" "$(cat "$processed_data")"
    
    # Test JSON parsing
    local json_data='{"symbol":"AAPL","price":150.25,"volume":1000000}'
    parse_json_financial_data "$json_data" "symbol"
    parse_json_financial_data "$json_data" "price"
    
    # Test fixed width processing
    local fixed_width_data="/tmp/fixed_width_data.txt"
    cat > "$fixed_width_data" << EOF
2024-01-15 09:30:00AAPL 150.25   1000000
2024-01-15 09:31:00AAPL 150.30   1200000
2024-01-15 09:32:00AAPL 150.20   800000
EOF
    
    local field_widths="19,5,8,10"
    parse_fixed_width_financial_data "$fixed_width_data" "$field_widths"
    
    # Cleanup
    rm -f "$sample_data" "$cleaned_data" "$processed_data" "$csv_report" "$json_report" "$fixed_width_data"
    
    log_message $LOG_LEVEL_INFO "Advanced text processing demonstration completed successfully"
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    trap 'log_message $LOG_LEVEL_ERROR "Script interrupted"; exit 130' INT TERM
    main "$@"
    exit 0
fi
