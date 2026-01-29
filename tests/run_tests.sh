#!/bin/bash

# ==============================================================================
# ft_ping Automated Test Suite
# ==============================================================================
# This script automatically runs all test cases from TESTING.md and logs results
# Usage: sudo ./run_tests.sh [--compare]
#   --compare: Also run comparison tests with system ping
# ==============================================================================

set -o pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
FT_PING="$PROJECT_ROOT/build/ft_ping"
LOG_DIR="$SCRIPT_DIR/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="$LOG_DIR/test_results_$TIMESTAMP.log"
SUMMARY_FILE="$LOG_DIR/test_summary_$TIMESTAMP.txt"

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

# Options
RUN_COMPARE=false

# ==============================================================================
# Utility Functions
# ==============================================================================

log() {
    local message="$1"
    echo -e "$message"
    echo -e "$(echo "$message" | sed 's/\x1B\[[0-9;]*m//g')" >> "$LOG_FILE"
}

log_header() {
    local header="$1"
    log ""
    log "${BOLD}${BLUE}============================================================${NC}"
    log "${BOLD}${BLUE}  $header${NC}"
    log "${BOLD}${BLUE}============================================================${NC}"
    log ""
}

log_subheader() {
    local subheader="$1"
    log ""
    log "${CYAN}------------------------------------------------------------${NC}"
    log "${CYAN}  $subheader${NC}"
    log "${CYAN}------------------------------------------------------------${NC}"
}

log_test_start() {
    local test_name="$1"
    log ""
    log "${YELLOW}▶ Running: $test_name${NC}"
}

log_pass() {
    local message="$1"
    log "${GREEN}✓ PASS: $message${NC}"
    ((PASSED_TESTS++))
}

log_fail() {
    local message="$1"
    log "${RED}✗ FAIL: $message${NC}"
    ((FAILED_TESTS++))
}

log_skip() {
    local message="$1"
    log "${YELLOW}⊘ SKIP: $message${NC}"
    ((SKIPPED_TESTS++))
}

log_info() {
    local message="$1"
    log "${BLUE}ℹ INFO: $message${NC}"
}

log_output() {
    local label="$1"
    local output="$2"
    log "  ${BOLD}$label:${NC}"
    echo "$output" | head -20 | while IFS= read -r line; do
        log "    $line"
    done
    local total_lines=$(echo "$output" | wc -l)
    if [ "$total_lines" -gt 20 ]; then
        log "    ... ($(($total_lines - 20)) more lines)"
    fi
}

# Check if we have root privileges
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo -e "${RED}Error: This script requires root privileges.${NC}"
        echo -e "Please run with: ${BOLD}sudo $0${NC}"
        exit 1
    fi
}

# Check if ft_ping binary exists
check_binary() {
    if [ ! -x "$FT_PING" ]; then
        echo -e "${RED}Error: ft_ping binary not found at $FT_PING${NC}"
        echo -e "Please build the project first with: ${BOLD}make${NC}"
        exit 1
    fi
}

# Create log directory
setup_logs() {
    mkdir -p "$LOG_DIR"
    touch "$LOG_FILE"
    touch "$SUMMARY_FILE"
    log_info "Log file: $LOG_FILE"
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --compare)
                RUN_COMPARE=true
                shift
                ;;
            --help|-h)
                echo "Usage: sudo $0 [--compare]"
                echo "  --compare: Also run comparison tests with system ping"
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                exit 1
                ;;
        esac
    done
}

# Run a test command with timeout
run_test() {
    local cmd="$1"
    local timeout_sec="${2:-5}"
    local output
    local exit_code
    
    output=$(timeout "$timeout_sec" bash -c "$cmd" 2>&1)
    exit_code=$?
    
    echo "$output"
    return $exit_code
}

# ==============================================================================
# Test Functions
# ==============================================================================

# 1. Basic Functionality Tests
test_basic_functionality() {
    log_header "1. Basic Functionality Tests"
    
    # 1.1 Localhost Ping
    log_subheader "1.1 Localhost Ping"
    ((TOTAL_TESTS++))
    log_test_start "Ping localhost with count limit"
    
    output=$(run_test "$FT_PING -c 3 localhost" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ] && echo "$output" | grep -q "icmp_seq"; then
        log_pass "Localhost ping works correctly"
    else
        log_fail "Localhost ping failed (exit code: $exit_code)"
    fi
    
    # 1.2 Internet Host Ping (google.com)
    log_subheader "1.2 Internet Host Ping"
    ((TOTAL_TESTS++))
    log_test_start "Ping google.com"
    
    output=$(run_test "$FT_PING -c 3 google.com" 15)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ] && echo "$output" | grep -q "icmp_seq"; then
        log_pass "Internet host ping works correctly"
    else
        log_fail "Internet host ping failed (exit code: $exit_code)"
    fi
}

# 2. Command-Line Options Tests
test_command_line_options() {
    log_header "2. Command-Line Options Tests"
    
    # 2.1 Count Option (-c)
    log_subheader "2.1 Count Option (-c)"
    ((TOTAL_TESTS++))
    log_test_start "Test -c 3 option"
    
    output=$(run_test "$FT_PING -c 3 localhost" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    packet_count=$(echo "$output" | grep -c "icmp_seq" || echo "0")
    if [ "$packet_count" -eq 3 ]; then
        log_pass "Count option works correctly (sent exactly 3 packets)"
    else
        log_fail "Count option failed (expected 3 packets, got $packet_count)"
    fi
    
    # 2.2 TTL Option (--ttl)
    log_subheader "2.2 TTL Option (--ttl)"
    ((TOTAL_TESTS++))
    log_test_start "Test --ttl 32 option"
    
    output=$(run_test "$FT_PING -c 2 --ttl 32 localhost" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ]; then
        log_pass "TTL option works correctly"
    else
        log_fail "TTL option failed (exit code: $exit_code)"
    fi
    
    # 2.3 Timeout Option (-w)
    log_subheader "2.3 Timeout Option (-w)"
    ((TOTAL_TESTS++))
    log_test_start "Test -w 2 option"
    
    start_time=$(date +%s)
    output=$(run_test "$FT_PING -w 2 localhost" 5)
    end_time=$(date +%s)
    elapsed=$((end_time - start_time))
    log_output "Output" "$output"
    
    if [ $elapsed -le 4 ]; then
        log_pass "Timeout option works correctly (elapsed: ${elapsed}s)"
    else
        log_fail "Timeout option may not be working correctly (elapsed: ${elapsed}s)"
    fi
    
    # 2.4 Interval Option (-i)
    log_subheader "2.4 Interval Option (-i)"
    ((TOTAL_TESTS++))
    log_test_start "Test -i 1 option (1 second interval)"
    
    start_time=$(date +%s)
    output=$(run_test "$FT_PING -c 3 -i 1 localhost" 10)
    end_time=$(date +%s)
    elapsed=$((end_time - start_time))
    log_output "Output" "$output"
    
    # With 3 packets and 1 second interval, should take ~2-3 seconds
    if [ $elapsed -ge 2 ]; then
        log_pass "Interval option works correctly (elapsed: ${elapsed}s)"
    else
        log_fail "Interval option may not be working correctly (elapsed: ${elapsed}s, expected >= 2s)"
    fi
    
    # 2.5 Verbose Output (-v)
    log_subheader "2.5 Verbose Output (-v)"
    ((TOTAL_TESTS++))
    log_test_start "Test -v option"
    
    output=$(run_test "$FT_PING -c 2 -v localhost" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ]; then
        log_pass "Verbose option works correctly"
    else
        log_fail "Verbose option failed (exit code: $exit_code)"
    fi
    
    # 2.6 Version Display (-V)
    log_subheader "2.6 Version Display (-V)"
    ((TOTAL_TESTS++))
    log_test_start "Test -V option"
    
    output=$($FT_PING -V 2>&1)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ] || echo "$output" | grep -qi "version\|ft_ping"; then
        log_pass "Version option works correctly"
    else
        log_fail "Version option failed"
    fi
    
    # 2.7 Help Message (-? or --help)
    log_subheader "2.7 Help Message"
    ((TOTAL_TESTS++))
    log_test_start "Test --help option"
    
    output=$($FT_PING --help 2>&1)
    exit_code=$?
    log_output "Output" "$output"
    
    if echo "$output" | grep -qi "usage\|help\|options"; then
        log_pass "Help option works correctly"
    else
        log_fail "Help option failed"
    fi
}

# 3. Error Handling Tests
test_error_handling() {
    log_header "3. Error Handling Tests"
    
    # 3.1 Invalid Hostname
    log_subheader "3.1 Invalid Hostname"
    ((TOTAL_TESTS++))
    log_test_start "Test with invalid hostname"
    
    output=$(run_test "$FT_PING -c 1 nonexistenthostname123456xyz" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -ne 0 ]; then
        log_pass "Invalid hostname handled correctly (exit code: $exit_code)"
    else
        log_fail "Invalid hostname should return non-zero exit code"
    fi
    
    # 3.2 Invalid Option
    log_subheader "3.2 Invalid Option"
    ((TOTAL_TESTS++))
    log_test_start "Test with invalid option -x"
    
    output=$($FT_PING -x localhost 2>&1)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -ne 0 ]; then
        log_pass "Invalid option handled correctly (exit code: $exit_code)"
    else
        log_fail "Invalid option should return non-zero exit code"
    fi
    
    # 3.3 Missing Hostname
    log_subheader "3.3 Missing Hostname"
    ((TOTAL_TESTS++))
    log_test_start "Test with no arguments"
    
    output=$($FT_PING 2>&1)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -ne 0 ] && echo "$output" | grep -qi "usage\|missing\|hostname\|destination"; then
        log_pass "Missing hostname handled correctly"
    elif [ $exit_code -ne 0 ]; then
        log_pass "Missing hostname returns error (exit code: $exit_code)"
    else
        log_fail "Missing hostname should return non-zero exit code"
    fi
}

# 4. Edge Cases Tests
test_edge_cases() {
    log_header "4. Edge Cases Tests"
    
    # 4.1 Very Short Interval
    log_subheader "4.1 Very Short Interval"
    ((TOTAL_TESTS++))
    log_test_start "Test -i 0.2 option (200ms interval)"
    
    output=$(run_test "$FT_PING -c 5 -i 0.2 localhost" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ]; then
        log_pass "Short interval handled correctly"
    else
        log_fail "Short interval test failed (exit code: $exit_code)"
    fi
    
    # 4.2 Multiple Options Combined
    log_subheader "4.2 Multiple Options Combined"
    ((TOTAL_TESTS++))
    log_test_start "Test -c 2 --ttl 32 -i 1 combination"
    
    output=$(run_test "$FT_PING -c 2 --ttl 32 -i 1 localhost" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ]; then
        log_pass "Multiple options work together correctly"
    else
        log_fail "Multiple options test failed (exit code: $exit_code)"
    fi
}

# 5. Network Tests
test_network() {
    log_header "5. Network Tests"
    
    # 5.1 Google DNS (8.8.8.8)
    log_subheader "5.1 Google DNS Test"
    ((TOTAL_TESTS++))
    log_test_start "Ping Google DNS (8.8.8.8)"
    
    output=$(run_test "$FT_PING -c 3 8.8.8.8" 15)
    exit_code=$?
    log_output "Output" "$output"
    
    if [ $exit_code -eq 0 ] && echo "$output" | grep -q "icmp_seq"; then
        log_pass "Google DNS ping works correctly"
    else
        log_fail "Google DNS ping failed (exit code: $exit_code)"
    fi
    
    # 5.2 Unreachable Host (may timeout)
    log_subheader "5.2 Unreachable Host Test"
    ((TOTAL_TESTS++))
    log_test_start "Ping unreachable host (10.255.255.1)"
    
    # Using a non-routable IP for testing
    output=$(run_test "$FT_PING -c 1 -w 3 10.255.255.1" 10)
    exit_code=$?
    log_output "Output" "$output"
    
    # This should fail or timeout
    if [ $exit_code -ne 0 ] || echo "$output" | grep -qi "unreachable\|timeout\|100% packet loss"; then
        log_pass "Unreachable host handled correctly"
    else
        log_info "Unreachable host test inconclusive (network may route differently)"
    fi
}

# 6. Comparison Tests with System Ping
test_comparison() {
    if [ "$RUN_COMPARE" != "true" ]; then
        log_header "6. Comparison Tests (SKIPPED)"
        log_info "Use --compare flag to run comparison tests"
        return
    fi
    
    log_header "6. Comparison Tests with System Ping"
    
    # 6.1 Basic Comparison
    log_subheader "6.1 Basic Comparison - Localhost"
    ((TOTAL_TESTS++))
    
    log_test_start "ft_ping vs ping comparison"
    
    ft_output=$(run_test "$FT_PING -c 3 localhost" 10)
    ft_exit=$?
    
    sys_output=$(run_test "ping -c 3 localhost" 10)
    sys_exit=$?
    
    log_output "ft_ping output" "$ft_output"
    log_output "system ping output" "$sys_output"
    
    if [ $ft_exit -eq 0 ] && [ $sys_exit -eq 0 ]; then
        log_pass "Both commands completed successfully"
    else
        log_fail "Exit code mismatch (ft_ping: $ft_exit, ping: $sys_exit)"
    fi
    
    # 6.2 Count Option Comparison
    log_subheader "6.2 Count Option Comparison"
    ((TOTAL_TESTS++))
    
    ft_count=$(echo "$ft_output" | grep -c "icmp_seq" || echo "0")
    sys_count=$(echo "$sys_output" | grep -c "icmp_seq" || echo "0")
    
    log_info "ft_ping packets: $ft_count, system ping packets: $sys_count"
    
    if [ "$ft_count" -eq "$sys_count" ]; then
        log_pass "Packet count matches"
    else
        log_fail "Packet count mismatch"
    fi
    
    # 6.3 Error Handling Comparison
    log_subheader "6.3 Error Handling Comparison"
    ((TOTAL_TESTS++))
    
    ft_err=$(run_test "$FT_PING nonexistenthostname123456xyz" 5)
    ft_exit=$?
    
    sys_err=$(run_test "ping -c 1 nonexistenthostname123456xyz" 5)
    sys_exit=$?
    
    log_output "ft_ping error output" "$ft_err"
    log_output "system ping error output" "$sys_err"
    
    if [ $ft_exit -ne 0 ] && [ $sys_exit -ne 0 ]; then
        log_pass "Both commands handle invalid hostname correctly"
    else
        log_fail "Error handling mismatch (ft_ping: $ft_exit, ping: $sys_exit)"
    fi
}

# Generate summary report
generate_summary() {
    log ""
    log_header "TEST SUMMARY"
    
    local summary=""
    summary+="============================================================\n"
    summary+="  FT_PING TEST RESULTS - $(date)\n"
    summary+="============================================================\n"
    summary+="\n"
    summary+="  Total Tests:   $TOTAL_TESTS\n"
    summary+="  ${GREEN}Passed:        $PASSED_TESTS${NC}\n"
    summary+="  ${RED}Failed:        $FAILED_TESTS${NC}\n"
    summary+="  ${YELLOW}Skipped:       $SKIPPED_TESTS${NC}\n"
    summary+="\n"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        summary+="  ${GREEN}${BOLD}ALL TESTS PASSED! ✓${NC}\n"
    else
        summary+="  ${RED}${BOLD}SOME TESTS FAILED ✗${NC}\n"
    fi
    
    summary+="\n"
    summary+="  Log file: $LOG_FILE\n"
    summary+="============================================================\n"
    
    log "$summary"
    
    # Write plain text summary
    {
        echo "============================================================"
        echo "  FT_PING TEST RESULTS - $(date)"
        echo "============================================================"
        echo ""
        echo "  Total Tests:   $TOTAL_TESTS"
        echo "  Passed:        $PASSED_TESTS"
        echo "  Failed:        $FAILED_TESTS"
        echo "  Skipped:       $SKIPPED_TESTS"
        echo ""
        if [ $FAILED_TESTS -eq 0 ]; then
            echo "  ALL TESTS PASSED!"
        else
            echo "  SOME TESTS FAILED"
        fi
        echo ""
        echo "  Log file: $LOG_FILE"
        echo "============================================================"
    } > "$SUMMARY_FILE"
}

# ==============================================================================
# Main Execution
# ==============================================================================

main() {
    parse_args "$@"
    check_root
    check_binary
    setup_logs
    
    log_header "FT_PING AUTOMATED TEST SUITE"
    log_info "Starting tests at $(date)"
    log_info "ft_ping binary: $FT_PING"
    log_info "Comparison mode: $RUN_COMPARE"
    
    # Run all test suites
    test_basic_functionality
    test_command_line_options
    test_error_handling
    test_edge_cases
    test_network
    test_comparison
    
    # Generate final summary
    generate_summary
    
    # Exit with appropriate code
    if [ $FAILED_TESTS -eq 0 ]; then
        exit 0
    else
        exit 1
    fi
}

main "$@"
