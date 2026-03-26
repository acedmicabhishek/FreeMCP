add_library(archcore STATIC
    root/root.cpp
    rwx/rwx.cpp
    terminal/terminal.cpp
)

target_include_directories(archcore PUBLIC ${CMAKE_SOURCE_DIR}/include)
