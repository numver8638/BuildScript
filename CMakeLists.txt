cmake_minimum_required(VERSION 3.12)

# Use local vcpkg instance
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Check Host arch
cmake_host_system_information(RESULT __ARCH QUERY OS_PLATFORM)
if (__ARCH MATCHES "^(i[3-6]|x|X)86$")
    set(__VCPKG_TARGET_ARCH "x86")
elseif (__ARCH MATCHES "^(amd|AMD|[xX]86[-_])64$")
    set(__VCPKG_TARGET_ARCH "x64")
elseif (__ARCH MATCHES "^(arm|ARM|aarch|AARCH)64$")
    set(__VCPKG_TARGET_ARCH "arm64")
else ()
    message(FATAL_ERROR "Unknown/Unsupported host architecture: ${__ARCH}")
endif ()

# Check Host OS
if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(__VCPKG_TARGET_OS windows-static-md)
    set(TARGET_OS "Windows")
    set(TARGET_OS_DEFINE "OS_WINDOWS")
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set(__VCPKG_TARGET_OS linux)
    set(TARGET_OS "Linux")
    set(TARGET_OS_DEFINE "OS_LINUX")
elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    set(__VCPKG_TARGET_OS osx)
    set(TARGET_OS "macOS")
    set(TARGET_OS_DEFINE "OS_MACOS")
else ()
    message(FATAL_ERROR "Unknown/Unsupported host system: ${CMAKE_HOST_SYSTEM_NAME}")
endif ()

set(VCPKG_TARGET_TRIPLET ${__VCPKG_TARGET_ARCH}-${__VCPKG_TARGET_OS})
unset(__VCPKG_TARGET_ARCH)
unset(__VCPKG_TARGET_OS)
unset(__ARCH)

message(STATUS "VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")

# Project
project(BuildScript VERSION 0.0.1
                    LANGUAGES CXX)

set(BOOST_VERSION "1.78.0")
set(FMT_VERSION "8.1.1")

# Find required packages
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost ${BOOST_VERSION} REQUIRED iostreams)
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
                      PRIVATE Boost::boost Boost::iostreams
                      PUBLIC  fmt::fmt)

target_compile_definitions(BuildScript
                           PRIVATE "$<BUILD_INTERFACE:ON_BUILD>" ${TARGET_OS_DEFINE})

set(PUBLIC_HEADERS
    ## Compiler
    BuildScript/Compiler/ErrorReporter.h
    BuildScript/Compiler/SourcePosition.h
    BuildScript/Compiler/SourceText.h

    ## GC
    BuildScript/GC/Allocator.h
    BuildScript/GC/Handle.h
    BuildScript/GC/Heap.h
    BuildScript/GC/Object.h
    BuildScript/GC/Options.h
    BuildScript/GC/Rootable.h
    BuildScript/GC/Tracker.h
    BuildScript/GC/Vector.h
    ## Utils
    BuildScript/Utils/AllStatic.h
    BuildScript/Utils/BitCast.h
    BuildScript/Utils/Encoding.h
    BuildScript/Utils/NonCopyable.h
    BuildScript/Utils/NonMovable.h
    BuildScript/Utils/Singleton.h
    BuildScript/Utils/TrailObjects.h

    BuildScript/Assert.h
    BuildScript/Config.h
    BuildScript/Version.h)

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

## Configure package
include(CmakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/BuildScriptConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/BuildScript
                              NO_SET_AND_CHECK_MACRO)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/BuildScriptConfigVersion.cmake
                                 COMPATIBILITY SameMajorVersion)

## Install
function(install_files)
    cmake_parse_arguments(_PREFIX "" "DIRECTORY;DESTINATION" "FILES" ${ARGN})

    foreach(_FILE ${_PREFIX_FILES})
        string(REGEX MATCH "(.*)[/\\]" _DIR ${_FILE})
        install(FILES ${_PREFIX_DIRECTORY}${_FILE} DESTINATION ${_PREFIX_DESTINATION}/${_DIR})
    endforeach(_FILE)
endfunction(install_files)

install(TARGETS BuildScript
        CONFIGURATIONS Debug Release
        EXPORT BuildScriptTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(TARGETS Driver
        CONFIGURATIONS Release
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install_files(DIRECTORY Header/
              DESTINATION ${CMAKE_INSTALL_PREFIX}/include
              FILES ${PUBLIC_HEADERS})

install(EXPORT BuildScriptTargets
        CONFIGURATIONS Debug Release
        FILE BuildScriptTargets.cmake
        NAMESPACE BuildScript::
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/BuildScript)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BuildScriptConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/BuildScriptConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/BuildScript)

# Lists of sources
target_sources(BuildScript PRIVATE
               # ==== Compiler ====
               ## Analysis
               Source/Compiler/Analysis/Scope.cpp
               Source/Compiler/Analysis/SemanticAnalyzer.cpp
               Source/Compiler/Analysis/SemanticAnalyzer.Declaration.cpp
               Source/Compiler/Analysis/SemanticAnalyzer.Expression.cpp
               Source/Compiler/Analysis/SemanticAnalyzer.Statement.cpp

               ## AST
               Source/Compiler/AST/ASTDumper.cpp
               Source/Compiler/AST/ASTWalker.cpp
               Source/Compiler/AST/Declarations.cpp
               Source/Compiler/AST/Expressions.cpp
               Source/Compiler/AST/OperatorKind.cpp
               Source/Compiler/AST/ParameterList.cpp
               Source/Compiler/AST/Statements.cpp

               ## Parser
               Source/Compiler/Parse/Lexer.cpp
               Source/Compiler/Parse/Lexer.KeywordTable.cpp
               Source/Compiler/Parse/Lexer.String.cpp
               Source/Compiler/Parse/Parser.Declaration.cpp
               Source/Compiler/Parse/Parser.Expression.cpp
               Source/Compiler/Parse/Parser.Statement.cpp
               Source/Compiler/Parse/Parser.Misc.cpp
               Source/Compiler/Parse/Parser.cpp
               Source/Compiler/Parse/ParserBase.cpp
               Source/Compiler/Parse/Token.cpp

               ## Symbol
               Source/Compiler/Symbol/Symbol.cpp

               Source/Compiler/ErrorReporter.cpp
               Source/Compiler/ManagedObject.cpp
               Source/Compiler/SourceText.cpp

               # ==== GC ====
               Source/GC/Impl/Noop.cpp
               Source/GC/Allocator.cpp
               Source/GC/Handle.cpp
               Source/GC/Heap.cpp
               Source/GC/Rootable.cpp

               # ==== Platform ====
               Source/Platform/MemoryChunk.cpp

               # ==== Utility ====
               Source/Utils/Convert.cpp
               Source/Utils/Encoding.cpp
               Source/Utils/Encoding/EUCKR.cpp
               Source/Utils/Encoding/UTF16.cpp
               Source/Utils/Encoding/UTF32.cpp
               Source/Utils/Encoding/UTF8.cpp
               Source/Utils/Hash.cpp

               # ==== Version ====
               ${CMAKE_CURRENT_BINARY_DIR}/Generated/Version.cpp)