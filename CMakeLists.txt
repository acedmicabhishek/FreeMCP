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
    src/Core/SystemDiscovery.cpp
    src/Core/OllamaClient.cpp
    src/Core/ToolExecutor.cpp
    src/Core/PromptBuilder.cpp
    src/Core/ChatEngine.cpp
    src/Core/SystemShell.cpp
    src/Core/Utils.cpp
    src/Core/NetListener.cpp
    src/Core/ConfigManager.cpp
    src/Core/IntentClassifier.cpp
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

# Link libraries — all modules
target_link_libraries(FreeMCP PRIVATE
    ${GTK3_LIBRARIES}
    browser
    system_mod
    files_mod
    network_mod
    audio_mod
    display_mod
    clipboard_mod
    notify_mod
    power_mod
    mouse_mod
)

# Subdirectories for modules
add_subdirectory(Module/archcore)
add_subdirectory(Module/browser)
add_subdirectory(Module/system)
add_subdirectory(Module/files)
add_subdirectory(Module/network)
add_subdirectory(Module/audio)
add_subdirectory(Module/display)
add_subdirectory(Module/clipboard)
add_subdirectory(Module/notify)
add_subdirectory(Module/power)
add_subdirectory(Module/mouse)
