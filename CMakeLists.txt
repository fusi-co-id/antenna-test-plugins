cmake_minimum_required(VERSION 3.16)
project(PluginTester VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test application
add_executable(test_plugin
    test_plugin.cpp
    iplugininterface.h
)

# Include directories
target_include_directories(test_plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Windows specific settings
if(WIN32)
    set_target_properties(test_plugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()
