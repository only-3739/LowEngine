cmake_minimum_required(VERSION 3.24)

project(LowEngineBuild LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED CONFIG)

# FTXUI
include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v7.0.3
)

FetchContent_MakeAvailable(ftxui)

# vk-bootstrap
add_subdirectory(external/vk-bootstrap)

# Executable
add_executable(${PROJECT_NAME}
    src/main.cxx
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
    vk-bootstrap::vk-bootstrap
    ftxui::ftxui
)

# Compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

