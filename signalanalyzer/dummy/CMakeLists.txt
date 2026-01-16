cmake_minimum_required(VERSION 3.16)
project(DummySignalAnalyzer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plugin source files
set(PLUGIN_SOURCES
    dummysignalanalyzer.cpp
)

set(PLUGIN_HEADERS
    dummysignalanalyzer.h
    ../../iplugininterface.h
)

# Create shared library (DLL)
add_library(dummy SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# Include directories
target_include_directories(dummy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

# Set output name to match folder name
set_target_properties(dummy PROPERTIES
    OUTPUT_NAME "dummy"
    PREFIX ""
)

# Windows specific settings
if(WIN32)
    set_target_properties(dummy PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()

# Install rules
install(TARGETS dummy
    RUNTIME DESTINATION instruments/signalanalyzer/dummy
    LIBRARY DESTINATION instruments/signalanalyzer/dummy
)

install(FILES dummy.json
    DESTINATION instruments/signalanalyzer/dummy
)

# Copy JSON file to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json
    ${CMAKE_CURRENT_BINARY_DIR}/dummy.json
    COPYONLY
)

# Create ZIP package after build
if(WIN32)
    add_custom_command(TARGET dummy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/dummy
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummy> ${CMAKE_CURRENT_BINARY_DIR}/package/dummy/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json ${CMAKE_CURRENT_BINARY_DIR}/package/dummy/
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_CURRENT_BINARY_DIR}/package/dummy' -DestinationPath '${CMAKE_CURRENT_BINARY_DIR}/dummy-plugin.zip' -Force"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating dummy-plugin.zip"
    )
else()
    add_custom_command(TARGET dummy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/dummy
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dummy> ${CMAKE_CURRENT_BINARY_DIR}/package/dummy/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummy.json ${CMAKE_CURRENT_BINARY_DIR}/package/dummy/
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}/package ${CMAKE_COMMAND} -E tar czf ${CMAKE_CURRENT_BINARY_DIR}/dummy-plugin.zip --format=zip dummy
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating dummy-plugin.zip"
    )
endif()
