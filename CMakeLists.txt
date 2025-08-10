cmake_minimum_required(VERSION 3.16)
project(RCS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if Qt is available
find_package(Qt6 COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets)
    if(NOT Qt5_FOUND)
        message(STATUS "Qt not found, building console version")
        set(BUILD_CONSOLE_VERSION ON)
    else()
        set(QT_VERSION_MAJOR 5)
    endif()
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Set executable name
set(TARGET_NAME rcs)

if(BUILD_CONSOLE_VERSION)
    # Console version source files
    set(SOURCES
        main_console.cpp
        mousecontroller_console.cpp
        macrorecorder_console.cpp
        console_ui.cpp
    )
    
    set(HEADERS
        mousecontroller_console.h
        macrorecorder_console.h
        console_ui.h
    )
    
    # Platform-specific libraries for console version
    if(WIN32)
        target_link_libraries(${TARGET_NAME} user32)
    elseif(UNIX AND NOT APPLE)
        find_package(X11)
        if(X11_FOUND)
            target_link_libraries(${TARGET_NAME} ${X11_LIBRARIES})
            target_include_directories(${TARGET_NAME} PRIVATE ${X11_INCLUDE_DIR})
        endif()
    endif()
    
else()
    # Qt version source files
    set(SOURCES
        main.cpp
        mainwindow.cpp
        mousecontroller.cpp
        macrorecorder.cpp
    )
    
    set(HEADERS
        mainwindow.h
        mousecontroller.h
        macrorecorder.h
    )
    
    # Enable automatic MOC, UIC, and RCC
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    
    # Link Qt libraries
    if(QT_VERSION_MAJOR EQUAL 6)
        target_link_libraries(${TARGET_NAME} Qt6::Core Qt6::Widgets)
    else()
        target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Widgets)
    endif()
    
    # Platform-specific settings for Qt version
    if(WIN32)
        target_link_libraries(${TARGET_NAME} user32)
    elseif(UNIX AND NOT APPLE)
        find_package(X11)
        if(X11_FOUND)
            target_link_libraries(${TARGET_NAME} ${X11_LIBRARIES})
            target_include_directories(${TARGET_NAME} PRIVATE ${X11_INCLUDE_DIR})
        endif()
    endif()
    
    # Copy style sheet to build directory
    configure_file(${CMAKE_SOURCE_DIR}/style.qss ${CMAKE_BINARY_DIR}/bin/style.qss COPYONLY)
endif()

# Create executable
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)