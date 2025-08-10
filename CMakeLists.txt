cmake_minimum_required(VERSION 3.16)
project(RCS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

# Check for X11 on Linux
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
endif()

# ImGui source files
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# Platform-specific ImGui backends
if(WIN32)
    list(APPEND IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    )
else()
    list(APPEND IMGUI_SOURCES
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )
endif()

# Main application sources
set(APP_SOURCES
    src/main.cpp
    src/recoil_control.cpp
    src/weapon_profiles.cpp
    src/muzzle_flash_detector.cpp
    src/ui_manager.cpp
    src/input_controller.cpp
)

# Include directories
include_directories(
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    src/
)

# Create executable
add_executable(RCS ${APP_SOURCES} ${IMGUI_SOURCES})

# Platform-specific linking
if(WIN32)
    target_link_libraries(RCS 
        user32 
        kernel32 
        gdi32 
        dwmapi 
        d3d11 
        dxgi
    )
else()
    # For Linux demonstration - using GLFW + OpenGL
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    target_link_libraries(RCS 
        ${OPENGL_LIBRARIES}
        ${GLFW3_LIBRARIES}
        ${X11_LIBRARIES}
        dl
        pthread
    )
    target_include_directories(RCS PRIVATE ${GLFW3_INCLUDE_DIRS})
    target_compile_options(RCS PRIVATE ${GLFW3_CFLAGS_OTHER})
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(RCS PRIVATE /W4)
else()
    target_compile_options(RCS PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add preprocessor definitions
if(WIN32)
    target_compile_definitions(RCS PRIVATE PLATFORM_WINDOWS=1)
else()
    target_compile_definitions(RCS PRIVATE PLATFORM_LINUX=1)
endif()