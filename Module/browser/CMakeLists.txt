add_library(browser STATIC
    browser.cpp
)

target_include_directories(browser PUBLIC ${CMAKE_SOURCE_DIR}/include)
