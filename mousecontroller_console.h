#pragma once

#include <chrono>
#include <functional>

struct Point {
    int x, y;
    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Point& other) const { return !(*this == other); }
};

enum class MouseButton {
    Left,
    Right,
    Middle
};

class MouseController
{
public:
    MouseController();
    ~MouseController();

    // Mouse position functions
    Point getCurrentMousePosition() const;
    void setMousePosition(const Point &position);
    
    // Mouse click functions
    void leftClick(const Point &position = Point());
    void rightClick(const Point &position = Point());
    void doubleClick(const Point &position = Point());
    
    // Mouse state
    bool isLeftButtonPressed() const;
    bool isRightButtonPressed() const;
    
    // Callbacks for position and click events
    void setPositionCallback(std::function<void(const Point&)> callback);
    void setClickCallback(std::function<void(MouseButton, const Point&)> callback);
    
    // Start/stop position monitoring
    void startMonitoring();
    void stopMonitoring();

private:
    void initializePlatformSpecific();
    void cleanupPlatformSpecific();
    void performClick(MouseButton button, const Point &position);
    void updateMousePosition();
    
    Point m_currentPosition;
    Point m_lastPosition;
    bool m_monitoring;
    
    std::function<void(const Point&)> m_positionCallback;
    std::function<void(MouseButton, const Point&)> m_clickCallback;
    
#ifdef _WIN32
    // Windows-specific members
#elif defined(__linux__)
    void* m_display; // Display* but avoiding X11 headers here
#endif
};