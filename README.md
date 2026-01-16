# Antenna Tester GUI - Plugin Development

This document provides instructions for developing, building, and installing plugins for the Antenna Tester GUI application.

> **🎉 Now Qt-Free!** As of January 2026, plugins can be developed using standard C++17 without requiring Qt dependencies. This makes plugin development simpler, more flexible, and accessible to any C++ developer. See [MIGRATION_TO_QT_FREE.md](MIGRATION_TO_QT_FREE.md) for details.

## What is a Plugin?

A plugin is a self-contained software component (typically a dynamic-link library or `.dll` on Windows, and a shared object or `.so` on Linux) that extends the functionality of the Antenna Tester GUI. The application uses plugins to communicate with various hardware instruments, such as:

- Signal Analyzers
- Signal Generators
- Positioners

This modular plugin architecture allows the application to support new hardware devices without needing to modify or recompile the core application itself. Each plugin must implement one of the C++ interfaces defined in [iplugininterface.h](iplugininterface.h) and be paired with a JSON metadata file.

## Prerequisites

Before building the plugins, ensure you have the following software installed:

### Required Software
1. **CMake** (version 3.16 or higher)
   - Download from: https://cmake.org/download/
   - Add CMake to your system PATH during installation

2. **C++ Compiler with C++17 Support**
   - **Windows**: Visual Studio 2019 or newer, or MinGW-w64
   - **Linux**: GCC 7+ or Clang 5+: `sudo apt-get install g++`
   - **macOS**: Xcode Command Line Tools

3. **Build Tools**
   - **Windows**: Visual Studio Build Tools, MSBuild, or Ninja
   - **Linux**: Make: `sudo apt-get install make`

> **Note:** Qt is no longer required for plugin development! Plugins use only standard C++ libraries.

### Verification
To verify your installation:
```bash
# Check CMake version
cmake --version

# Check compiler (Windows)
cl.exe

# Check compiler (Linux/macOS)
g++ --version
```

## Getting Started: The Dummy Plugins

This repository includes a set of "dummy" plugins for each instrument type. These serve two primary purposes:

1.  **As a reference implementation:** They provide a clear, working example of how to implement the required interfaces.
2.  **As a template:** You can copy and modify a dummy plugin project to serve as the foundation for your own hardware plugin.

## Plugin Features

### Dummy Positioner
- Simulates connection/disconnection
- Implements movement in azimuth, elavation, polarity, and planar (X, Y, V)
- Configurable movement range and steps
- Emits proper Qt signals
- Debug logging for all operations

### Dummy Signal Analyzer
- Simulates connection/disconnection
- Implements findPeak() with random realistic data
- Configurable frequency range and RBW
- Emits proper Qt signals
- Debug logging for all operations

### Dummy Signal Generator
- Simulates connection/disconnection
- RF enable/disable functionality
- Configurable frequency and power
- Proper state management
- Debug logging for all operations

## Testing the Plugins

1. Build the plugins using the instructions above
2. Copy/install to the application's instruments directory
3. Launch the Antenna Tester GUI application
4. Go to **Instruments > Signal Analyzers** (or Signal Generators)
5. You should see "Dummy Signal Analyzer" (or "Dummy Signal Generator") with **Valid** status in green
6. Select the plugin from the INSTRUMENTS combo box in the main UI
7. The status label should turn green with the plugin name
8. Click START to test the functionality

## Directory Structure

The example directory structure for the dummy plugins is as follows. A real plugin would have a similar layout.

```
antenna-test-plugins/
├── signalanalyzer/
│   └── dummy/
│       ├── CMakeLists.txt
│       ├── dummysignalanalyzer.h
│       ├── dummysignalanalyzer.cpp
│       ├── plugin.json
│       └── build/
│           └── signalanalyzer-dummy-plugin.zip  (created after build)
└── signalgenerator/
    └── dummy/
        ├── ... (similar files)
└── positioner/
    └── dummy/
        ├── ... (similar files)
```

## Building the Plugins

### Method 1: Automated Build Scripts (Recommended)

The scripts will:
1. Check for Qt installation
2. Build all plugins in sequence (signalanalyzer, signalgenerator, positioner)
3. Create ZIP files in the `dist/` folder
4. Provide a summary of what was built

#### Windows

Use the provided batch scripts to build all plugins automatically:

```bash
build_plugins.bat
```

#### Linux

Use the provided shell script to build all plugins automatically:

```bash
# Make the script executable
chmod +x build_plugins.sh

# Run the build script
./build_plugins.sh
```

### Method 2: Manual Building

If you prefer to build plugins individually or need more control:

#### Windows (Using CMake)

##### Build Signal Analyzer Plugin

```bash
cd signalanalyzer/dummy
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=\"C:\\Qt\\6.9.1\\msvc2022_64\" ..
cmake --build . --config Release
```

##### Build Signal Generator Plugin

```bash
cd signalgenerator/dummy
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=\"C:\\Qt\\6.9.1\\msvc2022_64\" ..
cmake --build . --config Release
```

##### Build Positioner Plugin

```bash
cd positioner/dummy
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=\"C:\\Qt\\6.9.1\\msvc2022_64\" ..
cmake --build . --config Release
```

#### Linux

##### Build Signal Analyzer Plugin

```bash
cd signalanalyzer/dummy
mkdir build
cd build
cmake ..
make
```

##### Build Signal Generator Plugin

```bash
cd signalgenerator/dummy
mkdir build
cd build
cmake ..
make
```

##### Build Positioner Plugin

```bash
cd positioner/dummy
mkdir build
cd build
cmake ..
make
```

## The `dist/` Folder

After building with the automated scripts, all plugin ZIP files are collected in the `dist/` folder:

```
dist/
├── signalanalyzer-dummy-plugin.zip
├── signalgenerator-dummy-plugin.zip
└── positioner-dummy-plugin.zip
```

These ZIP files are ready for installation and contain:
- The plugin DLL/SO file
- The JSON metadata file
- Any required dependencies

If you build manually, you will have to copy all the plugin's .zip file manually.

## Installing Plugins

After building, ZIP files are automatically created in the `build/` directory of each plugin.

### Option 1: Using Application Install Button (Recommended)

1. Launch the Antenna Tester GUI application
2. Go to **Instruments** menu → **Signal Analyzers** (or **Signal Generators**)
3. Click **"Install Plugin"** button
4. Select the `dummy-plugin.zip` file from:
   - Positioner: `positioner/dummy/build/dummy-plugin.zip`
   - Signal Analyzer: `signalanalyzer/dummy/build/dummy-plugin.zip`
   - Signal Generator: `signalgenerator/dummy/build/dummy-plugin.zip`
5. The plugin will be automatically extracted and installed

### Option 2: Manual Installation

Extract the ZIP file and copy the plugin folder to the application's instruments directory:

```bash
# Extract dummy-plugin.zip, then copy the 'dummy' folder to:
<app_directory>\instruments\positioner\
# or
<app_directory>\instruments\signalanalyzer\
# or
<app_directory>\instruments\signalgenerator\
```

This will automatically create the directory structure and copy both the DLL and JSON files.

## Modifying for Real Hardware

To create a real hardware plugin:

1. Copy one of the dummy plugin folders
2. Rename files and classes appropriately
3. Replace simulation code with actual hardware communication
4. Update the JSON metadata with correct information
5. Rebuild and install

See [PLUGIN_DEVELOPMENT.md](PLUGIN_DEVELOPMENT.md) for complete plugin development guide.

## Troubleshooting

**Plugin shows as "Invalid" in application:**
- Check that both .dll and .json files are in the correct directory
- Verify JSON file has valid format and required fields
- Ensure Qt libraries are in PATH
- Check application debug output for detailed error messages

**Build fails - Qt not found:**
- Verify CMAKE_PREFIX_PATH points to Qt installation
- Ensure Qt6 Core and Widgets modules are installed

**Plugin loads but doesn't connect:**
- Check debug console output for error messages
- Verify the plugin exports the factory functions correctly
- Ensure Q_INTERFACES macro is present in the header
