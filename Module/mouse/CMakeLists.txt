add_library(mouse_mod STATIC mouse.cpp)
target_include_directories(mouse_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
