# Plugin Development Guide

This guide explains how to create instrument plugins for the Antenna Tester GUI application.

## Overview

The Antenna Tester GUI supports dynamic loading of instrument plugins for Signal Analyzers, Signal Generators, and Positioners. Plugins are DLL libraries that implement standard interfaces defined by the application.

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
    Q_OBJECT
    Q_INTERFACES(ISignalAnalyzerPlugin)
    
public:
    MySignalAnalyzerPlugin();
    virtual ~MySignalAnalyzerPlugin();
    
    // Device discovery
    QVector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const QString &address) override;

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
    
signals:
    void connected() override;
    void disconnected() override;
    void peakFound(const Peak &peak) override;
    void errorOccurred(const QString &error) override;
    
private:
    // Your implementation details
};
```

### Required Export Functions

```cpp
extern "C" {
    __declspec(dllexport) ISignalAnalyzerPlugin* createSignalAnalyzerPlugin()
    {
        return new MySignalAnalyzerPlugin();
    }
    
    __declspec(dllexport) void destroyPlugin(QObject* plugin)
    {
        delete plugin;
    }
}
```

## Signal Generator Plugin Interface

### Interface Implementation

```cpp
class MySignalGeneratorPlugin : public ISignalGeneratorPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISignalGeneratorPlugin)
    
public:
    MySignalGeneratorPlugin();
    virtual ~MySignalGeneratorPlugin();
    
    // Device discovery
    QVector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const QString &address) override;

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
    
signals:
    void connected() override;
    void disconnected() override;
    void rfEnabled() override;
    void rfDisabled() override;
    void errorOccurred(const QString &error) override;
    
private:
    // Your implementation details
};
```

### Required Export Functions

```cpp
extern "C" {
    __declspec(dllexport) ISignalGeneratorPlugin* createSignalGeneratorPlugin()
    {
        return new MySignalGeneratorPlugin();
    }
    
    __declspec(dllexport) void destroyPlugin(QObject* plugin)
    {
        delete plugin;
    }
}
```

## Positioner Plugin Interface

### Interface Implementation

```cpp
class MyPositionerPlugin : public ISignalGeneratorPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISignalGeneratorPlugin)
    
public:
    MyPositionerPlugin();
    virtual ~MyPositionerPlugin();
    
    // Device discovery
    QVector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const QString &address) override;
    
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
    
signals:
    void connected();
    void disconnected();
    void movementStarted();
    void movementStopped();
    void positionChanged(double az, double el, double pol);
    void errorOccurred(const QString &error);
    
private:
    // Your implementation details
};
```

### Required Export Functions

```cpp
extern "C" {
    __declspec(dllexport) IPositionerPlugin* createPositionerPlugin()
    {
        return new MyPositionerPlugin();
    }
    
    __declspec(dllexport) void destroyPlugin(QObject* plugin)
    {
        delete plugin;
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
- Qt 6.x
- MinGW or MSVC compiler
- Qt Core and Qt Widgets modules

### Compiler Flags
```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Example CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyPlugin)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_library(my_plugin SHARED
    my_plugin.cpp
    my_plugin.h
)

target_link_libraries(my_plugin
    Qt6::Core
    Qt6::Widgets
)

target_compile_definitions(my_plugin PRIVATE
    MY_PLUGIN_LIBRARY
)
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

See the this directories for example plugin implementations:
- `signalanalyzer/dummy/` - Dummy signal analyzer plugin
- `signalgenerator/dummy/` - Dummy signal generator plugin
- `positioner/dummy/` - Dummy signal generator plugin

## Troubleshooting

**Plugin shows "Missing DLL"**
- Verify the DLL file exists in the correct folder
- Check file naming matches the folder name

**Plugin shows "Invalid Interface"**
- Ensure factory functions are exported correctly
- Verify the plugin implements all required interface methods
- Check that Q_INTERFACES macro is present

**Plugin shows "Load Error"**
- Check DLL dependencies (use Dependency Walker)
- Verify Qt version compatibility
- Ensure correct compiler (MinGW/MSVC) matches application

**Plugin loads but doesn't connect**
- Check hardware connection settings
- Review plugin's connect() implementation
- Enable debug logging to see error messages
