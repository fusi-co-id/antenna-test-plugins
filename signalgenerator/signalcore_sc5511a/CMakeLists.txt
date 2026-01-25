cmake_minimum_required(VERSION 3.16)
project(signalcore_sc5511aSignalGenerator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plugin source files
set(PLUGIN_SOURCES
    signalcore_sc5511a.cpp
)

set(PLUGIN_HEADERS
    signalcore_sc5511a.h
    ../../iplugininterface.h
    include/sc5511a.h
    include/stdafx.h
)

# Create shared library (DLL)
add_library(signalcore_sc5511a SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# Include directories
target_include_directories(signalcore_sc5511a PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

# Link sc5511a library from x64 directory
target_link_libraries(signalcore_sc5511a PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/x64/sc5511a.lib
)

# Set output name to match folder name
set_target_properties(signalcore_sc5511a PROPERTIES
    OUTPUT_NAME "signalcore_sc5511a"
    PREFIX ""
)

# Windows specific settings
if(WIN32)
    set_target_properties(signalcore_sc5511a PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()

# Install rules
install(TARGETS signalcore_sc5511a
    RUNTIME DESTINATION instruments/signalgenerator/signalcore_sc5511a
    LIBRARY DESTINATION instruments/signalgenerator/signalcore_sc5511a
)

install(FILES signalcore_sc5511a.json
    DESTINATION instruments/signalgenerator/signalcore_sc5511a
)

# Copy JSON file to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/signalcore_sc5511a.json
    ${CMAKE_CURRENT_BINARY_DIR}/signalcore_sc5511a.json
    COPYONLY
)

# Create ZIP package after build
if(WIN32)
    add_custom_command(TARGET signalcore_sc5511a POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:signalcore_sc5511a> ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/signalcore_sc5511a.json ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/x64 ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/x64
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Release/signalcore_sc5511a.exp ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Checking for PDB file to include in zip..."
        COMMAND if exist ${CMAKE_CURRENT_BINARY_DIR}/Debug/signalcore_sc5511a.pdb ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Debug/signalcore_sc5511a.pdb ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a' -DestinationPath '${CMAKE_CURRENT_BINARY_DIR}/signalcore_sc5511a-plugin.zip' -Force"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating signalcore_sc5511a-plugin.zip (DLL, JSON, and PDB if Debug)"
    )
else()
    add_custom_command(TARGET signalcore_sc5511a POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:signalcore_sc5511a> ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/signalcore_sc5511a.json ${CMAKE_CURRENT_BINARY_DIR}/package/signalcore_sc5511a/
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}/package ${CMAKE_COMMAND} -E tar czf ${CMAKE_CURRENT_BINARY_DIR}/signalcore_sc5511a-plugin.zip --format=zip signalcore_sc5511a
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating signalcore_sc5511a-plugin.zip"
    )
endif()
