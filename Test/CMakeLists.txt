# Packages
find_package(Catch2 2.11 REQUIRED)

# Sources
add_executable(Test Driver.cpp
                    # Compiler tests
                    Compiler/SourceTextTest.cpp

                    # Utils tests
                    Utils/EncodingTest.cpp)

# Compile options
set_target_properties(Test PROPERTIES
                           CXX_STANDARD 11
                           CXX_STANDARD_REQUIRED ON
                           CXX_EXTENSIONS OFF
                           OUTPUT_NAME buildscript.test.${CMAKE_BUILD_TYPE}
                           COMPILE_DEFINITIONS TEST_SRC=u8\"${CMAKE_CURRENT_SOURCE_DIR}\")

if (MSVC)
    target_compile_options(Test PRIVATE /utf-8)
endif() # MSVC

target_link_libraries(Test PRIVATE Library Catch2::Catch2)

include(CTest)
include(Catch)

# Find tests
catch_discover_tests(Test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
