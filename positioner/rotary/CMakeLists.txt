cmake_minimum_required(VERSION 3.16)
project(Rotary VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plugin source files
set(PLUGIN_SOURCES
    rotary.cpp
    ../../socket_instrument.cpp
)

set(PLUGIN_HEADERS
    rotary.h
    ../../iplugininterface.h
    ../../socket_instrument.h
)
    
# Create shared library (DLL)
add_library(rotary SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# Include directories
target_include_directories(rotary PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

if(WIN32)
    target_link_libraries(rotary ws2_32)
endif()

# Set output name to match folder name
set_target_properties(rotary PROPERTIES
    OUTPUT_NAME "rotary"
    PREFIX ""
)

# Windows specific settings
if(WIN32)
    set_target_properties(rotary PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()

# Install rules
install(TARGETS rotary
    RUNTIME DESTINATION instruments/positioner/rotary
    LIBRARY DESTINATION instruments/positioner/rotary
)

install(FILES rotary.json
    DESTINATION instruments/positioner/rotary
)

# Copy JSON file to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/rotary.json
    ${CMAKE_CURRENT_BINARY_DIR}/rotary.json
    COPYONLY
)

# Create ZIP package after build
if(WIN32)
    add_custom_command(TARGET rotary POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/rotary
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rotary> ${CMAKE_CURRENT_BINARY_DIR}/package/rotary/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/rotary.json ${CMAKE_CURRENT_BINARY_DIR}/package/rotary/
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Checking for PDB file to include in zip..."
        COMMAND if exist ${CMAKE_CURRENT_BINARY_DIR}/Debug/rotary.pdb ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Debug/rotary.pdb ${CMAKE_CURRENT_BINARY_DIR}/package/rotary/
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_CURRENT_BINARY_DIR}/package/rotary' -DestinationPath '${CMAKE_CURRENT_BINARY_DIR}/rotary-plugin.zip' -Force"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating rotary-plugin.zip (DLL, JSON, and PDB if Debug)"
    )
else()
    add_custom_command(TARGET rotary POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/rotary
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rotary> ${CMAKE_CURRENT_BINARY_DIR}/package/rotary/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/rotary.json ${CMAKE_CURRENT_BINARY_DIR}/package/rotary/
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}/package ${CMAKE_COMMAND} -E tar czf ${CMAKE_CURRENT_BINARY_DIR}/rotary-plugin.zip --format=zip rotary
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating rotary-plugin.zip"
    )
endif()
