cmake_minimum_required(VERSION 3.16)
project(Planar VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Plugin source files
set(PLUGIN_SOURCES
    planar.cpp
    ../../socket_instrument.cpp
)

set(PLUGIN_HEADERS
    planar.h
    ../../socket_instrument.h
    ../../iplugininterface.h
)

# Create shared library (DLL)
add_library(planar SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

# Include directories
target_include_directories(planar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

# Set output name to match folder name
set_target_properties(planar PROPERTIES
    OUTPUT_NAME "planar"
    PREFIX ""
)

# Windows specific settings
if(WIN32)
    set_target_properties(planar PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}/Release"
    )
endif()

# Install rules
install(TARGETS planar
    RUNTIME DESTINATION instruments/positioner/planar
    LIBRARY DESTINATION instruments/positioner/planar
)

install(FILES planar.json
    DESTINATION instruments/positioner/planar
)

# Copy JSON file to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/planar.json
    ${CMAKE_CURRENT_BINARY_DIR}/planar.json
    COPYONLY
)

# Create ZIP package after build
if(WIN32)
    add_custom_command(TARGET planar POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/planar
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:planar> ${CMAKE_CURRENT_BINARY_DIR}/package/planar/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/planar.json ${CMAKE_CURRENT_BINARY_DIR}/package/planar/
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Checking for PDB file to include in zip..."
        COMMAND if exist ${CMAKE_CURRENT_BINARY_DIR}/Debug/planar.pdb ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/Debug/planar.pdb ${CMAKE_CURRENT_BINARY_DIR}/package/planar/
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_CURRENT_BINARY_DIR}/package/planar' -DestinationPath '${CMAKE_CURRENT_BINARY_DIR}/planar-plugin.zip' -Force"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating planar-plugin.zip (DLL, JSON, and PDB if Debug)"
    )
else()
    add_custom_command(TARGET planar POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/package/planar
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:planar> ${CMAKE_CURRENT_BINARY_DIR}/package/planar/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/planar.json ${CMAKE_CURRENT_BINARY_DIR}/package/planar/
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_BINARY_DIR}/package ${CMAKE_COMMAND} -E tar czf ${CMAKE_CURRENT_BINARY_DIR}/planar-plugin.zip --format=zip planar
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/package
        COMMENT "Creating planar-plugin.zip"
    )
endif()
