add_library(notify_mod STATIC notify.cpp)
target_include_directories(notify_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
