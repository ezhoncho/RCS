#pragma once

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QDateTime>
#include <QVector>
#include <QDataStream>
#include <QFile>

class MouseController;

struct MacroEvent
{
    enum Type {
        MouseMove,
        LeftClick,
        RightClick,
        DoubleClick,
        Delay
    };
    
    Type type;
    QPoint position;
    qint64 timestamp;
    int delayMs;
    
    MacroEvent() : type(MouseMove), position(0, 0), timestamp(0), delayMs(0) {}
    MacroEvent(Type t, const QPoint &pos, qint64 time) 
        : type(t), position(pos), timestamp(time), delayMs(0) {}
    MacroEvent(int delay) 
        : type(Delay), position(0, 0), timestamp(0), delayMs(delay) {}
};

Q_DECLARE_METATYPE(MacroEvent)

class MacroRecorder : public QObject
{
    Q_OBJECT

public:
    enum State {
        Idle,
        Recording,
        Playing
    };

    explicit MacroRecorder(MouseController *mouseController, QObject *parent = nullptr);
    ~MacroRecorder();

    // Recording control
    void startRecording();
    void stopRecording();
    void pauseRecording();
    void resumeRecording();
    
    // Playback control
    void playMacro();
    void stopPlayback();
    void pausePlayback();
    void resumePlayback();
    
    // Macro management
    void clearMacro();
    bool loadMacro(const QString &filename);
    bool saveMacro(const QString &filename);
    
    // State queries
    State state() const { return m_state; }
    bool isRecording() const { return m_state == Recording; }
    bool isPlaying() const { return m_state == Playing; }
    bool hasMacro() const { return !m_events.isEmpty(); }
    
    // Settings
    void setRecordMouseMovement(bool record) { m_recordMouseMovement = record; }
    bool recordMouseMovement() const { return m_recordMouseMovement; }
    
    void setPlaybackSpeed(double speed) { m_playbackSpeed = speed; }
    double playbackSpeed() const { return m_playbackSpeed; }
    
    // Statistics
    int eventCount() const { return m_events.size(); }
    qint64 totalDuration() const;

signals:
    void recordingStarted();
    void recordingStopped();
    void playbackStarted();
    void playbackStopped();
    void playbackProgress(int currentEvent, int totalEvents);
    void macroCleared();
    void stateChanged(State newState);

private slots:
    void onMousePositionChanged(const QPoint &position);
    void onMouseClicked(Qt::MouseButton button, const QPoint &position);
    void executeNextEvent();

private:
    void setState(State newState);
    void recordEvent(const MacroEvent &event);
    void scheduleNextEvent();
    qint64 getCurrentTimestamp() const;
    
    MouseController *m_mouseController;
    QVector<MacroEvent> m_events;
    State m_state;
    
    // Recording settings
    bool m_recordMouseMovement;
    qint64 m_recordingStartTime;
    QPoint m_lastRecordedPosition;
    
    // Playback settings
    double m_playbackSpeed;
    QTimer *m_playbackTimer;
    int m_currentEventIndex;
    qint64 m_playbackStartTime;
    qint64 m_playbackPauseOffset;
};