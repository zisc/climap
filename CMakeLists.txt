# Builds tests and the example. To use CLIMap, simply
# #include "CLIMap.hpp" in your project.
# This CMakeLists.txt is not required.

cmake_minimum_required( VERSION 3.7 )

if("${STD}" STREQUAL "")
    set( STD c++11 )
endif()

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${STD}" )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
add_subdirectory( test )
add_subdirectory( example )

