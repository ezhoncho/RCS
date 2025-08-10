#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif

class MouseController : public QObject
{
    Q_OBJECT

public:
    explicit MouseController(QObject *parent = nullptr);
    ~MouseController();

    // Mouse position functions
    QPoint getCurrentMousePosition() const;
    void setMousePosition(const QPoint &position);
    
    // Mouse click functions
    void leftClick(const QPoint &position = QPoint());
    void rightClick(const QPoint &position = QPoint());
    void doubleClick(const QPoint &position = QPoint());
    
    // Mouse state
    bool isLeftButtonPressed() const;
    bool isRightButtonPressed() const;

signals:
    void mousePositionChanged(const QPoint &position);
    void mouseClicked(Qt::MouseButton button, const QPoint &position);

private slots:
    void updateMousePosition();

private:
    void initializePlatformSpecific();
    void cleanupPlatformSpecific();
    void performClick(Qt::MouseButton button, const QPoint &position);
    
    QTimer *m_positionTimer;
    QPoint m_lastPosition;
    
#ifdef Q_OS_WIN
    // Windows-specific members
#elif defined(Q_OS_LINUX)
    Display *m_display;
#endif
};