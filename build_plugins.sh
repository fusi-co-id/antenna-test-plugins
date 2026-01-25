#!/bin/bash


# Build all plugins for Linux/Windows

# Parse build type argument
BUILD_TYPE="Release"
if [ "$1" == "--debug" ]; then
    BUILD_TYPE="Debug"
elif [ "$1" == "--release" ]; then
    BUILD_TYPE="Release"
fi

echo "============================================"
echo "Building Antenna Tester GUI Plugins"
echo "============================================"

# Set Qt path - MODIFY THIS IF NEEDED
# QT_PATH="/opt/Qt/6.9.1/gcc_64"

if [ ! -d "$QT_PATH" ]; then
    echo "WARNING: Qt not found at $QT_PATH"
    echo "Attempting to use system Qt installation..."
    QT_PATH=""
fi

if [ -n "$QT_PATH" ]; then
    export CMAKE_PREFIX_PATH="$QT_PATH"
    echo ""
    echo "Qt path: $QT_PATH"
fi

echo ""

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Find all plugin directories (signalanalyzer, signalgenerator, positioner)
echo "Looking for plugin directories..."
PLUGIN_CATEGORIES=""
for category in "$SCRIPT_DIR"/*/; do
    category_name=$(basename "$category")
    if [[ "$category_name" =~ ^(signalanalyzer|signalgenerator|positioner)$ ]]; then
        PLUGIN_CATEGORIES="$PLUGIN_CATEGORIES $category_name"
        echo "  Found category: $category_name"
    fi
done

if [ -z "$PLUGIN_CATEGORIES" ]; then
    echo "ERROR: No valid plugin categories found!"
    echo "Expected directories: signalanalyzer, signalgenerator, or positioner"
    exit 1
fi

echo ""
echo "Plugin categories to process: $PLUGIN_CATEGORIES"
echo ""

# Create dist folder if it doesn't exist
mkdir -p "$SCRIPT_DIR/dist"

# Build counter
BUILD_COUNT=0

for category in $PLUGIN_CATEGORIES; do
    echo "============================================"
    echo "Processing $category plugins..."
    echo "============================================"
    
    # Find all plugin subdirectories in this category
    for plugin_dir in "$SCRIPT_DIR/$category"/*/; do
        plugin_name=$(basename "$plugin_dir")
        
        echo ""
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
done

# Copy and rename ZIP files to dist folder
echo ""
echo "============================================"
echo "Copying ZIP files to dist folder..."
echo "============================================"

ZIP_COUNT=0
for category in $PLUGIN_CATEGORIES; do
    for plugin_dir in "$SCRIPT_DIR/$category"/*/; do
        plugin_name=$(basename "$plugin_dir")
        ZIP_SOURCE="$plugin_dir/build/$plugin_name-plugin.zip"
        ZIP_SOURCE_ALT="$plugin_dir/build/dummy-plugin.zip"
        ZIP_DEST="$SCRIPT_DIR/dist/$category-$plugin_name-plugin.zip"

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
