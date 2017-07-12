cmake_minimum_required( VERSION 3.7 )
find_package( Boost COMPONENTS unit_test_framework REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable( tests main.cpp KeyTest.cpp )
target_link_libraries( tests boost_unit_test_framework )

add_executable( map_test_manual MapTestManual.cpp)
