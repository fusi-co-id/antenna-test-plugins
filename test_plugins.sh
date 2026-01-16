#!/bin/bash
echo "======================================"
echo "   Plugin Test Runner"
echo "======================================"
echo ""

if [ ! -d "build" ]; then
    mkdir build
else
    echo "Cleaning previous build..."
    rm -rf build/*
fi

cd build

if [ -n "$CMAKE_PREFIX_PATH" ]; then
    cmake -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" ..
else
    cmake ..
fi

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed plugin tester"
    exit 1
fi

cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed for plugin tester"
    exit 1
fi

echo ""
echo "Plugin tester built successfully!"
echo "Running plugin tester..."
echo ""
cd Release
./test_plugin
echo ""
read -p "Press Enter to exit..."
