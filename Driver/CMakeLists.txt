# Packages
find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)

# Sources
add_executable(Driver Driver.cpp)

# Compile options
set_target_properties(Driver PROPERTIES
                             CXX_STANDARD 11
                             CXX_STANDARD_REQUIRED ON
                             CXX_EXTENSIONS OFF
                             OUTPUT_NAME buildscript.${CMAKE_BUILD_TYPE}
                             OUTPUT_NAME_RELEASE buildscript)

if (MSVC)
    target_compile_options(Driver PRIVATE /utf-8)
endif() # MSVC

target_link_libraries(Driver PRIVATE Library Boost::program_options)