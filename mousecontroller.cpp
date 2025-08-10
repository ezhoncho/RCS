#include "mousecontroller.h"
#include <QCursor>
#include <QDebug>

#ifdef Q_OS_LINUX
#include <X11/Xutil.h>
#include <unistd.h>
#endif

MouseController::MouseController(QObject *parent)
    : QObject(parent)
    , m_positionTimer(new QTimer(this))
    , m_lastPosition(0, 0)
#ifdef Q_OS_LINUX
    , m_display(nullptr)
#endif
{
    initializePlatformSpecific();
    
    // Set up position tracking timer
    connect(m_positionTimer, &QTimer::timeout, this, &MouseController::updateMousePosition);
    m_positionTimer->start(50); // Update every 50ms
}

MouseController::~MouseController()
{
    cleanupPlatformSpecific();
}

void MouseController::initializePlatformSpecific()
{
#ifdef Q_OS_LINUX
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        qWarning() << "Failed to open X11 display";
    }
#endif
}

void MouseController::cleanupPlatformSpecific()
{
#ifdef Q_OS_LINUX
    if (m_display) {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
#endif
}

QPoint MouseController::getCurrentMousePosition() const
{
    return QCursor::pos();
}

void MouseController::setMousePosition(const QPoint &position)
{
#ifdef Q_OS_WIN
    SetCursorPos(position.x(), position.y());
#elif defined(Q_OS_LINUX)
    if (m_display) {
        XWarpPointer(m_display, None, DefaultRootWindow(m_display), 
                     0, 0, 0, 0, position.x(), position.y());
        XFlush(m_display);
    }
#else
    QCursor::setPos(position);
#endif
}

void MouseController::leftClick(const QPoint &position)
{
    performClick(Qt::LeftButton, position);
}

void MouseController::rightClick(const QPoint &position)
{
    performClick(Qt::RightButton, position);
}

void MouseController::doubleClick(const QPoint &position)
{
    if (!position.isNull()) {
        setMousePosition(position);
        QApplication::processEvents();
        usleep(10000); // 10ms delay
    }
    
    performClick(Qt::LeftButton, position);
    usleep(50000); // 50ms delay between clicks
    performClick(Qt::LeftButton, position);
}

void MouseController::performClick(Qt::MouseButton button, const QPoint &position)
{
    if (!position.isNull()) {
        setMousePosition(position);
        QApplication::processEvents();
        usleep(10000); // Small delay after position change
    }
    
    QPoint clickPos = position.isNull() ? getCurrentMousePosition() : position;
    
#ifdef Q_OS_WIN
    DWORD flags_down, flags_up;
    if (button == Qt::LeftButton) {
        flags_down = MOUSEEVENTF_LEFTDOWN;
        flags_up = MOUSEEVENTF_LEFTUP;
    } else if (button == Qt::RightButton) {
        flags_down = MOUSEEVENTF_RIGHTDOWN;
        flags_up = MOUSEEVENTF_RIGHTUP;
    } else {
        return;
    }
    
    mouse_event(flags_down, 0, 0, 0, 0);
    usleep(10000); // 10ms delay
    mouse_event(flags_up, 0, 0, 0, 0);
    
#elif defined(Q_OS_LINUX)
    if (m_display) {
        unsigned int xbutton;
        if (button == Qt::LeftButton) {
            xbutton = Button1;
        } else if (button == Qt::RightButton) {
            xbutton = Button3;
        } else {
            return;
        }
        
        XTestFakeButtonEvent(m_display, xbutton, True, CurrentTime);
        XFlush(m_display);
        usleep(10000); // 10ms delay
        XTestFakeButtonEvent(m_display, xbutton, False, CurrentTime);
        XFlush(m_display);
    }
#endif
    
    emit mouseClicked(button, clickPos);
}

bool MouseController::isLeftButtonPressed() const
{
#ifdef Q_OS_WIN
    return GetAsyncKeyState(VK_LBUTTON) & 0x8000;
#elif defined(Q_OS_LINUX)
    if (m_display) {
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        XQueryPointer(m_display, DefaultRootWindow(m_display),
                     &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
        return mask & Button1Mask;
    }
#endif
    return false;
}

bool MouseController::isRightButtonPressed() const
{
#ifdef Q_OS_WIN
    return GetAsyncKeyState(VK_RBUTTON) & 0x8000;
#elif defined(Q_OS_LINUX)
    if (m_display) {
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        XQueryPointer(m_display, DefaultRootWindow(m_display),
                     &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
        return mask & Button3Mask;
    }
#endif
    return false;
}

void MouseController::updateMousePosition()
{
    QPoint currentPos = getCurrentMousePosition();
    if (currentPos != m_lastPosition) {
        m_lastPosition = currentPos;
        emit mousePositionChanged(currentPos);
    }
}

#include "mousecontroller.moc"