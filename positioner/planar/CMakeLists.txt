cmake_minimum_required(VERSION 3.16)
project(DummyPositioner VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plugin source files
set(PLUGIN_SOURCES
    dummypositioner.cpp
    ../../socket_instrument.cpp
)

set(PLUGIN_HEADERS
    dummypositioner.h
    ../../iplugininterface.h
    ../../socket_instrument.h
)

# Create shared library (DLL)
add_library(dummypositioner SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# Include directories
target_include_directories(dummypositioner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

if(WIN32)
    target_link_libraries(dummypositioner ws2_32)
endif()

# Set output name to match folder name
set_target_properties(dummypositioner PROPERTIES
    OUTPUT_NAME "dummypositioner"
    PREFIX ""
)

# Windows specific settings
if(WIN32)
    set_target_properties(dummypositioner PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()

# Install rules
install(TARGETS dummypositioner
    RUNTIME DESTINATION instruments/positioner/dummypositioner
    LIBRARY DESTINATION instruments/positioner/dummypositioner
)

install(FILES dummypositioner.json
    DESTINATION instruments/positioner/dummypositioner
)

# Copy JSON file to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/dummypositioner.json
    ${CMAKE_CURRENT_BINARY_DIR}/dummypositioner.json
    COPYONLY
)

# Create ZIP package after build
if(WIN32)
    add_custom_command(TARGET dummypositioner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummypositioner> ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummypositioner.json ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner/
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Checking for PDB file to include in zip..."
        COMMAND if exist ${CMAKE_CURRENT_BINARY_DIR}/Debug/dummypositioner.pdb ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Debug/dummypositioner.pdb ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner/
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner' -DestinationPath '${CMAKE_CURRENT_BINARY_DIR}/dummypositioner-plugin.zip' -Force"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating dummypositioner-plugin.zip (DLL, JSON, and PDB if Debug)"
    )
else()
    add_custom_command(TARGET dummypositioner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummypositioner> ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummypositioner.json ${CMAKE_CURRENT_BINARY_DIR}/package/dummypositioner/
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}/package ${CMAKE_COMMAND} -E tar czf ${CMAKE_CURRENT_BINARY_DIR}/dummypositioner-plugin.zip --format=zip dummypositioner
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating dummypositioner-plugin.zip"
    )
endif()
