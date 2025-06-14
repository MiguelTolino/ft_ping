 # Manual Testing Guide for ft_ping

This guide provides a comprehensive set of test cases to verify the functionality of ft_ping. Follow each test case step by step and verify the expected behavior.

## Prerequisites
- Build ft_ping using `make`
- Ensure you have root privileges (required for raw socket operations)
- Have an active internet connection

## Test Cases

### 1. Basic Functionality Tests

#### 1.1 Localhost Ping
1. Run: `sudo ./ft_ping localhost`
2. Verify:
   - Program starts and shows ping statistics
   - ICMP packets are being sent and received
   - Round-trip time is displayed
   - Program continues until interrupted (Ctrl+C)

#### 1.2 Internet Host Ping
1. Run: `sudo ./ft_ping google.com`
2. Verify:
   - DNS resolution works
   - Packets are sent and received
   - Round-trip time is displayed
   - Statistics are shown correctly

### 2. Command-Line Options Tests

#### 2.1 Count Option (-c)
1. Run: `sudo ./ft_ping -c 3 localhost`
2. Verify:
   - Program sends exactly 3 packets
   - Program exits after 3 packets
   - Final statistics are displayed

#### 2.2 TTL Option (-t)
1. Run: `sudo ./ft_ping -t 32 localhost`
2. Verify:
   - TTL value is set to 32
   - Packets are sent and received
   - No TTL exceeded errors occur

#### 2.3 Timeout Option (-w)
1. Run: `sudo ./ft_ping -w 2 localhost`
2. Verify:
   - Program waits up to 2 seconds for each response
   - Timeout is properly handled

#### 2.4 Interval Option (-i)
1. Run: `sudo ./ft_ping -i 2 localhost`
2. Verify:
   - 2-second delay between packets
   - Timing is consistent

#### 2.5 Verbose Output (-v)
1. Run: `sudo ./ft_ping -v localhost`
2. Verify:
   - Additional debugging information is displayed
   - ICMP packet details are shown

#### 2.6 Version Display (-V)
1. Run: `./ft_ping -V`
2. Verify:
   - Version information is displayed
   - Program exits after showing version

#### 2.7 Help Message (-?)
1. Run: `./ft_ping -?`
2. Verify:
   - Help message is displayed
   - All options are listed
   - Program exits after showing help

### 3. Error Handling Tests

#### 3.1 Invalid Hostname
1. Run: `sudo ./ft_ping nonexistenthostname123456`
2. Verify:
   - Appropriate error message is displayed
   - Program exits with error code

#### 3.2 Invalid Option
1. Run: `sudo ./ft_ping -x localhost`
2. Verify:
   - Error message about invalid option is displayed
   - Program exits with error code

#### 3.3 Missing Hostname
1. Run: `sudo ./ft_ping`
2. Verify:
   - Usage message is displayed
   - Program exits with error code

### 4. Edge Cases

#### 4.1 Very Short Interval
1. Run: `sudo ./ft_ping -i 0.2 localhost`
2. Verify:
   - Program handles short intervals correctly
   - No timing issues occur

#### 4.2 Very Long Timeout
1. Run: `sudo ./ft_ping -w 10 localhost`
2. Verify:
   - Program waits up to 10 seconds for responses
   - Timeout is properly handled

#### 4.3 Multiple Options
1. Run: `sudo ./ft_ping -c 2 -t 32 -i 1 localhost`
2. Verify:
   - All options work together correctly
   - No conflicts between options

### 5. Network Tests

#### 5.1 Google DNS
1. Run: `sudo ./ft_ping 8.8.8.8`
2. Verify:
   - Packets reach Google DNS
   - Responses are received
   - Round-trip times are reasonable

#### 5.2 Unreachable Host
1. Run: `sudo ./ft_ping 192.168.255.255`
2. Verify:
   - Appropriate error message is displayed
   - Program handles unreachable host correctly

### 6. Comparison with Original Ping Command

For each test case below, run both commands and compare their output and behavior.

#### 6.1 Basic Comparison
1. Run both commands:
   ```bash
   sudo ./ft_ping localhost
   sudo ping localhost
   ```
2. Compare:
   - Output format and style
   - Statistics display
   - Round-trip time format
   - Packet size
   - Default behavior

#### 6.2 Option Comparison

##### 6.2.1 Count Option
1. Run:
   ```bash
   sudo ./ft_ping -c 3 localhost
   sudo ping -c 3 localhost
   ```
2. Compare:
   - Packet count behavior
   - Final statistics format
   - Exit behavior

##### 6.2.2 TTL Option
1. Run:
   ```bash
   sudo ./ft_ping -t 32 localhost
   sudo ping -t 32 localhost
   ```
2. Compare:
   - TTL handling
   - Error messages for TTL exceeded
   - Packet behavior

##### 6.2.3 Timeout Option
1. Run:
   ```bash
   sudo ./ft_ping -w 2 localhost
   sudo ping -w 2 localhost
   ```
2. Compare:
   - Timeout behavior
   - Error handling
   - Program exit on timeout

##### 6.2.4 Interval Option
1. Run:
   ```bash
   sudo ./ft_ping -i 2 localhost
   sudo ping -i 2 localhost
   ```
2. Compare:
   - Interval timing
   - Packet spacing
   - Overall timing behavior

#### 6.3 Error Handling Comparison

##### 6.3.1 Invalid Hostname
1. Run:
   ```bash
   sudo ./ft_ping nonexistenthostname123456
   sudo ping nonexistenthostname123456
   ```
2. Compare:
   - Error message format
   - Exit codes
   - DNS resolution behavior

##### 6.3.2 Invalid Option
1. Run:
   ```bash
   sudo ./ft_ping -x localhost
   sudo ping -x localhost
   ```
2. Compare:
   - Error message format
   - Help/usage display
   - Exit behavior

#### 6.4 Network Behavior Comparison

##### 6.4.1 Google DNS
1. Run:
   ```bash
   sudo ./ft_ping 8.8.8.8
   sudo ping 8.8.8.8
   ```
2. Compare:
   - Response times
   - Packet loss handling
   - Statistics calculation

##### 6.4.2 Unreachable Host
1. Run:
   ```bash
   sudo ./ft_ping 192.168.255.255
   sudo ping 192.168.255.255
   ```
2. Compare:
   - Error message format
   - Timeout behavior
   - Exit codes

#### 6.5 Edge Case Comparison

##### 6.5.1 Very Short Interval
1. Run:
   ```bash
   sudo ./ft_ping -i 0.2 localhost
   sudo ping -i 0.2 localhost
   ```
2. Compare:
   - Minimum interval handling
   - Timing accuracy
   - System resource usage

##### 6.5.2 Very Long Timeout
1. Run:
   ```bash
   sudo ./ft_ping -w 10 localhost
   sudo ping -w 10 localhost
   ```
2. Compare:
   - Timeout behavior
   - Program responsiveness
   - Exit conditions

## Comparison Checklist

For each comparison test, verify:
- [ ] Output format matches original ping
- [ ] Statistics calculation is consistent
- [ ] Error handling is similar
- [ ] Option behavior matches
- [ ] Timing and intervals are accurate
- [ ] Exit codes are consistent
- [ ] Resource usage is similar

## Notes for Comparison Testing
- Run tests in the same network conditions
- Use the same host for both commands
- Compare multiple runs to ensure consistency
- Document any significant differences
- Pay special attention to timing and statistics
- Verify that error messages are helpful and accurate

## Expected Behavior Checklist

For each test case, verify:
- [ ] Correct error messages when applicable
- [ ] Proper packet transmission and reception
- [ ] Accurate statistics display
- [ ] Appropriate program exit codes
- [ ] No memory leaks (use valgrind if available)
- [ ] No segmentation faults
- [ ] Clean program termination

## Notes
- Some tests require root privileges
- Network conditions may affect test results
- Consider running tests multiple times to ensure consistency
- Document any unexpected behavior or issues encountered