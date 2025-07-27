cmake_minimum_required(VERSION 3.16)
project(CDN_System)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# Try to find cpprestsdk
find_package(cpprestsdk REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create directories for different components
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)

# Add subdirectories
add_subdirectory(src/common)
add_subdirectory(src/origin_server)
add_subdirectory(src/meta_server)
add_subdirectory(src/cdn_node)
add_subdirectory(src/fss)
add_subdirectory(src/client)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)