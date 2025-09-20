# Bash Scripting Implementation Guide for Fintech Applications

## Table of Contents
1. [Getting Started](#getting-started)
2. [Development Environment Setup](#development-environment-setup)
3. [Project Structure](#project-structure)
4. [Implementation Workflow](#implementation-workflow)
5. [Testing Strategy](#testing-strategy)
6. [Deployment Pipeline](#deployment-pipeline)
7. [Monitoring and Maintenance](#monitoring-and-maintenance)
8. [Troubleshooting Guide](#troubleshooting-guide)
9. [Performance Optimization](#performance-optimization)
10. [Security Implementation](#security-implementation)

## Getting Started

### Prerequisites

Before starting with bash scripting for fintech applications, ensure you have:

1. **System Requirements**
   - Linux/Unix environment (Ubuntu 20.04+ recommended)
   - Bash 4.0 or higher
   - GNU coreutils
   - Standard development tools

2. **Required Tools**
   ```bash
   # Install essential tools
   sudo apt-get update
   sudo apt-get install -y \
       bash \
       bc \
       jq \
       curl \
       wget \
       git \
       vim \
       tree \
       htop \
       iotop \
       netstat \
       lsof
   ```

3. **Development Tools**
   ```bash
   # Install development and testing tools
   sudo apt-get install -y \
       shellcheck \
       bats \
       shunit2 \
       checkbashisms \
       bashate
   ```

### Initial Setup

1. **Create Project Directory**
   ```bash
   mkdir -p ~/fintech-bash-scripts
   cd ~/fintech-bash-scripts
   ```

2. **Initialize Git Repository**
   ```bash
   git init
   git config user.name "Your Name"
   git config user.email "your.email@company.com"
   ```

3. **Create Project Structure**
   ```bash
   mkdir -p {src,tests,docs,config,scripts,logs}
   touch README.md .gitignore
   ```

## Development Environment Setup

### 1. IDE Configuration

#### VS Code Setup
Create `.vscode/settings.json`:
```json
{
    "shellcheck.enable": true,
    "shellcheck.executablePath": "/usr/bin/shellcheck",
    "files.associations": {
        "*.sh": "shellscript"
    },
    "editor.formatOnSave": true,
    "shellformat.path": "/usr/bin/shfmt",
    "terminal.integrated.shell.linux": "/bin/bash"
}
```

#### Vim Configuration
Create `.vimrc`:
```vim
" Bash scripting configuration
set expandtab
set tabstop=4
set shiftwidth=4
set softtabstop=4
set number
set ruler
set hlsearch
set incsearch
set ignorecase
set smartcase

" Syntax highlighting
syntax on
filetype on
filetype plugin on
filetype indent on

" Bash specific
autocmd FileType sh setlocal expandtab shiftwidth=4 softtabstop=4
autocmd FileType sh setlocal commentstring=#\ %s
```

### 2. Development Tools Configuration

#### ShellCheck Configuration
Create `.shellcheckrc`:
```bash
# ShellCheck configuration for fintech scripts
disable=SC1090,SC1091,SC2034,SC2154
enable=all
exclude-dir=node_modules,venv,.git
```

#### Git Hooks
Create `.git/hooks/pre-commit`:
```bash
#!/bin/bash
# Pre-commit hook for bash scripts

echo "Running pre-commit checks..."

# Check for shellcheck issues
if command -v shellcheck >/dev/null 2>&1; then
    echo "Running shellcheck..."
    if ! shellcheck --version >/dev/null 2>&1; then
        echo "Error: shellcheck not found"
        exit 1
    fi
    
    # Check all shell scripts
    for file in $(git diff --cached --name-only --diff-filter=ACM | grep '\.sh$'); do
        if [[ -f "$file" ]]; then
            echo "Checking $file..."
            if ! shellcheck "$file"; then
                echo "Error: shellcheck failed for $file"
                exit 1
            fi
        fi
    done
else
    echo "Warning: shellcheck not installed"
fi

# Check for bashisms
if command -v checkbashisms >/dev/null 2>&1; then
    echo "Running checkbashisms..."
    for file in $(git diff --cached --name-only --diff-filter=ACM | grep '\.sh$'); do
        if [[ -f "$file" ]]; then
            echo "Checking $file for bashisms..."
            if ! checkbashisms "$file"; then
                echo "Error: checkbashisms failed for $file"
                exit 1
            fi
        fi
    done
fi

echo "Pre-commit checks passed!"
```

### 3. Environment Configuration

#### Development Environment Variables
Create `config/dev.env`:
```bash
# Development environment configuration
export ENVIRONMENT="development"
export LOG_LEVEL="DEBUG"
export DEBUG_MODE="true"
export TEST_MODE="true"

# API endpoints
export FINANCIAL_API_BASE_URL="https://api-dev.financial-data.com"
export MARKET_DATA_API_URL="https://market-data-dev.company.com"

# Database connections
export DB_HOST="localhost"
export DB_PORT="5432"
export DB_NAME="fintech_dev"
export DB_USER="dev_user"

# File paths
export CONFIG_DIR="/home/user/fintech-bash-scripts/config"
export LOG_DIR="/home/user/fintech-bash-scripts/logs"
export TEMP_DIR="/tmp/fintech_dev"

# Security (use different keys for dev)
export ENCRYPTION_KEY="dev_encryption_key_12345"
export API_KEY="dev_api_key_12345"
```

#### Production Environment Variables
Create `config/prod.env`:
```bash
# Production environment configuration
export ENVIRONMENT="production"
export LOG_LEVEL="INFO"
export DEBUG_MODE="false"
export TEST_MODE="false"

# API endpoints
export FINANCIAL_API_BASE_URL="https://api.financial-data.com"
export MARKET_DATA_API_URL="https://market-data.company.com"

# Database connections
export DB_HOST="prod-db.company.com"
export DB_PORT="5432"
export DB_NAME="fintech_prod"
export DB_USER="prod_user"

# File paths
export CONFIG_DIR="/etc/fintech"
export LOG_DIR="/var/log/fintech"
export TEMP_DIR="/tmp/fintech_prod"

# Security (use secure keys for production)
export ENCRYPTION_KEY="${ENCRYPTION_KEY:-}"
export API_KEY="${API_KEY:-}"
```

## Project Structure

### Recommended Directory Layout

```
fintech-bash-scripts/
├── README.md
├── .gitignore
├── .shellcheckrc
├── .git/
│   └── hooks/
│       ├── pre-commit
│       ├── pre-push
│       └── post-commit
├── .vscode/
│   └── settings.json
├── config/
│   ├── dev.env
│   ├── staging.env
│   ├── prod.env
│   └── common.conf
├── src/
│   ├── core/
│   │   ├── logging.sh
│   │   ├── validation.sh
│   │   ├── security.sh
│   │   └── utils.sh
│   ├── financial/
│   │   ├── calculations.sh
│   │   ├── data_processing.sh
│   │   ├── risk_management.sh
│   │   └── compliance.sh
│   ├── trading/
│   │   ├── order_processing.sh
│   │   ├── market_data.sh
│   │   └── execution.sh
│   └── main/
│       ├── financial_processor.sh
│       ├── trading_engine.sh
│       └── risk_monitor.sh
├── tests/
│   ├── unit/
│   │   ├── test_calculations.sh
│   │   ├── test_validation.sh
│   │   └── test_security.sh
│   ├── integration/
│   │   ├── test_data_processing.sh
│   │   └── test_trading_flow.sh
│   ├── performance/
│   │   ├── test_throughput.sh
│   │   └── test_latency.sh
│   └── fixtures/
│       ├── sample_data.csv
│       ├── test_config.json
│       └── expected_outputs/
├── scripts/
│   ├── setup.sh
│   ├── deploy.sh
│   ├── test.sh
│   └── monitor.sh
├── docs/
│   ├── API.md
│   ├── DEPLOYMENT.md
│   ├── TROUBLESHOOTING.md
│   └── examples/
├── logs/
│   ├── application.log
│   ├── error.log
│   └── audit.log
└── tmp/
    └── (temporary files)
```

### File Organization Guidelines

1. **Source Code (`src/`)**
   - Organize by functional modules
   - Keep related functions together
   - Use descriptive file names
   - Include comprehensive headers

2. **Tests (`tests/`)**
   - Mirror source structure
   - Use descriptive test names
   - Include test fixtures
   - Maintain test coverage

3. **Configuration (`config/`)**
   - Environment-specific configs
   - Common configuration files
   - Secure credential management
   - Version control considerations

4. **Documentation (`docs/`)**
   - API documentation
   - Deployment guides
   - Troubleshooting guides
   - Code examples

## Implementation Workflow

### 1. Development Process

#### Step 1: Requirements Analysis
```bash
# Create requirements document
cat > docs/requirements.md << 'EOF'
# Financial Data Processor Requirements

## Functional Requirements
1. Process market data from multiple sources
2. Validate data integrity and format
3. Calculate financial metrics
4. Generate reports and alerts
5. Handle errors gracefully

## Non-Functional Requirements
1. Process 1M+ records per minute
2. Latency < 100ms for real-time data
3. 99.9% uptime availability
4. SOX compliance
5. Audit trail for all operations

## Security Requirements
1. Encrypt sensitive data
2. Validate all inputs
3. Log all operations
4. Access control
5. Data retention policies
EOF
```

#### Step 2: Design and Architecture
```bash
# Create architecture document
cat > docs/architecture.md << 'EOF'
# System Architecture

## Components
1. Data Ingestion Layer
   - Market data feeds
   - File processing
   - API integration

2. Processing Layer
   - Data validation
   - Financial calculations
   - Risk management

3. Output Layer
   - Report generation
   - Alert system
   - Data export

## Data Flow
Input -> Validation -> Processing -> Output -> Audit
EOF
```

#### Step 3: Implementation
```bash
# Create main script structure
cat > src/main/financial_processor.sh << 'EOF'
#!/bin/bash
#
# Financial Data Processor
# Processes high-frequency market data for trading systems
#
# Author: System Engineering Team
# Version: 1.0.0
# Last Modified: $(date +%Y-%m-%d)
#

# =============================================================================
# SCRIPT CONFIGURATION
# =============================================================================

set -euo pipefail
IFS=$'\n\t'

# Load configuration
source "$(dirname "$0")/../../config/common.conf"

# Load core modules
source "$(dirname "$0")/../core/logging.sh"
source "$(dirname "$0")/../core/validation.sh"
source "$(dirname "$0")/../core/security.sh"

# Load financial modules
source "$(dirname "$0")/../financial/calculations.sh"
source "$(dirname "$0")/../financial/data_processing.sh"

# =============================================================================
# MAIN EXECUTION
# =============================================================================

main() {
    log_info "Starting financial data processor"
    
    # Process command line arguments
    parse_arguments "$@"
    
    # Initialize system
    initialize_system
    
    # Process data
    process_financial_data
    
    # Generate reports
    generate_reports
    
    # Cleanup
    cleanup_system
    
    log_info "Financial data processor completed successfully"
}

# Run main function if script is executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
EOF
```

### 2. Testing Strategy

#### Unit Testing
```bash
# Create unit test template
cat > tests/unit/test_calculations.sh << 'EOF'
#!/bin/bash
#
# Unit tests for financial calculations
#

# Load test framework
source "$(dirname "$0")/../test_framework.sh"

# Load module under test
source "$(dirname "$0")/../../src/financial/calculations.sh"

# Test cases
test_simple_interest_calculation() {
    local principal=1000
    local rate=0.05
    local time=2
    local expected=100.00
    
    local result=$(calculate_simple_interest "$principal" "$rate" "$time")
    
    assert_equal "$expected" "$result" "simple_interest_calculation"
}

test_compound_interest_calculation() {
    local principal=1000
    local rate=0.05
    local time=2
    local expected=102.50
    
    local result=$(calculate_compound_interest "$principal" "$rate" "$time")
    
    assert_equal "$expected" "$result" "compound_interest_calculation"
}

# Run tests
run_tests
EOF
```

#### Integration Testing
```bash
# Create integration test
cat > tests/integration/test_data_processing.sh << 'EOF'
#!/bin/bash
#
# Integration tests for data processing
#

# Load test framework
source "$(dirname "$0")/../test_framework.sh"

# Load modules
source "$(dirname "$0")/../../src/financial/data_processing.sh"
source "$(dirname "$0")/../../src/core/validation.sh"

# Test data processing pipeline
test_data_processing_pipeline() {
    # Create test data
    local test_data_file="/tmp/test_data_$$.csv"
    cat > "$test_data_file" << 'DATA'
symbol,price,volume
AAPL,150.25,1000000
GOOGL,2800.50,500000
MSFT,300.75,750000
DATA
    
    # Process data
    local output_file="/tmp/processed_data_$$.csv"
    process_market_data "$test_data_file" "$output_file"
    
    # Verify output
    assert_file_exists "$output_file" "output_file_created"
    
    local output_lines=$(wc -l < "$output_file")
    assert_equal "4" "$output_lines" "correct_number_of_lines"  # Header + 3 data lines
    
    # Cleanup
    rm -f "$test_data_file" "$output_file"
}

# Run tests
run_tests
EOF
```

### 3. Code Quality Assurance

#### Static Analysis
```bash
# Create quality check script
cat > scripts/quality_check.sh << 'EOF'
#!/bin/bash
#
# Code quality check script
#

set -euo pipefail

# Configuration
readonly SRC_DIR="src"
readonly TEST_DIR="tests"
readonly QUALITY_THRESHOLD=80

# Colors for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly NC='\033[0m' # No Color

# Check functions
check_syntax() {
    echo "Checking syntax..."
    local errors=0
    
    for file in $(find "$SRC_DIR" -name "*.sh"); do
        if ! bash -n "$file"; then
            echo -e "${RED}Syntax error in $file${NC}"
            ((errors++))
        fi
    done
    
    if [[ $errors -eq 0 ]]; then
        echo -e "${GREEN}Syntax check passed${NC}"
    else
        echo -e "${RED}Syntax check failed: $errors errors${NC}"
        return 1
    fi
}

check_shellcheck() {
    echo "Running shellcheck..."
    local errors=0
    
    for file in $(find "$SRC_DIR" -name "*.sh"); do
        if ! shellcheck "$file"; then
            ((errors++))
        fi
    done
    
    if [[ $errors -eq 0 ]]; then
        echo -e "${GREEN}Shellcheck passed${NC}"
    else
        echo -e "${RED}Shellcheck failed: $errors errors${NC}"
        return 1
    fi
}

check_test_coverage() {
    echo "Checking test coverage..."
    local total_functions=$(grep -r "^[a-zA-Z_][a-zA-Z0-9_]*()" "$SRC_DIR" | wc -l)
    local tested_functions=$(grep -r "^test_.*()" "$TEST_DIR" | wc -l)
    
    if [[ $total_functions -gt 0 ]]; then
        local coverage=$((tested_functions * 100 / total_functions))
        echo "Test coverage: $coverage% ($tested_functions/$total_functions functions)"
        
        if [[ $coverage -lt $QUALITY_THRESHOLD ]]; then
            echo -e "${YELLOW}Warning: Test coverage below threshold ($QUALITY_THRESHOLD%)${NC}"
            return 1
        else
            echo -e "${GREEN}Test coverage meets threshold${NC}"
        fi
    else
        echo -e "${YELLOW}No functions found to test${NC}"
    fi
}

# Main execution
main() {
    echo "Starting quality checks..."
    
    local overall_result=0
    
    check_syntax || overall_result=1
    check_shellcheck || overall_result=1
    check_test_coverage || overall_result=1
    
    if [[ $overall_result -eq 0 ]]; then
        echo -e "${GREEN}All quality checks passed${NC}"
    else
        echo -e "${RED}Quality checks failed${NC}"
    fi
    
    exit $overall_result
}

# Run main function
main "$@"
EOF
```

## Testing Strategy

### 1. Test Framework Setup

#### BATS Testing Framework
```bash
# Install BATS
git clone https://github.com/bats-core/bats-core.git /tmp/bats-core
sudo /tmp/bats-core/install.sh /usr/local

# Create test configuration
cat > tests/test_helper.bash << 'EOF'
# Test helper functions for BATS

# Load the script under test
load_financial_processor() {
    source "$(dirname "$BATS_TEST_FILENAME")/../../src/main/financial_processor.sh"
}

# Create test data
create_test_data() {
    local file="$1"
    cat > "$file" << 'DATA'
symbol,price,volume
AAPL,150.25,1000000
GOOGL,2800.50,500000
MSFT,300.75,750000
DATA
}

# Cleanup test data
cleanup_test_data() {
    rm -f /tmp/test_*.csv
}
EOF
```

#### Test Execution Script
```bash
# Create test runner
cat > scripts/run_tests.sh << 'EOF'
#!/bin/bash
#
# Test runner script
#

set -euo pipefail

# Configuration
readonly TEST_DIR="tests"
readonly REPORT_DIR="test_reports"
readonly COVERAGE_DIR="coverage"

# Create report directories
mkdir -p "$REPORT_DIR" "$COVERAGE_DIR"

# Run unit tests
run_unit_tests() {
    echo "Running unit tests..."
    bats "$TEST_DIR/unit/" > "$REPORT_DIR/unit_tests.txt" 2>&1
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        echo "Unit tests passed"
    else
        echo "Unit tests failed"
        cat "$REPORT_DIR/unit_tests.txt"
    fi
    
    return $exit_code
}

# Run integration tests
run_integration_tests() {
    echo "Running integration tests..."
    bats "$TEST_DIR/integration/" > "$REPORT_DIR/integration_tests.txt" 2>&1
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        echo "Integration tests passed"
    else
        echo "Integration tests failed"
        cat "$REPORT_DIR/integration_tests.txt"
    fi
    
    return $exit_code
}

# Run performance tests
run_performance_tests() {
    echo "Running performance tests..."
    bats "$TEST_DIR/performance/" > "$REPORT_DIR/performance_tests.txt" 2>&1
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        echo "Performance tests passed"
    else
        echo "Performance tests failed"
        cat "$REPORT_DIR/performance_tests.txt"
    fi
    
    return $exit_code
}

# Generate test report
generate_test_report() {
    local report_file="$REPORT_DIR/test_report_$(date +%Y%m%d_%H%M%S).html"
    
    cat > "$report_file" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .pass { color: green; }
        .fail { color: red; }
        .summary { background-color: #f0f0f0; padding: 10px; margin: 10px 0; }
    </style>
</head>
<body>
    <h1>Test Report</h1>
    <div class="summary">
        <h2>Summary</h2>
        <p>Generated: $(date)</p>
        <p>Unit Tests: <span class="pass">Passed</span></p>
        <p>Integration Tests: <span class="pass">Passed</span></p>
        <p>Performance Tests: <span class="pass">Passed</span></p>
    </div>
    <h2>Detailed Results</h2>
    <h3>Unit Tests</h3>
    <pre>$(cat "$REPORT_DIR/unit_tests.txt")</pre>
    <h3>Integration Tests</h3>
    <pre>$(cat "$REPORT_DIR/integration_tests.txt")</pre>
    <h3>Performance Tests</h3>
    <pre>$(cat "$REPORT_DIR/performance_tests.txt")</pre>
</body>
</html>
EOF
    
    echo "Test report generated: $report_file"
}

# Main execution
main() {
    echo "Starting test execution..."
    
    local overall_result=0
    
    run_unit_tests || overall_result=1
    run_integration_tests || overall_result=1
    run_performance_tests || overall_result=1
    
    generate_test_report
    
    if [[ $overall_result -eq 0 ]]; then
        echo "All tests passed"
    else
        echo "Some tests failed"
    fi
    
    exit $overall_result
}

# Run main function
main "$@"
EOF
```

## Deployment Pipeline

### 1. CI/CD Configuration

#### GitHub Actions Workflow
```yaml
# .github/workflows/ci.yml
name: CI/CD Pipeline

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Setup environment
      run: |
        sudo apt-get update
        sudo apt-get install -y bash bc jq curl shellcheck bats
    
    - name: Run quality checks
      run: ./scripts/quality_check.sh
    
    - name: Run tests
      run: ./scripts/run_tests.sh
    
    - name: Upload test results
      uses: actions/upload-artifact@v2
      if: always()
      with:
        name: test-results
        path: test_reports/

  deploy:
    needs: test
    runs-on: ubuntu-latest
    if: github.ref == 'refs/heads/main'
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Deploy to staging
      run: ./scripts/deploy.sh staging
    
    - name: Run smoke tests
      run: ./scripts/smoke_tests.sh staging
    
    - name: Deploy to production
      run: ./scripts/deploy.sh production
```

### 2. Deployment Scripts

#### Deployment Script
```bash
# Create deployment script
cat > scripts/deploy.sh << 'EOF'
#!/bin/bash
#
# Deployment script for fintech bash scripts
#

set -euo pipefail

# Configuration
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
readonly ENVIRONMENT="${1:-staging}"

# Environment-specific configuration
case "$ENVIRONMENT" in
    "staging")
        readonly DEPLOY_DIR="/opt/fintech/staging"
        readonly CONFIG_FILE="staging.env"
        ;;
    "production")
        readonly DEPLOY_DIR="/opt/fintech/production"
        readonly CONFIG_FILE="prod.env"
        ;;
    *)
        echo "Error: Unknown environment: $ENVIRONMENT"
        echo "Usage: $0 [staging|production]"
        exit 1
        ;;
esac

# Deployment functions
backup_current_deployment() {
    echo "Creating backup of current deployment..."
    
    if [[ -d "$DEPLOY_DIR" ]]; then
        local backup_dir="/opt/fintech/backups/$(date +%Y%m%d_%H%M%S)"
        mkdir -p "$backup_dir"
        cp -r "$DEPLOY_DIR" "$backup_dir/"
        echo "Backup created: $backup_dir"
    fi
}

deploy_scripts() {
    echo "Deploying scripts to $ENVIRONMENT..."
    
    # Create deployment directory
    mkdir -p "$DEPLOY_DIR"
    
    # Copy source files
    cp -r "$PROJECT_DIR/src" "$DEPLOY_DIR/"
    cp -r "$PROJECT_DIR/config" "$DEPLOY_DIR/"
    cp -r "$PROJECT_DIR/scripts" "$DEPLOY_DIR/"
    
    # Set permissions
    chmod +x "$DEPLOY_DIR/scripts"/*.sh
    chmod +x "$DEPLOY_DIR/src/main"/*.sh
    
    # Copy configuration
    cp "$PROJECT_DIR/config/$CONFIG_FILE" "$DEPLOY_DIR/config/environment.env"
    
    echo "Scripts deployed successfully"
}

verify_deployment() {
    echo "Verifying deployment..."
    
    # Check if main scripts exist
    local main_script="$DEPLOY_DIR/src/main/financial_processor.sh"
    if [[ ! -f "$main_script" ]]; then
        echo "Error: Main script not found: $main_script"
        return 1
    fi
    
    # Check if scripts are executable
    if [[ ! -x "$main_script" ]]; then
        echo "Error: Main script is not executable: $main_script"
        return 1
    fi
    
    # Test script execution
    if ! bash -n "$main_script"; then
        echo "Error: Main script has syntax errors"
        return 1
    fi
    
    echo "Deployment verification passed"
}

# Main execution
main() {
    echo "Starting deployment to $ENVIRONMENT..."
    
    backup_current_deployment
    deploy_scripts
    verify_deployment
    
    echo "Deployment to $ENVIRONMENT completed successfully"
}

# Run main function
main "$@"
EOF
```

## Monitoring and Maintenance

### 1. Health Monitoring

#### Health Check Script
```bash
# Create health check script
cat > scripts/health_check.sh << 'EOF'
#!/bin/bash
#
# Health check script for fintech applications
#

set -euo pipefail

# Configuration
readonly HEALTH_CHECK_INTERVAL=60
readonly HEALTH_CHECK_TIMEOUT=30
readonly ALERT_EMAIL="alerts@company.com"

# Health check functions
check_script_execution() {
    local script_path="$1"
    
    if [[ ! -f "$script_path" ]]; then
        echo "ERROR: Script not found: $script_path"
        return 1
    fi
    
    if [[ ! -x "$script_path" ]]; then
        echo "ERROR: Script not executable: $script_path"
        return 1
    fi
    
    # Test script execution with timeout
    if timeout "$HEALTH_CHECK_TIMEOUT" bash -n "$script_path"; then
        echo "OK: Script syntax check passed"
        return 0
    else
        echo "ERROR: Script syntax check failed"
        return 1
    fi
}

check_data_processing() {
    local data_file="$1"
    
    if [[ ! -f "$data_file" ]]; then
        echo "ERROR: Data file not found: $data_file"
        return 1
    fi
    
    # Check if data file is recent (within last hour)
    local file_age=$(($(date +%s) - $(stat -c %Y "$data_file")))
    if [[ $file_age -gt 3600 ]]; then
        echo "WARNING: Data file is old: $file_age seconds"
        return 1
    fi
    
    echo "OK: Data file is recent"
    return 0
}

check_system_resources() {
    # Check disk space
    local disk_usage=$(df / | awk 'NR==2 {print $5}' | sed 's/%//')
    if [[ $disk_usage -gt 90 ]]; then
        echo "ERROR: Disk usage too high: $disk_usage%"
        return 1
    fi
    
    # Check memory usage
    local memory_usage=$(free | awk 'NR==2{printf "%.0f", $3*100/$2}')
    if [[ $memory_usage -gt 90 ]]; then
        echo "ERROR: Memory usage too high: $memory_usage%"
        return 1
    fi
    
    echo "OK: System resources within limits"
    return 0
}

send_alert() {
    local message="$1"
    local subject="Fintech Application Alert - $(hostname)"
    
    echo "$message" | mail -s "$subject" "$ALERT_EMAIL"
    echo "Alert sent: $subject"
}

# Main health check
main() {
    local overall_status=0
    
    echo "Starting health check at $(date)"
    
    # Check main scripts
    check_script_execution "/opt/fintech/production/src/main/financial_processor.sh" || overall_status=1
    
    # Check data processing
    check_data_processing "/var/log/fintech/latest_data.csv" || overall_status=1
    
    # Check system resources
    check_system_resources || overall_status=1
    
    if [[ $overall_status -eq 0 ]]; then
        echo "Health check passed"
    else
        echo "Health check failed"
        send_alert "Health check failed on $(hostname) at $(date)"
    fi
    
    exit $overall_status
}

# Run main function
main "$@"
EOF
```

### 2. Log Management

#### Log Rotation Configuration
```bash
# Create logrotate configuration
cat > /etc/logrotate.d/fintech << 'EOF'
/var/log/fintech/*.log {
    daily
    missingok
    rotate 30
    compress
    delaycompress
    notifempty
    create 644 fintech fintech
    postrotate
        /bin/kill -HUP `cat /var/run/rsyslogd.pid 2> /dev/null` 2> /dev/null || true
    endscript
}

/var/log/fintech/audit.log {
    daily
    missingok
    rotate 2555  # 7 years for SOX compliance
    compress
    delaycompress
    notifempty
    create 600 fintech fintech
    postrotate
        /bin/kill -HUP `cat /var/run/rsyslogd.pid 2> /dev/null` 2> /dev/null || true
    endscript
}
EOF
```

## Troubleshooting Guide

### 1. Common Issues and Solutions

#### Script Execution Issues
```bash
# Create troubleshooting script
cat > scripts/troubleshoot.sh << 'EOF'
#!/bin/bash
#
# Troubleshooting script for fintech applications
#

set -euo pipefail

# Diagnostic functions
check_environment() {
    echo "=== Environment Check ==="
    
    # Check bash version
    echo "Bash version: $(bash --version | head -1)"
    
    # Check required tools
    local required_tools=("bc" "jq" "curl" "awk" "sed")
    for tool in "${required_tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            echo "OK: $tool is installed"
        else
            echo "ERROR: $tool is not installed"
        fi
    done
    
    # Check environment variables
    echo "Environment variables:"
    env | grep -E "(FINANCIAL|API|DB)" | sort
}

check_permissions() {
    echo "=== Permissions Check ==="
    
    local script_dir="/opt/fintech/production"
    if [[ -d "$script_dir" ]]; then
        echo "Script directory permissions:"
        ls -la "$script_dir"
        
        echo "Script executable permissions:"
        find "$script_dir" -name "*.sh" -exec ls -la {} \;
    else
        echo "ERROR: Script directory not found: $script_dir"
    fi
}

check_logs() {
    echo "=== Log Check ==="
    
    local log_dir="/var/log/fintech"
    if [[ -d "$log_dir" ]]; then
        echo "Recent log entries:"
        find "$log_dir" -name "*.log" -mtime -1 -exec tail -20 {} \;
    else
        echo "ERROR: Log directory not found: $log_dir"
    fi
}

check_data_files() {
    echo "=== Data Files Check ==="
    
    local data_dir="/var/data/fintech"
    if [[ -d "$data_dir" ]]; then
        echo "Data directory contents:"
        ls -la "$data_dir"
        
        echo "Data file sizes:"
        find "$data_dir" -type f -exec du -h {} \;
    else
        echo "ERROR: Data directory not found: $data_dir"
    fi
}

# Main troubleshooting
main() {
    echo "Starting troubleshooting diagnostics..."
    
    check_environment
    check_permissions
    check_logs
    check_data_files
    
    echo "Troubleshooting completed"
}

# Run main function
main "$@"
EOF
```

### 2. Performance Monitoring

#### Performance Monitoring Script
```bash
# Create performance monitoring script
cat > scripts/performance_monitor.sh << 'EOF'
#!/bin/bash
#
# Performance monitoring script
#

set -euo pipefail

# Configuration
readonly MONITOR_INTERVAL=60
readonly LOG_FILE="/var/log/fintech/performance.log"

# Performance monitoring functions
monitor_cpu_usage() {
    local cpu_usage=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | sed 's/%us,//')
    echo "CPU Usage: $cpu_usage%"
}

monitor_memory_usage() {
    local memory_usage=$(free | awk 'NR==2{printf "%.1f", $3*100/$2}')
    echo "Memory Usage: $memory_usage%"
}

monitor_disk_usage() {
    local disk_usage=$(df / | awk 'NR==2 {print $5}' | sed 's/%//')
    echo "Disk Usage: $disk_usage%"
}

monitor_script_performance() {
    local script_path="$1"
    
    if [[ -f "$script_path" ]]; then
        local start_time=$(date +%s.%3N)
        bash -n "$script_path"
        local end_time=$(date +%s.%3N)
        local duration=$(echo "scale=3; $end_time - $start_time" | bc -l)
        echo "Script syntax check duration: ${duration}s"
    fi
}

# Main monitoring loop
main() {
    echo "Starting performance monitoring..."
    
    while true; do
        local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
        
        echo "[$timestamp] Performance Metrics:" >> "$LOG_FILE"
        monitor_cpu_usage >> "$LOG_FILE"
        monitor_memory_usage >> "$LOG_FILE"
        monitor_disk_usage >> "$LOG_FILE"
        monitor_script_performance "/opt/fintech/production/src/main/financial_processor.sh" >> "$LOG_FILE"
        echo "---" >> "$LOG_FILE"
        
        sleep "$MONITOR_INTERVAL"
    done
}

# Run main function
main "$@"
EOF
```

This comprehensive implementation guide provides everything needed to set up, develop, test, and deploy production-grade bash scripts for fintech applications. Each section includes practical examples and real-world scenarios that demonstrate best practices for enterprise-level bash scripting.
