#!/bin/bash
#
# Production Practices: Testing Framework
# Enterprise-Grade Testing Framework for Bash Scripts
#
# This script provides a comprehensive testing framework for bash scripts
# with unit testing, integration testing, performance testing, and
# automated test execution capabilities.
#
# Author: System Engineering Team
# Version: 1.0
# Last Modified: $(date +%Y-%m-%d)
#

# =============================================================================
# TESTING FRAMEWORK CONFIGURATION
# =============================================================================

set -euo pipefail
IFS=$'\n\t'

# Test framework configuration
readonly TEST_FRAMEWORK_VERSION="1.0.0"
readonly TEST_RESULTS_DIR="/tmp/bash_test_results"
readonly TEST_LOGS_DIR="/tmp/bash_test_logs"
readonly TEST_COVERAGE_DIR="/tmp/bash_test_coverage"

# Test execution modes
readonly TEST_MODE_UNIT="unit"
readonly TEST_MODE_INTEGRATION="integration"
readonly TEST_MODE_PERFORMANCE="performance"
readonly TEST_MODE_ALL="all"

# Test result codes
readonly TEST_RESULT_PASS=0
readonly TEST_RESULT_FAIL=1
readonly TEST_RESULT_SKIP=2
readonly TEST_RESULT_ERROR=3

# Test statistics
declare -A TEST_STATS=(
    ["total"]="0"
    ["passed"]="0"
    ["failed"]="0"
    ["skipped"]="0"
    ["errors"]="0"
)

# =============================================================================
# LOGGING AND REPORTING
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

# Initialize test framework
init_test_framework() {
    log_message $LOG_LEVEL_INFO "Initializing bash testing framework v$TEST_FRAMEWORK_VERSION"
    
    # Create test directories
    mkdir -p "$TEST_RESULTS_DIR" "$TEST_LOGS_DIR" "$TEST_COVERAGE_DIR"
    
    # Initialize test statistics
    TEST_STATS["total"]="0"
    TEST_STATS["passed"]="0"
    TEST_STATS["failed"]="0"
    TEST_STATS["skipped"]="0"
    TEST_STATS["errors"]="0"
    
    log_message $LOG_LEVEL_INFO "Test framework initialized successfully"
}

# =============================================================================
# TEST ASSERTION FUNCTIONS
# =============================================================================

# Assert equality
assert_equal() {
    local expected="$1"
    local actual="$2"
    local test_name="${3:-assert_equal}"
    
    if [[ "$expected" == "$actual" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - Expected: '$expected', Actual: '$actual'"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - Expected: '$expected', Actual: '$actual'"
        return $TEST_RESULT_FAIL
    fi
}

# Assert inequality
assert_not_equal() {
    local expected="$1"
    local actual="$2"
    local test_name="${3:-assert_not_equal}"
    
    if [[ "$expected" != "$actual" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - Values are not equal as expected"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - Values are equal: '$expected'"
        return $TEST_RESULT_FAIL
    fi
}

# Assert true (exit code 0)
assert_true() {
    local command="$1"
    local test_name="${2:-assert_true}"
    
    if eval "$command" >/dev/null 2>&1; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - Command returned true"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - Command returned false: $command"
        return $TEST_RESULT_FAIL
    fi
}

# Assert false (non-zero exit code)
assert_false() {
    local command="$1"
    local test_name="${2:-assert_false}"
    
    if ! eval "$command" >/dev/null 2>&1; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - Command returned false as expected"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - Command returned true: $command"
        return $TEST_RESULT_FAIL
    fi
}

# Assert file exists
assert_file_exists() {
    local file_path="$1"
    local test_name="${2:-assert_file_exists}"
    
    if [[ -f "$file_path" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - File exists: $file_path"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - File does not exist: $file_path"
        return $TEST_RESULT_FAIL
    fi
}

# Assert file does not exist
assert_file_not_exists() {
    local file_path="$1"
    local test_name="${2:-assert_file_not_exists}"
    
    if [[ ! -f "$file_path" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - File does not exist as expected: $file_path"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - File exists: $file_path"
        return $TEST_RESULT_FAIL
    fi
}

# Assert directory exists
assert_dir_exists() {
    local dir_path="$1"
    local test_name="${2:-assert_dir_exists}"
    
    if [[ -d "$dir_path" ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - Directory exists: $dir_path"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - Directory does not exist: $dir_path"
        return $TEST_RESULT_FAIL
    fi
}

# Assert string contains substring
assert_contains() {
    local string="$1"
    local substring="$2"
    local test_name="${3:-assert_contains}"
    
    if [[ "$string" == *"$substring"* ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - String contains substring"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - String does not contain substring: '$substring' in '$string'"
        return $TEST_RESULT_FAIL
    fi
}

# Assert string does not contain substring
assert_not_contains() {
    local string="$1"
    local substring="$2"
    local test_name="${3:-assert_not_contains}"
    
    if [[ "$string" != *"$substring"* ]]; then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - String does not contain substring as expected"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - String contains substring: '$substring' in '$string'"
        return $TEST_RESULT_FAIL
    fi
}

# Assert numeric comparison
assert_greater_than() {
    local value1="$1"
    local value2="$2"
    local test_name="${3:-assert_greater_than}"
    
    if (( $(echo "$value1 > $value2" | bc -l) )); then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - $value1 > $value2"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - $value1 is not greater than $value2"
        return $TEST_RESULT_FAIL
    fi
}

# Assert numeric comparison
assert_less_than() {
    local value1="$1"
    local value2="$2"
    local test_name="${3:-assert_less_than}"
    
    if (( $(echo "$value1 < $value2" | bc -l) )); then
        log_message $LOG_LEVEL_DEBUG "PASS: $test_name - $value1 < $value2"
        return $TEST_RESULT_PASS
    else
        log_message $LOG_LEVEL_ERROR "FAIL: $test_name - $value1 is not less than $value2"
        return $TEST_RESULT_FAIL
    fi
}

# =============================================================================
# TEST EXECUTION FRAMEWORK
# =============================================================================

# Run a single test
run_test() {
    local test_function="$1"
    local test_name="${2:-$test_function}"
    local test_mode="${3:-$TEST_MODE_UNIT}"
    
    log_message $LOG_LEVEL_INFO "Running test: $test_name ($test_mode)"
    
    local start_time=$(date +%s.%3N)
    local test_result=$TEST_RESULT_PASS
    local test_output=""
    
    # Capture test output
    test_output=$(eval "$test_function" 2>&1) || test_result=$?
    local end_time=$(date +%s.%3N)
    local duration=$(echo "scale=3; $end_time - $start_time" | bc -l)
    
    # Update test statistics
    ((TEST_STATS["total"]++))
    case $test_result in
        $TEST_RESULT_PASS) ((TEST_STATS["passed"]++)) ;;
        $TEST_RESULT_FAIL) ((TEST_STATS["failed"]++)) ;;
        $TEST_RESULT_SKIP) ((TEST_STATS["skipped"]++)) ;;
        *) ((TEST_STATS["errors"]++)) ;;
    esac
    
    # Log test result
    case $test_result in
        $TEST_RESULT_PASS)
            log_message $LOG_LEVEL_INFO "PASS: $test_name (${duration}s)"
            ;;
        $TEST_RESULT_FAIL)
            log_message $LOG_LEVEL_ERROR "FAIL: $test_name (${duration}s)"
            log_message $LOG_LEVEL_ERROR "Test output: $test_output"
            ;;
        $TEST_RESULT_SKIP)
            log_message $LOG_LEVEL_WARN "SKIP: $test_name (${duration}s)"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "ERROR: $test_name (${duration}s)"
            log_message $LOG_LEVEL_ERROR "Test output: $test_output"
            ;;
    esac
    
    # Write test result to file
    local result_file="$TEST_RESULTS_DIR/${test_name}_${test_mode}.result"
    cat > "$result_file" << EOF
test_name=$test_name
test_mode=$test_mode
test_result=$test_result
duration=$duration
start_time=$start_time
end_time=$end_time
output=$test_output
EOF
    
    return $test_result
}

# Run test suite
run_test_suite() {
    local test_mode="${1:-$TEST_MODE_UNIT}"
    local test_suite="${2:-all}"
    
    log_message $LOG_LEVEL_INFO "Running test suite: $test_suite ($test_mode mode)"
    
    local suite_start_time=$(date +%s.%3N)
    local suite_result=$TEST_RESULT_PASS
    
    # Run tests based on suite
    case "$test_suite" in
        "unit")
            run_unit_tests "$test_mode"
            ;;
        "integration")
            run_integration_tests "$test_mode"
            ;;
        "performance")
            run_performance_tests "$test_mode"
            ;;
        "all")
            run_unit_tests "$test_mode"
            run_integration_tests "$test_mode"
            run_performance_tests "$test_mode"
            ;;
        *)
            log_message $LOG_LEVEL_ERROR "Unknown test suite: $test_suite"
            return $TEST_RESULT_ERROR
            ;;
    esac
    
    local suite_end_time=$(date +%s.%3N)
    local suite_duration=$(echo "scale=3; $suite_end_time - $suite_start_time" | bc -l)
    
    # Generate test report
    generate_test_report "$test_mode" "$suite_duration"
    
    # Check if any tests failed
    if [[ ${TEST_STATS["failed"]} -gt 0 || ${TEST_STATS["errors"]} -gt 0 ]]; then
        suite_result=$TEST_RESULT_FAIL
    fi
    
    log_message $LOG_LEVEL_INFO "Test suite completed: $suite_duration seconds"
    return $suite_result
}

# =============================================================================
# UNIT TESTS
# =============================================================================

# Test basic arithmetic operations
test_arithmetic_operations() {
    log_message $LOG_LEVEL_DEBUG "Testing arithmetic operations"
    
    # Test addition
    local result=$(echo "2 + 3" | bc -l)
    assert_equal "5" "$result" "addition_test"
    
    # Test subtraction
    result=$(echo "10 - 4" | bc -l)
    assert_equal "6" "$result" "subtraction_test"
    
    # Test multiplication
    result=$(echo "3 * 4" | bc -l)
    assert_equal "12" "$result" "multiplication_test"
    
    # Test division
    result=$(echo "15 / 3" | bc -l)
    assert_equal "5" "$result" "division_test"
    
    # Test floating point
    result=$(echo "scale=2; 10 / 3" | bc -l)
    assert_equal "3.33" "$result" "floating_point_test"
}

# Test string operations
test_string_operations() {
    log_message $LOG_LEVEL_DEBUG "Testing string operations"
    
    local test_string="Hello World"
    
    # Test string length
    local length=${#test_string}
    assert_equal "11" "$length" "string_length_test"
    
    # Test substring extraction
    local substring="${test_string:0:5}"
    assert_equal "Hello" "$substring" "substring_test"
    
    # Test string replacement
    local replaced="${test_string//World/Fintech}"
    assert_equal "Hello Fintech" "$replaced" "string_replacement_test"
    
    # Test case conversion
    local upper="${test_string^^}"
    assert_equal "HELLO WORLD" "$upper" "uppercase_test"
    
    local lower="${test_string,,}"
    assert_equal "hello world" "$lower" "lowercase_test"
}

# Test array operations
test_array_operations() {
    log_message $LOG_LEVEL_DEBUG "Testing array operations"
    
    # Test indexed array
    local -a test_array=("apple" "banana" "cherry")
    
    # Test array length
    local array_length=${#test_array[@]}
    assert_equal "3" "$array_length" "array_length_test"
    
    # Test array access
    assert_equal "apple" "${test_array[0]}" "array_access_test"
    assert_equal "banana" "${test_array[1]}" "array_access_test_2"
    assert_equal "cherry" "${test_array[2]}" "array_access_test_3"
    
    # Test associative array
    local -A test_assoc=(
        ["fruit1"]="apple"
        ["fruit2"]="banana"
        ["fruit3"]="cherry"
    )
    
    # Test associative array access
    assert_equal "apple" "${test_assoc[fruit1]}" "assoc_array_access_test"
    assert_equal "banana" "${test_assoc[fruit2]}" "assoc_array_access_test_2"
    assert_equal "cherry" "${test_assoc[fruit3]}" "assoc_array_access_test_3"
}

# Test file operations
test_file_operations() {
    log_message $LOG_LEVEL_DEBUG "Testing file operations"
    
    local test_file="/tmp/test_file_$$"
    local test_content="This is a test file"
    
    # Test file creation
    echo "$test_content" > "$test_file"
    assert_file_exists "$test_file" "file_creation_test"
    
    # Test file reading
    local read_content=$(cat "$test_file")
    assert_equal "$test_content" "$read_content" "file_reading_test"
    
    # Test file writing
    local new_content="This is updated content"
    echo "$new_content" > "$test_file"
    local updated_content=$(cat "$test_file")
    assert_equal "$new_content" "$updated_content" "file_writing_test"
    
    # Test file deletion
    rm "$test_file"
    assert_file_not_exists "$test_file" "file_deletion_test"
}

# Test financial calculations
test_financial_calculations() {
    log_message $LOG_LEVEL_DEBUG "Testing financial calculations"
    
    # Test simple interest calculation
    local principal=1000
    local rate=0.05
    local time=2
    local simple_interest=$(echo "scale=2; $principal * $rate * $time" | bc -l)
    assert_equal "100.00" "$simple_interest" "simple_interest_test"
    
    # Test compound interest calculation
    local compound_interest=$(echo "scale=2; $principal * (1 + $rate) ^ $time - $principal" | bc -l)
    assert_equal "102.50" "$compound_interest" "compound_interest_test"
    
    # Test present value calculation
    local future_value=1100
    local present_value=$(echo "scale=2; $future_value / (1 + $rate) ^ $time" | bc -l)
    assert_equal "997.73" "$present_value" "present_value_test"
    
    # Test portfolio return calculation
    local stock1_return=0.10
    local stock2_return=0.05
    local stock1_weight=0.6
    local stock2_weight=0.4
    local portfolio_return=$(echo "scale=4; $stock1_return * $stock1_weight + $stock2_return * $stock2_weight" | bc -l)
    assert_equal "0.0800" "$portfolio_return" "portfolio_return_test"
}

# Run all unit tests
run_unit_tests() {
    local test_mode="$1"
    
    log_message $LOG_LEVEL_INFO "Running unit tests ($test_mode mode)"
    
    run_test "test_arithmetic_operations" "arithmetic_operations" "$test_mode"
    run_test "test_string_operations" "string_operations" "$test_mode"
    run_test "test_array_operations" "array_operations" "$test_mode"
    run_test "test_file_operations" "file_operations" "$test_mode"
    run_test "test_financial_calculations" "financial_calculations" "$test_mode"
}

# =============================================================================
# INTEGRATION TESTS
# =============================================================================

# Test script integration
test_script_integration() {
    log_message $LOG_LEVEL_DEBUG "Testing script integration"
    
    # Create a test script
    local test_script="/tmp/test_script_$$.sh"
    cat > "$test_script" << 'EOF'
#!/bin/bash
set -euo pipefail

# Test function
calculate_sum() {
    local num1="$1"
    local num2="$2"
    echo "$((num1 + num2))"
}

# Main function
main() {
    local result=$(calculate_sum 5 3)
    echo "Result: $result"
}

# Run if executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
EOF
    
    chmod +x "$test_script"
    
    # Test script execution
    local output=$("$test_script")
    assert_contains "$output" "Result: 8" "script_execution_test"
    
    # Test script exit code
    "$test_script" >/dev/null 2>&1
    assert_equal "0" "$?" "script_exit_code_test"
    
    # Cleanup
    rm "$test_script"
}

# Test data processing pipeline
test_data_processing_pipeline() {
    log_message $LOG_LEVEL_DEBUG "Testing data processing pipeline"
    
    # Create test data
    local test_data_file="/tmp/test_data_$$.csv"
    cat > "$test_data_file" << EOF
symbol,price,volume
AAPL,150.25,1000000
GOOGL,2800.50,500000
MSFT,300.75,750000
EOF
    
    # Test CSV processing
    local processed_data=$(awk -F',' 'NR>1 {print $1 "," $2 "," $3}' "$test_data_file")
    assert_contains "$processed_data" "AAPL,150.25,1000000" "csv_processing_test"
    assert_contains "$processed_data" "GOOGL,2800.50,500000" "csv_processing_test_2"
    assert_contains "$processed_data" "MSFT,300.75,750000" "csv_processing_test_3"
    
    # Test data validation
    local valid_records=$(echo "$processed_data" | wc -l)
    assert_equal "3" "$valid_records" "data_validation_test"
    
    # Cleanup
    rm "$test_data_file"
}

# Test error handling
test_error_handling() {
    log_message $LOG_LEVEL_DEBUG "Testing error handling"
    
    # Test division by zero handling
    local result=$(echo "scale=2; 10 / 0" | bc -l 2>/dev/null || echo "error")
    assert_equal "error" "$result" "division_by_zero_test"
    
    # Test file not found handling
    local output=$(cat /nonexistent_file 2>&1 || echo "file_not_found")
    assert_contains "$output" "file_not_found" "file_not_found_test"
    
    # Test invalid command handling
    local output=$(invalid_command 2>&1 || echo "command_not_found")
    assert_contains "$output" "command_not_found" "invalid_command_test"
}

# Run all integration tests
run_integration_tests() {
    local test_mode="$1"
    
    log_message $LOG_LEVEL_INFO "Running integration tests ($test_mode mode)"
    
    run_test "test_script_integration" "script_integration" "$test_mode"
    run_test "test_data_processing_pipeline" "data_processing_pipeline" "$test_mode"
    run_test "test_error_handling" "error_handling" "$test_mode"
}

# =============================================================================
# PERFORMANCE TESTS
# =============================================================================

# Test performance of arithmetic operations
test_arithmetic_performance() {
    log_message $LOG_LEVEL_DEBUG "Testing arithmetic performance"
    
    local iterations=10000
    local start_time=$(date +%s.%3N)
    
    # Perform arithmetic operations
    for ((i=0; i<iterations; i++)); do
        local result=$(echo "scale=2; $i * 3.14159" | bc -l)
    done
    
    local end_time=$(date +%s.%3N)
    local duration=$(echo "scale=3; $end_time - $start_time" | bc -l)
    local operations_per_second=$(echo "scale=0; $iterations / $duration" | bc -l)
    
    # Assert performance threshold (at least 1000 operations per second)
    assert_greater_than "$operations_per_second" "1000" "arithmetic_performance_test"
    
    log_message $LOG_LEVEL_DEBUG "Arithmetic performance: $operations_per_second ops/sec"
}

# Test performance of string operations
test_string_performance() {
    log_message $LOG_LEVEL_DEBUG "Testing string performance"
    
    local iterations=10000
    local test_string="This is a test string for performance testing"
    local start_time=$(date +%s.%3N)
    
    # Perform string operations
    for ((i=0; i<iterations; i++)); do
        local result="${test_string//test/performance}"
        local length=${#result}
    done
    
    local end_time=$(date +%s.%3N)
    local duration=$(echo "scale=3; $end_time - $start_time" | bc -l)
    local operations_per_second=$(echo "scale=0; $iterations / $duration" | bc -l)
    
    # Assert performance threshold (at least 500 operations per second)
    assert_greater_than "$operations_per_second" "500" "string_performance_test"
    
    log_message $LOG_LEVEL_DEBUG "String performance: $operations_per_second ops/sec"
}

# Test performance of file operations
test_file_performance() {
    log_message $LOG_LEVEL_DEBUG "Testing file performance"
    
    local test_file="/tmp/performance_test_$$.txt"
    local iterations=1000
    local start_time=$(date +%s.%3N)
    
    # Perform file operations
    for ((i=0; i<iterations; i++)); do
        echo "Line $i" >> "$test_file"
    done
    
    local end_time=$(date +%s.%3N)
    local duration=$(echo "scale=3; $end_time - $start_time" | bc -l)
    local operations_per_second=$(echo "scale=0; $iterations / $duration" | bc -l)
    
    # Assert performance threshold (at least 100 operations per second)
    assert_greater_than "$operations_per_second" "100" "file_performance_test"
    
    log_message $LOG_LEVEL_DEBUG "File performance: $operations_per_second ops/sec"
    
    # Cleanup
    rm "$test_file"
}

# Run all performance tests
run_performance_tests() {
    local test_mode="$1"
    
    log_message $LOG_LEVEL_INFO "Running performance tests ($test_mode mode)"
    
    run_test "test_arithmetic_performance" "arithmetic_performance" "$test_mode"
    run_test "test_string_performance" "string_performance" "$test_mode"
    run_test "test_file_performance" "file_performance" "$test_mode"
}

# =============================================================================
# TEST REPORTING
# =============================================================================

# Generate test report
generate_test_report() {
    local test_mode="$1"
    local duration="$2"
    
    local report_file="$TEST_RESULTS_DIR/test_report_${test_mode}_$(date +%Y%m%d_%H%M%S).txt"
    
    log_message $LOG_LEVEL_INFO "Generating test report: $report_file"
    
    cat > "$report_file" << EOF
BASH TESTING FRAMEWORK REPORT
============================
Framework Version: $TEST_FRAMEWORK_VERSION
Test Mode: $test_mode
Duration: ${duration}s
Generated: $(date '+%Y-%m-%d %H:%M:%S')

TEST SUMMARY:
============
Total Tests: ${TEST_STATS[total]}
Passed: ${TEST_STATS[passed]}
Failed: ${TEST_STATS[failed]}
Skipped: ${TEST_STATS[skipped]}
Errors: ${TEST_STATS[errors]}

SUCCESS RATE:
=============
EOF
    
    if [[ ${TEST_STATS[total]} -gt 0 ]]; then
        local success_rate=$(echo "scale=2; ${TEST_STATS[passed]} * 100 / ${TEST_STATS[total]}" | bc -l)
        echo "Success Rate: ${success_rate}%" >> "$report_file"
    else
        echo "Success Rate: N/A" >> "$report_file"
    fi
    
    cat >> "$report_file" << EOF

DETAILED RESULTS:
================
EOF
    
    # Add detailed results for each test
    for result_file in "$TEST_RESULTS_DIR"/*.result; do
        if [[ -f "$result_file" ]]; then
            echo "---" >> "$report_file"
            cat "$result_file" >> "$report_file"
            echo "" >> "$report_file"
        fi
    done
    
    log_message $LOG_LEVEL_INFO "Test report generated: $report_file"
}

# =============================================================================
# MAIN EXECUTION FUNCTION
# =============================================================================

main() {
    local test_mode="${1:-$TEST_MODE_ALL}"
    local test_suite="${2:-all}"
    
    log_message $LOG_LEVEL_INFO "Starting bash testing framework"
    
    # Initialize framework
    init_test_framework
    
    # Run test suite
    run_test_suite "$test_mode" "$test_suite"
    
    # Display final statistics
    log_message $LOG_LEVEL_INFO "Test execution completed:"
    log_message $LOG_LEVEL_INFO "  Total: ${TEST_STATS[total]}"
    log_message $LOG_LEVEL_INFO "  Passed: ${TEST_STATS[passed]}"
    log_message $LOG_LEVEL_INFO "  Failed: ${TEST_STATS[failed]}"
    log_message $LOG_LEVEL_INFO "  Skipped: ${TEST_STATS[skipped]}"
    log_message $LOG_LEVEL_INFO "  Errors: ${TEST_STATS[errors]}"
    
    # Exit with appropriate code
    if [[ ${TEST_STATS[failed]} -gt 0 || ${TEST_STATS[errors]} -gt 0 ]]; then
        log_message $LOG_LEVEL_ERROR "Test execution failed"
        exit 1
    else
        log_message $LOG_LEVEL_INFO "All tests passed successfully"
        exit 0
    fi
}

# =============================================================================
# SCRIPT EXECUTION
# =============================================================================

if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
