add_library(clipboard_mod STATIC clipboard.cpp)
target_include_directories(clipboard_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
