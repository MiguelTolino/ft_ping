#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create logs directory if it doesn't exist
mkdir -p logs

# Test hosts (using IPs for faster resolution)
HOSTS=("8.8.8.8" "127.0.0.1")

# Timeout in seconds for each test
TIMEOUT=5

# Function to print test header
print_header() {
    echo -e "\n${YELLOW}=== Testing $1 ===${NC}"
}

# Function to clean test name for filename
clean_test_name() {
    echo "$1" | tr ' ' '_' | tr ':' '_' | tr '(' '_' | tr ')' '_'
}

# Function to run test and compare outputs
run_test() {
    local test_name=$1
    local ft_ping_cmd=$2
    local ping_cmd=$3
    local test_id=$(clean_test_name "$test_name")
    
    print_header "$test_name"
    
    # Create log files
    local ft_ping_log="logs/ft_ping_${test_id}.log"
    local ping_log="logs/ping_${test_id}.log"
    local diff_log="logs/diff_${test_id}.log"
    
    echo -e "${YELLOW}Running ft_ping:${NC}"
    echo "$ft_ping_cmd"
    timeout $TIMEOUT bash -c "$ft_ping_cmd" > "$ft_ping_log" 2>&1
    
    echo -e "\n${YELLOW}Running original ping:${NC}"
    echo "$ping_cmd"
    timeout $TIMEOUT bash -c "$ping_cmd" > "$ping_log" 2>&1
    
    # Compare outputs
    echo -e "\n${YELLOW}Differences:${NC}"
    if diff -u "$ft_ping_log" "$ping_log" > "$diff_log"; then
        echo -e "${GREEN}No differences found${NC}"
    else
        echo -e "${RED}Differences found. See $diff_log for details${NC}"
        cat "$diff_log"
    fi
}

# Test 1: Basic ping (reduced count)
run_test "Basic ping" \
    "ft_ping -c 2 8.8.8.8" \
    "ping -c 2 8.8.8.8"

# Test 2: Count option (small count)
run_test "Count option (-c)" \
    "ft_ping -c 1 8.8.8.8" \
    "ping -c 1 8.8.8.8"

# Test 3: TTL option
run_test "TTL option (-t)" \
    "ft_ping -c 1 -t 32 8.8.8.8" \
    "ping -c 1 -t 32 8.8.8.8"

# Test 4: Verbose output
run_test "Verbose output (-v)" \
    "ft_ping -c 1 -v 8.8.8.8" \
    "ping -c 1 -v 8.8.8.8"

# Test 5: Version display
run_test "Version display (-V)" \
    "ft_ping -V" \
    "ping -V"

# Test 6: Invalid host
run_test "Invalid host" \
    "ft_ping nonexistenthost123456789.com" \
    "ping nonexistenthost123456789.com"

# Test 7: Invalid TTL
run_test "Invalid TTL" \
    "ft_ping -t 300 8.8.8.8" \
    "ping -t 300 8.8.8.8"

# Test 8: Invalid count
run_test "Invalid count" \
    "ft_ping -c 0 8.8.8.8" \
    "ping -c 0 8.8.8.8"

# Test 9: Multiple hosts (reduced count)
for host in "${HOSTS[@]}"; do
    run_test "Testing host: $host" \
        "ft_ping -c 1 $host" \
        "ping -c 1 $host"
done

# Test 10: Combined options
run_test "Combined options (-c -t)" \
    "ft_ping -c 1 -t 64 8.8.8.8" \
    "ping -c 1 -t 64 8.8.8.8"

# Create summary
echo -e "\n${YELLOW}Creating test summary...${NC}"
echo "Test Summary" > logs/summary.log
echo "============" >> logs/summary.log
echo "" >> logs/summary.log

for test_file in logs/diff_*.log; do
    if [ -f "$test_file" ]; then
        test_name=$(basename "$test_file" | sed 's/diff_//' | sed 's/\.log//')
        if [ -s "$test_file" ]; then
            echo "❌ $test_name: Differences found" >> logs/summary.log
        else
            echo "✅ $test_name: No differences" >> logs/summary.log
        fi
    fi
done

echo -e "\n${GREEN}All tests completed!${NC}"
echo -e "${YELLOW}Check the logs directory for detailed results${NC}"
echo -e "${YELLOW}Summary available in logs/summary.log${NC}" 