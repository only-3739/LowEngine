cmake_minimum_required(VERSION 3.24)

project(LowEngineBuild LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(Vulkan 1.3 REQUIRED COMPONENTS glslc)
find_package(SDL3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)

# vk-bootstrap
add_subdirectory(external/vk-bootstrap External/VkBootstrap)

# Executable
add_executable(${PROJECT_NAME}
    src/main.cxx
    src/Core/LifeCycle.cxx
    src/Graphic/Vulkan/VkEngine.cxx
    src/Graphic/Vulkan/VkImages.cxx
    src/Graphic/Vulkan/VkInitializer.cxx
    src/SDL/Window/WindowCore.cxx
    src/SDL/Input/InputManager.cxx
    src/Debug/Debug.cxx
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
    fmt::fmt
    vk-bootstrap::vk-bootstrap
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:LE_DEBUG>
)

