#!/bin/bash

# Build all plugins for Linux/Windows

# Parse build type argument or clean_plugins command
if [ "$1" == "--clean" ]; then
    echo "============================================"
    echo "Cleaning all plugin build folders..."
    echo "============================================"
    for category in signalanalyzer signalgenerator positioner; do
        for plugin_dir in `ls $category`; do

            build_dir="$category/$plugin_dir/build"
            echo "Cleaning $build_dir"
            if [ -d "$build_dir" ]; then
                echo "Removing $build_dir"
                rm -rf "$build_dir"
            else
                echo "No build folder for $plugin_dir"
            fi

        done
    done
    echo "All build folders removed."
    exit 0
fi

BUILD_TYPE="Release"
if [ "$1" == "--debug" ]; then
    BUILD_TYPE="Debug"
elif [ "$1" == "--release" ]; then
    BUILD_TYPE="Release"
fi

echo "============================================"
echo "Building Antenna Tester GUI Plugins"
echo "============================================"

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Hardcoded list of plugins to build
# Format: category/plugin_name
PLUGINS_TO_BUILD=( \
    # "signalanalyzer/dummy" \
    # "signalgenerator/dummy" \
    # "signalgenerator/signalcore_sc5511a" \
    # "positioner/dummypositioner" \
    "positioner/planar" \
)

echo "Plugins to build:"
for plugin in "${PLUGINS_TO_BUILD[@]}"; do
    echo "  $plugin"
done
echo ""

# Create dist folder if it doesn't exist
mkdir -p "$SCRIPT_DIR/dist"

# Build counter
BUILD_COUNT=0

for plugin in "${PLUGINS_TO_BUILD[@]}"; do
    category="${plugin%%/*}"
    plugin_name="${plugin##*/}"
    plugin_dir="$SCRIPT_DIR/$category/$plugin_name"

    echo "============================================"
    echo "Building $category/$plugin_name plugin..."
    echo "--------------------------------------------"

    cd "$plugin_dir"

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
        echo "ERROR: CMake configuration failed for $category/$plugin_name plugin"
        continue
    fi

    cmake --build . --config $BUILD_TYPE
    if [ $? -ne 0 ]; then
        echo "ERROR: Build failed for $category/$plugin_name plugin"
        continue
    fi

    echo ""
    echo "$category/$plugin_name plugin built successfully! (Config: $BUILD_TYPE)"
    BUILD_COUNT=$((BUILD_COUNT + 1))
done


# Copy and rename ZIP files to dist folder
echo ""
echo "============================================"
echo "Copying ZIP files to dist folder..."
echo "============================================"

ZIP_COUNT=0
for plugin in "${PLUGINS_TO_BUILD[@]}"; do
    category="${plugin%%/*}"
    plugin_name="${plugin##*/}"
    plugin_dir="$SCRIPT_DIR/$category/$plugin_name"
    ZIP_SOURCE="$plugin_dir/build/$plugin_name-plugin.zip"
    ZIP_SOURCE_ALT="$plugin_dir/build/dummy-plugin.zip"
    DATETIME=$(date +%Y%m%d_%H%M%S)
    BUILD_TAG=""
    if [ "$BUILD_TYPE" == "Debug" ]; then
        BUILD_TAG="_Debug_"
    fi
    ZIP_DEST="$SCRIPT_DIR/dist/$category-$plugin_name-plugin${BUILD_TAG}$DATETIME.zip"

    # If Debug build, look in Debug subdir for zip if it exists
    if [ "$BUILD_TYPE" == "Debug" ]; then
        if [ -f "$plugin_dir/build/Debug/$plugin_name-plugin.zip" ]; then
            ZIP_SOURCE="$plugin_dir/build/Debug/$plugin_name-plugin.zip"
        elif [ -f "$plugin_dir/build/Debug/dummy-plugin.zip" ]; then
            ZIP_SOURCE="$plugin_dir/build/Debug/dummy-plugin.zip"
        fi
    fi

    # Try alternative ZIP file names
    if [ ! -f "$ZIP_SOURCE" ]; then
        ZIP_SOURCE="$ZIP_SOURCE_ALT"
    fi

    if [ -f "$ZIP_SOURCE" ]; then
        echo "Copying $category/$plugin_name plugin..."
        cp "$ZIP_SOURCE" "$ZIP_DEST"
        echo "  Source: $ZIP_SOURCE"
        echo "  Destination: $ZIP_DEST"
        ZIP_COUNT=$((ZIP_COUNT + 1))
    else
        echo "WARNING: ZIP file not found for $category/$plugin_name"
        echo "  Checked: $plugin_dir/build/ and $plugin_dir/build/Debug/"
    fi
done

echo ""
echo "============================================"
echo "Build Complete!"
echo "============================================"
echo ""
echo "Summary:"
echo "  Plugins built: $BUILD_COUNT"
echo "  ZIP files copied: $ZIP_COUNT"
echo ""

if [ $ZIP_COUNT -gt 0 ]; then
    echo "ZIP files available in the dist folder:"
    for zipfile in "$SCRIPT_DIR/dist"/*.zip; do
        if [ -f "$zipfile" ]; then
            echo "  $(basename "$zipfile")"
        fi
    done
    echo ""
fi

echo "Installation:"
echo "1. Use the ZIP files in the dist folder for installation"
echo "2. Launch the Antenna Tester GUI application"
echo "3. Go to Instruments menu to verify plugins are detected"
echo ""
