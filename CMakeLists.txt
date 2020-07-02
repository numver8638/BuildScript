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
if (!MSVC)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost 1.71.0 REQUIRED COMPONENTS filesystem iostreams)
find_package(fmt 6.2.0 REQUIRED)

# Sources
add_library(Library SHARED
                    # Compiler sources
                    Source/Compiler/AST/ASTVisitor.cpp
                    Source/Compiler/ErrorReporter.ReportID.cpp
                    Source/Compiler/Lexer.cpp
                    Source/Compiler/Parser.cpp
                    Source/Compiler/Parser.ClassMember.cpp
                    Source/Compiler/Parser.Statement.cpp
                    Source/Compiler/Parser.Declaration.cpp
                    Source/Compiler/Parser.Expression.cpp
                    Source/Compiler/SourceText.cpp
                    Source/Compiler/StringRef.cpp

                    # Debug code sources
                    Source/Debug/ASTDumper.cpp
                    Source/Debug/ASTDumper.ClassMember.cpp
                    Source/Debug/ASTDumper.Declaration.cpp
                    Source/Debug/ASTDumper.Expression.cpp
                    Source/Debug/ASTDumper.Statement.cpp
                    Source/Debug/NodeWriter.cpp

                    # Utility sources
                    Source/Utils/Encoding.cpp
                    Source/Utils/Encoding/EUCKR.ConversionTable.cpp
                    Source/Utils/Encoding/EUCKR.cpp
                    Source/Utils/Encoding/UTF8.cpp
                    Source/Utils/Encoding/UTF16.cpp
                    Source/Utils/Encoding/UTF32.cpp
                    
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
    target_compile_options(Library PRIVATE /utf-8   # Accept source as UTF-8.
                                           /wd4275  # Ignore warning when __declspec(dllexport)-ed class inherits
                                                    # non __declspec(dllexport)-ed class.
                                           /wd4251) # Ignore warning when member variable doen't have __declspec(dllexport)
                                                    # in __declspec(dllexport)-ed class.
    target_compile_definitions(Library PRIVATE   EXPORT_API=__declspec\(dllexport\)
                                       INTERFACE EXPORT_API=__declspec\(dllimport\))
else()
    target_compile_definitions(Library PUBLIC EXPORT_API=)
endif() # MSVC

target_include_directories(Library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Header)

target_link_libraries(Library PUBLIC
                              Boost::boost      # optional, endian
                              Boost::filesystem
                              Boost::iostreams
                              fmt::fmt)

configure_file(Source/Version.cpp.in Version.generated.cpp)

add_subdirectory(Driver)

enable_testing()
add_subdirectory(Test)