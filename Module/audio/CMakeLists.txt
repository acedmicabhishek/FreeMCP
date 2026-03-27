add_library(audio_mod STATIC audio.cpp)
target_include_directories(audio_mod PUBLIC ${CMAKE_SOURCE_DIR}/include)
