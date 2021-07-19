find_package(Catch2 REQUIRED)

add_executable(TestDriver
               Driver.cpp

               # Test files
               Utils/ConvertTest.cpp
               )

set_target_properties(TestDriver PROPERTIES CXX_STANDARD 17)

target_link_libraries(TestDriver Catch2::Catch2 BuildScript)

add_custom_target(TestCopyDLL ALL
                  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BuildScript> ${CMAKE_CURRENT_BINARY_DIR}
                  DEPENDS $<TARGET_FILE:BuildScript>)

add_dependencies(TestDriver TestCopyDLL)

include(CTest)
include(Catch)

catch_discover_tests(TestDriver WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
