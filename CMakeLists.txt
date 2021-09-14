cmake_minimum_required(VERSION 3.11)

# Check Host OS
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_TOOLCHAIN_FILE  "C:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET x64-windows-static-md)
    set(TARGET_OS "Windows")
    set(TARGET_OS_DEFINE "OS_WINDOWS")
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET x64-linux)
    set(TARGET_OS "Linux")
    set(TARGET_OS_DEFINE "OS_LINUX")
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(VCPKG_TARGET_TRIPLET x64-osx)
    set(TARGET_OS "macOS")
    set(TARGET_OS_DEFINE "OS_MACOS")
else ()
    message(FATAL_ERROR "Unknown/Unsupported host system.")
endif ()


# Project
project(BuildScript VERSION 0.0.1
                    LANGUAGES CXX)

set(BOOST_VERSION "1.75.0")
set(FMT_VERSION "7.1.0")

# Find required packages
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS filesystem iostreams)
find_package(fmt ${FMT_VERSION} REQUIRED)

# Create target - sources are listed bottom of CMakeLists.txt.
add_library(BuildScript SHARED)

# Set include path
target_include_directories(BuildScript
                           PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Header>"
                                  "$<INSTALL_INTERFACE:include>")

set_target_properties(BuildScript
                      PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSION OFF
                      OUTPUT_NAME buildscript
                      DEBUG_OUTPUT_NAME buildscript-debug)

configure_file(Source/Version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/Generated/Version.cpp @ONLY)

target_link_libraries(BuildScript
                      PRIVATE Boost::boost Boost::filesystem Boost::iostreams
                      PUBLIC  fmt::fmt)

target_compile_definitions(BuildScript
                           PRIVATE "$<BUILD_INTERFACE:ON_BUILD>" ${TARGET_OS_DEFINE})

# Add compiler specific options
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(BuildScript
                           PUBLIC
                           /utf-8   # Set source/output encoding to UTF-8
                           /wd4275  # Suppress warning on derived class that
                                    # inherits dllexport-ed class is not dll-exported.
                           /wd4251  # Suppress warning on dllexport-ed class has members
                                    # that is not dllexport-ed.
                           /wd26812 # Suppress warning on recommend using `enum class` rather than `enum'.
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(BuildScript
                           PRIVATE -fvisibility=hidden -fno-exceptions)
else ()
    message(FATAL_ERROR "Unknown/Unsupported compiler.")
endif ()

add_subdirectory(Test)

enable_testing()

# Lists of sources
target_sources(BuildScript PRIVATE
               # ==== Compiler ====
               Source/Compiler/SourceText.cpp

               # ==== Utility ====
               Source/Utils/Convert.cpp
               Source/Utils/Encoding.cpp
               Source/Utils/Encoding/EUCKR.cpp
               Source/Utils/Encoding/UTF16.cpp
               Source/Utils/Encoding/UTF32.cpp
               Source/Utils/Encoding/UTF8.cpp

               # ==== Version ====
               ${CMAKE_CURRENT_BINARY_DIR}/Generated/Version.cpp)