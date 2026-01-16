# Plugin Development Guide

This guide explains how to create instrument plugins for the Antenna Tester GUI application.

## Overview

The Antenna Tester GUI supports dynamic loading of instrument plugins for Signal Analyzers, Signal Generators, and Positioners. Plugins are DLL libraries that implement standard C++ interfaces without requiring Qt dependencies.

**Key Features:**
- **No Qt Dependency**: Plugins can be developed using standard C++ without Qt
- **Standard C++ Types**: Uses `std::string`, `std::vector`, and `std::function`
- **Callback-based Events**: Replaces Qt signals/slots with C++ function callbacks
- **Cross-platform Compatible**: Can be built with any C++17 compiler

## Plugin Structure

Each plugin consists of:
1. **DLL file** (`{plugin_name}.dll`) - The compiled plugin library
2. **JSON metadata** (`{plugin_name}.json`) - Plugin information
3. **Header file** (`{plugin_name}.h`) - Optional, for documentation

All files must be placed in a folder with the same name as the plugin:
```
instruments/
  signalanalyzer/
    my_plugin/
      my_plugin.dll
      my_plugin.json
      my_plugin.h (optional)
  signalgenerator/
    my_plugin/
      my_plugin.dll
      my_plugin.json
      my_plugin.h (optional)
  positioner/
    my_plugin/
      my_plugin.dll
      my_plugin.json
      my_plugin.h (optional)
```

## JSON Metadata Format

```json
{
    "name": "My Instrument Plugin",
    "vendor": "Instrument Manufacturer",
    "version": "1.0.0",
    "series": "Model Series",
    "type": "LAN",
    "author": "Your Name",
    "email": "your@email.com",
    "description": "Your plugin description.",
    "url": "https://yourwebsite.com",
    "license": "MIT"
}
```

**Required Fields:**
- `name` - Display name of the plugin
- `vendor` - Manufacturer name
- `version` - Plugin version, e.g: 1.0.0

**Optional Fields:**
- `series` - Instrument series or model range
- `type` - Connection type: "LAN", "USB", "GPIB", etc.
- `author` - Author of the plugin
- `email` - Author's email
- `description` - Description of the plugin
- `url` - Website url of the author
- `license` - License type

## Signal Analyzer Plugin Interface

### Required Header

Include the plugin interface header:
```cpp
#include "iplugininterface.h"
```

### Interface Implementation

```cpp
class MySignalAnalyzerPlugin : public ISignalAnalyzerPlugin
{
public:
    MySignalAnalyzerPlugin();
    virtual ~MySignalAnalyzerPlugin();
    
    // Device discovery
    std::vector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const std::string &address) override;

    // Connection management
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    void setStartFreq(double freqHz) override;
    void setStopFreq(double freqHz) override;
    void setRBW(double freqHz) override;
    
    // Measurement
    Peak findPeak() override;
    
    // Note: Event callbacks are optional and can be set by the host application
    // onConnected, onDisconnected, onPeakFound, onError, onDevicesScanned
    
private:
    // Your implementation details
};
```

### Event Callbacks

The plugin interface provides callback functions that can be set by the host application:

```cpp
// In your implementation, trigger callbacks when events occur:
void MySignalAnalyzerPlugin::connect() {
    // ... connection logic ...
    if (onConnected) {
        onConnected();
    }
}

void MySignalAnalyzerPlugin::disconnect() {
    // ... disconnection logic ...
    if (onDisconnected) {
        onDisconnected();
    }
}

std::vector<DeviceInfo> MySignalAnalyzerPlugin::scanDevices() {
    // ... scan logic ...
    std::vector<DeviceInfo> devices = /* ... */;
    
    if (onDevicesScanned) {
        onDevicesScanned(devices);
    }
    
    return devices;
}
```

### Required Export Functions

```cpp
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    ISignalAnalyzerPlugin* createSignalAnalyzerPlugin()
    {
        return new MySignalAnalyzerPlugin();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        delete static_cast<ISignalAnalyzerPlugin*>(plugin);
    }
}
```

## Signal Generator Plugin Interface

### Interface Implementation

```cpp
class MySignalGeneratorPlugin : public ISignalGeneratorPlugin
{
public:
    MySignalGeneratorPlugin();
    virtual ~MySignalGeneratorPlugin();
    
    // Device discovery
    std::vector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const std::string &address) override;

    // Connection management
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    void setFreq(double freqHz) override;
    void setPower(double powerDbm) override;
    
    // RF Control
    void enableRf() override;
    void disableRf() override;
    bool isRfEnabled() const override;
    
    // Note: Event callbacks are optional and can be set by the host application
    // onConnected, onDisconnected, onRfEnabled, onRfDisabled, onError, onDevicesScanned
    
private:
    // Your implementation details
};
```

### Required Export Functions

```cpp
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    ISignalGeneratorPlugin* createSignalGeneratorPlugin()
    {
        return new MySignalGeneratorPlugin();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        delete static_cast<ISignalGeneratorPlugin*>(plugin);
    }
}
```

## Positioner Plugin Interface

### Interface Implementation

```cpp
class MyPositionerPlugin : public IPositionerPlugin
{
public:
    MyPositionerPlugin();
    virtual ~MyPositionerPlugin();
    
    // Device discovery
    std::vector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const std::string &address) override;
    
    // Connection management
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    void setAZStep(double step) override;
    void setStep(const Step &step) override;
    void setMinRange(const MinRange &minRange) override;
    void setMaxRange(const MaxRange &maxRange) override;
    void setMovement(const Movement &movement) override;
    void setDistance(double distance) override;
    
    // Control
    void start() override;
    void stop() override;
    
    // Note: Event callbacks are optional and can be set by the host application
    // onConnected, onDisconnected, onMovementStarted, onMovementStopped, 
    // onPositionChanged, onError, onDevicesScanned
    
private:
    // Your implementation details
};
```

### Required Export Functions

```cpp
extern "C" {
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    IPositionerPlugin* createPositionerPlugin()
    {
        return new MyPositionerPlugin();
    }
    
    #ifdef _WIN32
        __declspec(dllexport)
    #endif
    void destroyPlugin(void* plugin)
    {
        delete static_cast<IPositionerPlugin*>(plugin);
    }
}
```

## Plugin Validation

The application validates plugins automatically:

1. **File Check**: Verifies DLL and JSON files exist
2. **JSON Parsing**: Validates JSON metadata format
3. **DLL Loading**: Attempts to load the DLL library
4. **Interface Check**: Verifies required factory functions exist
5. **Instance Test**: Creates a test instance to verify the interface

**Plugin Status Indicators:**
- ✅ **Valid** (Green) - Plugin loaded and validated successfully
- ❌ **Missing DLL** (Red) - DLL file not found
- ❌ **Missing JSON** (Red) - JSON metadata file not found
- ❌ **Invalid JSON** (Red) - JSON parse error or missing required fields
- ❌ **Invalid Interface** (Red) - Missing factory functions or interface mismatch
- ❌ **Load Error** (Red) - DLL load failure

## Build Requirements

### Dependencies
- **C++17 Compiler** (MSVC, MinGW, GCC, Clang, etc.)
- **No Qt Dependency Required** - Plugins use only standard C++

### Compiler Flags
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Example CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyPlugin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plugin source files
set(PLUGIN_SOURCES
    my_plugin.cpp
)

set(PLUGIN_HEADERS
    my_plugin.h
    ../../iplugininterface.h
)

# Create shared library (DLL)
add_library(my_plugin SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# Include directories
target_include_directories(my_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

# Set output name
set_target_properties(my_plugin PROPERTIES
    OUTPUT_NAME "my_plugin"
    PREFIX ""
)

# Windows specific settings
if(WIN32)
    set_target_properties(my_plugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()

# Install rules
install(TARGETS my_plugin
    RUNTIME DESTINATION instruments/signalgenerator/my_plugin
    LIBRARY DESTINATION instruments/signalgenerator/my_plugin
)

install(FILES my_plugin.json
    DESTINATION instruments/signalgenerator/my_plugin
)
```

## Key Differences from Qt-based Plugins

### Types Mapping

| Old Qt Type | New Standard C++ Type |
|------------|----------------------|
| `QString` | `std::string` |
| `QVector<T>` | `std::vector<T>` |
| `QObject*` | `void*` (in destroy function) |

### Event Handling

**Old (Qt Signals):**
```cpp
signals:
    void connected();
    void disconnected();
    
// In implementation:
emit connected();
```

**New (Callbacks):**
```cpp
// In implementation:
if (onConnected) {
    onConnected();
}
```

### Logging

**Old (Qt Debug):**
```cpp
#include <QDebug>
qDebug() << "Message";
qWarning() << "Warning";
```

**New (Standard C++):**
```cpp
#include <iostream>
std::cout << "Message" << std::endl;
std::cerr << "Warning" << std::endl;
```

### Threading/Delays

**Old (Qt Thread):**
```cpp
#include <QThread>
QThread::msleep(100);
```

**New (Standard C++):**
```cpp
#include <thread>
#include <chrono>
std::this_thread::sleep_for(std::chrono::milliseconds(100));
```

## Testing Your Plugin

1. **Build the plugin** and copy files to the appropriate instruments folder
2. **Launch the application**
3. **Open plugin dialog**: Instruments > Signal Analyzers (or Signal Generators)
4. **Check status column**:
   - Green "Valid" = Plugin ready to use
   - Red with error = Hover over status to see error message
5. **Select plugin** from the table
6. **Choose in UI**: Select your plugin from the INSTRUMENTS combo box

## Example Implementation

See these directories for example plugin implementations:
- `signalgenerator/signalcore_sc5511a/` - SignalCore SC5511A signal generator plugin (Qt-independent)
- `signalanalyzer/dummy/` - Dummy signal analyzer plugin (Qt-independent)
- `positioner/dummy/` - Dummy positioner plugin (Qt-independent)

## Troubleshooting

**Plugin shows "Missing DLL"**
- Verify the DLL file exists in the correct folder
- Check file naming matches the folder name

**Plugin shows "Invalid Interface"**
- Ensure factory functions are exported correctly
- Verify the plugin implements all required interface methods
- Check that function signatures match the interface exactly

**Plugin shows "Load Error"**
- Check DLL dependencies (use Dependency Walker on Windows)
- Verify C++ runtime compatibility
- Ensure correct compiler architecture (x86/x64) matches application

**Plugin loads but doesn't connect**
- Check hardware connection settings
- Review plugin's connect() implementation
- Enable debug logging (std::cout/std::cerr) to see error messages

**Compilation Errors**
- Ensure C++17 is enabled in your compiler settings
- Include all necessary headers (`<string>`, `<vector>`, `<functional>`, etc.)
- On Windows, include `<Windows.h>` if using Windows-specific APIs
