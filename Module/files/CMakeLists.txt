add_library(files_mod STATIC files.cpp)
target_include_directories(files_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
