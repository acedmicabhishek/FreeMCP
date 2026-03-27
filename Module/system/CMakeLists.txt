add_library(system_mod STATIC system.cpp)
target_include_directories(system_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
