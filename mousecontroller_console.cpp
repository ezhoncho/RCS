#include "mousecontroller_console.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
// X11 includes only if available
#ifdef HAVE_X11
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <unistd.h>
#endif
#endif

MouseController::MouseController()
    : m_currentPosition(0, 0)
    , m_lastPosition(0, 0)
    , m_monitoring(false)
#ifdef __linux__
    , m_display(nullptr)
#endif
{
    initializePlatformSpecific();
}

MouseController::~MouseController()
{
    stopMonitoring();
    cleanupPlatformSpecific();
}

void MouseController::initializePlatformSpecific()
{
#ifdef __linux__
#ifdef HAVE_X11
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        std::cerr << "Warning: Failed to open X11 display. Mouse operations will be simulated." << std::endl;
    }
#else
    std::cout << "Note: X11 not available. Mouse operations will be simulated." << std::endl;
#endif
#endif
}

void MouseController::cleanupPlatformSpecific()
{
#ifdef __linux__
#ifdef HAVE_X11
    if (m_display) {
        XCloseDisplay(static_cast<Display*>(m_display));
        m_display = nullptr;
    }
#endif
#endif
}

Point MouseController::getCurrentMousePosition() const
{
#ifdef _WIN32
    POINT p;
    if (GetCursorPos(&p)) {
        return Point(p.x, p.y);
    }
#elif defined(__linux__)
#ifdef HAVE_X11
    if (m_display) {
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        if (XQueryPointer(static_cast<Display*>(m_display), DefaultRootWindow(static_cast<Display*>(m_display)),
                         &root, &child, &root_x, &root_y, &win_x, &win_y, &mask)) {
            return Point(root_x, root_y);
        }
    }
#endif
#endif
    
    // Fallback: return cached position or simulate
    return m_currentPosition;
}

void MouseController::setMousePosition(const Point &position)
{
    std::cout << "Moving mouse to (" << position.x << ", " << position.y << ")" << std::endl;
    
#ifdef _WIN32
    SetCursorPos(position.x, position.y);
#elif defined(__linux__)
#ifdef HAVE_X11
    if (m_display) {
        XWarpPointer(static_cast<Display*>(m_display), None, DefaultRootWindow(static_cast<Display*>(m_display)), 
                     0, 0, 0, 0, position.x, position.y);
        XFlush(static_cast<Display*>(m_display));
    }
#endif
#endif
    
    m_currentPosition = position;
    if (m_positionCallback) {
        m_positionCallback(position);
    }
}

void MouseController::leftClick(const Point &position)
{
    performClick(MouseButton::Left, position);
}

void MouseController::rightClick(const Point &position)
{
    performClick(MouseButton::Right, position);
}

void MouseController::doubleClick(const Point &position)
{
    if (position.x != 0 || position.y != 0) {
        setMousePosition(position);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    performClick(MouseButton::Left, position);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    performClick(MouseButton::Left, position);
}

void MouseController::performClick(MouseButton button, const Point &position)
{
    if (position.x != 0 || position.y != 0) {
        setMousePosition(position);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    Point clickPos = (position.x == 0 && position.y == 0) ? getCurrentMousePosition() : position;
    
    std::string buttonName;
    switch (button) {
    case MouseButton::Left:
        buttonName = "Left";
        break;
    case MouseButton::Right:
        buttonName = "Right";
        break;
    case MouseButton::Middle:
        buttonName = "Middle";
        break;
    }
    
    std::cout << buttonName << " click at (" << clickPos.x << ", " << clickPos.y << ")" << std::endl;
    
#ifdef _WIN32
    DWORD flags_down, flags_up;
    if (button == MouseButton::Left) {
        flags_down = MOUSEEVENTF_LEFTDOWN;
        flags_up = MOUSEEVENTF_LEFTUP;
    } else if (button == MouseButton::Right) {
        flags_down = MOUSEEVENTF_RIGHTDOWN;
        flags_up = MOUSEEVENTF_RIGHTUP;
    } else {
        return;
    }
    
    mouse_event(flags_down, 0, 0, 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mouse_event(flags_up, 0, 0, 0, 0);
    
#elif defined(__linux__)
#ifdef HAVE_X11
    if (m_display) {
        unsigned int xbutton;
        if (button == MouseButton::Left) {
            xbutton = Button1;
        } else if (button == MouseButton::Right) {
            xbutton = Button3;
        } else {
            return;
        }
        
        XTestFakeButtonEvent(static_cast<Display*>(m_display), xbutton, True, CurrentTime);
        XFlush(static_cast<Display*>(m_display));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        XTestFakeButtonEvent(static_cast<Display*>(m_display), xbutton, False, CurrentTime);
        XFlush(static_cast<Display*>(m_display));
    }
#endif
#endif
    
    if (m_clickCallback) {
        m_clickCallback(button, clickPos);
    }
}

bool MouseController::isLeftButtonPressed() const
{
#ifdef _WIN32
    return GetAsyncKeyState(VK_LBUTTON) & 0x8000;
#elif defined(__linux__)
#ifdef HAVE_X11
    if (m_display) {
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        XQueryPointer(static_cast<Display*>(m_display), DefaultRootWindow(static_cast<Display*>(m_display)),
                     &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
        return mask & Button1Mask;
    }
#endif
#endif
    return false;
}

bool MouseController::isRightButtonPressed() const
{
#ifdef _WIN32
    return GetAsyncKeyState(VK_RBUTTON) & 0x8000;
#elif defined(__linux__)
#ifdef HAVE_X11
    if (m_display) {
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        XQueryPointer(static_cast<Display*>(m_display), DefaultRootWindow(static_cast<Display*>(m_display)),
                     &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
        return mask & Button3Mask;
    }
#endif
#endif
    return false;
}

void MouseController::setPositionCallback(std::function<void(const Point&)> callback)
{
    m_positionCallback = callback;
}

void MouseController::setClickCallback(std::function<void(MouseButton, const Point&)> callback)
{
    m_clickCallback = callback;
}

void MouseController::startMonitoring()
{
    m_monitoring = true;
    std::cout << "Mouse monitoring started (simulated)" << std::endl;
}

void MouseController::stopMonitoring()
{
    m_monitoring = false;
    std::cout << "Mouse monitoring stopped" << std::endl;
}

void MouseController::updateMousePosition()
{
    if (m_monitoring) {
        Point currentPos = getCurrentMousePosition();
        if (currentPos != m_lastPosition) {
            m_lastPosition = currentPos;
            if (m_positionCallback) {
                m_positionCallback(currentPos);
            }
        }
    }
}