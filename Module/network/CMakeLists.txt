add_library(network_mod STATIC network.cpp)
target_include_directories(network_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
