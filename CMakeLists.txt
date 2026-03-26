cmake_minimum_required(VERSION 3.10)

# Preferred Compiler Selection (Clang first, then GCC)
find_program(CLANG_PATH clang)
find_program(CLANGXX_PATH clang++)

if(CLANG_PATH AND CLANGXX_PATH)
    set(CMAKE_C_COMPILER ${CLANG_PATH})
    set(CMAKE_CXX_COMPILER ${CLANGXX_PATH})
else()
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

project(FreeMCP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Source files
set(SOURCES
    src/main.cpp
    src/persona.cpp
    src/memory.cpp
    src/Core/SystemDiscovery.cpp
    src/Core/OllamaClient.cpp
    src/Core/ToolExecutor.cpp
    src/Core/PromptBuilder.cpp
    src/Core/ChatEngine.cpp
    src/Core/Utils.cpp
    src/UI/StoreApp.cpp
    src/UI/StoreMainWindow.cpp
    src/UI/Views/HomeView.cpp
    src/UI/Views/BrainView.cpp
    src/UI/Views/ChatView.cpp
    src/UI/Views/LogsView.cpp
    src/UI/Views/SettingsView.cpp
)

# Main executable
add_executable(FreeMCP ${SOURCES})

# Include directories
target_include_directories(FreeMCP PRIVATE include ${GTK3_INCLUDE_DIRS})

# Link libraries
target_link_libraries(FreeMCP PRIVATE ${GTK3_LIBRARIES})

# Subdirectories for modules
add_subdirectory(Module/archcore)
