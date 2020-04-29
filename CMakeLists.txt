cmake_minimum_required(VERSION 3.12)

project(BuildScript VERSION 0.1.0
                    LANGUAGES CXX)

# Detect OSTYPE.
# TODO: Add other OSes.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OSTYPE "Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OSTYPE "MacOS")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OSTYPE "Windows")
else ()
    message(FATAL_ERROR "Unknown host OS: ${CMAKE_SYSTEM_NAME}")
endif () # OSTYPE

# Detect ARCHTYPE
# TODO: Add other architectures.
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
    set(ARCHTYPE "x64")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i[1-9]86")
    set(ARCHTYPE "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "arm|ARM")
    set(ARCHTYPE "ARM")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|AARCH64|arm64|ARM64")
    set(ARCHTYPE "AArch64")
else ()
    message(FATAL_ERROR "Unknown host architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Packages
find_package(Boost 1.71.0 REQUIRED COMPONENTS filesystem iostreams)

# Sources
add_library(Library SHARED
                    # Generated sources
                    ${CMAKE_CURRENT_BINARY_DIR}/Version.generated.cpp)

# Compile options
set_target_properties(Library PROPERTIES
                              CXX_STANDARD 11
                              CXX_STANDARD_REQUIRED ON
                              CXX_EXTENSIONS OFF
                              OUTPUT_NAME buildscript.${CMAKE_BUILD_TYPE}
                              OUTPUT_NAME_RELEASE buildscript)

if (MSVC)
    target_compile_options(Library PRIVATE /utf-8)
endif() # MSVC

target_include_directories(Library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header)

target_link_libraries(Library PUBLIC
                              Boost::boost      # optional
                              Boost::filesystem
                              Boost::iostreams)

configure_file(Source/Version.cpp.in Version.generated.cpp)