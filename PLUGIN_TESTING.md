# Plugin Testing Guide

Test application for testing plugin functionality. This tool allows you to select which plugin to test (signal generator, signal analyzer, or positioner).

## Building the Tester

### First Time Build

From the project root directory:

```bash
# Create build directory and configure
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Rebuilding After Changes

```bash
cd build
cmake --build . --config Release
```

## Running Tests

### Using the Shell Script (Recommended)

From the project root:
```bash
./test_plugins.sh
```

### Manual Execution

```bash
cd build/Release
./test_plugin       # Linux/macOS/Git Bash
# or
test_plugin.exe     # Windows
```

### Using Batch File (Windows)

```cmd
run_test.bat
```

## Usage

1. Run the test application using one of the methods above

2. Select a plugin to test from the menu:
   - **0**: Test all plugins sequentially
   - **1**: SignalCore SC5511A (Real Hardware Signal Generator)
   - **2**: Dummy Signal Generator (Simulated)
   - **3**: Dummy Signal Analyzer (Simulated)
   - **4**: Dummy Positioner (Simulated)

3. Watch the test output and callback events in the console

## Available Tests

### Signal Generator Tests
- **Device Scanning**: Search for available signal generators
- **Connection**: Connect to device via serial number or address
- **Connection Status**: Verify connection state
- **Frequency Control**: Set output frequency (e.g., 5.5 GHz)
- **Power Control**: Set output power level (dBm)
- **RF Output**: Enable/disable RF output
- **Callbacks**: Monitor connection, RF state change events

### Signal Analyzer Tests
- **Device Scanning**: Search for available spectrum analyzers
- **Connection**: Connect to device
- **Frequency Configuration**: Set start/stop frequency and RBW
- **Peak Finding**: Locate signal peaks in the spectrum
- **Multiple Measurements**: Perform repeated peak searches
- **Callbacks**: Monitor connection, peak detection events

### Positioner Tests
- **Device Scanning**: Search for available positioners
- **Connection**: Connect to device
- **Position Control**: Move to specific azimuth/elevation coordinates
- **Movement Monitoring**: Track position changes via callbacks
- **Stop Control**: Emergency stop functionality
- **Home Position**: Return to origin (0°, 0°)
- **Callbacks**: Monitor movement start/stop, position updates

## Plugin Paths

The test application loads plugins from these relative paths (from `build/Release/`):

- `../../signalgenerator/signalcore_sc5511a/build/Release/signalcore_sc5511a.dll`
- `../../signalgenerator/dummy/build/Release/dummy.dll`
- `../../signalanalyzer/dummy/build/Release/dummy.dll`
- `../../positioner/dummy/build/Release/dummy.dll`

**Important**: Make sure to build all plugins before running the test application.

## Building All Plugins

Use the provided script to build all plugins:

```bash
./build_plugins.sh
```

This will build:
- SignalCore SC5511A signal generator plugin (with real hardware support)
- Dummy signal generator plugin
- Dummy signal analyzer plugin
- Dummy positioner plugin

## Test Output Example

```
======================================
    Plugin Test Application
======================================

Available Plugins:
  1. SignalCore SC5511A (signalgenerator)
  2. Dummy Signal Generator (signalgenerator)
  3. Dummy Signal Analyzer (signalanalyzer)
  4. Dummy Positioner (positioner)
  0. Test All Plugins

Select plugin to test (0-4): 1

========================================
Testing Signal Generator Plugin
========================================

[Test 1] Scanning for devices...
[SignalCoreSC5511A Plugin] Scanning for devices...
[Callback] Devices scanned: 2 device(s) found
  - SC5511A (SN: 12345678, Address: 12345678)

[Test 2] Connecting to device at: 12345678
[SignalCoreSC5511A Plugin] Connecting to device...
[Callback] Connected!
...
```

## Troubleshooting

### "Failed to load plugin DLL"

**Cause**: Plugin DLL not found at expected path

**Solutions**:
1. Build the plugins first: `./build_plugins.sh`
2. Check that plugin DLLs exist in their respective `build/Release/` directories
3. Verify the working directory is `build/Release/` when running the test

### "Failed to get factory functions"

**Cause**: Plugin DLL doesn't export the required factory functions

**Solutions**:
1. Rebuild the plugin with `cmake --build . --config Release`
2. Verify the plugin properly exports `createXXXPlugin` and `destroyPlugin` functions

### Plugin Tests Fail

**Cause**: Various plugin-specific issues

**Solutions**:
1. Check console output for detailed error messages
2. For SignalCore SC5511A: Ensure hardware is connected and powered on
3. For dummy plugins: These should always work (simulated hardware)

## Development Notes

- The test application uses Windows `LoadLibrary` for dynamic plugin loading
- All callbacks are connected and display events in real-time
- Tests are non-destructive and safe to run repeatedly
- Dummy plugins simulate hardware behavior without requiring actual devices

