add_library(display_mod STATIC display.cpp)
target_include_directories(display_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
