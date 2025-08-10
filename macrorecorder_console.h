#pragma once

#include "mousecontroller_console.h"
#include <vector>
#include <chrono>
#include <functional>
#include <string>

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
    Point position;
    std::chrono::milliseconds timestamp;
    int delayMs;
    
    MacroEvent() : type(MouseMove), position(0, 0), timestamp(0), delayMs(0) {}
    MacroEvent(Type t, const Point &pos, std::chrono::milliseconds time) 
        : type(t), position(pos), timestamp(time), delayMs(0) {}
    MacroEvent(int delay) 
        : type(Delay), position(0, 0), timestamp(std::chrono::milliseconds(0)), delayMs(delay) {}
};

class MacroRecorder
{
public:
    enum State {
        Idle,
        Recording,
        Playing
    };

    explicit MacroRecorder(MouseController *mouseController);
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
    bool loadMacro(const std::string &filename);
    bool saveMacro(const std::string &filename);
    
    // State queries
    State state() const { return m_state; }
    bool isRecording() const { return m_state == Recording; }
    bool isPlaying() const { return m_state == Playing; }
    bool hasMacro() const { return !m_events.empty(); }
    
    // Settings
    void setRecordMouseMovement(bool record) { m_recordMouseMovement = record; }
    bool recordMouseMovement() const { return m_recordMouseMovement; }
    
    void setPlaybackSpeed(double speed) { m_playbackSpeed = speed; }
    double playbackSpeed() const { return m_playbackSpeed; }
    
    // Statistics
    int eventCount() const { return static_cast<int>(m_events.size()); }
    std::chrono::milliseconds totalDuration() const;
    
    // Callbacks
    void setRecordingStartedCallback(std::function<void()> callback) { m_recordingStartedCallback = callback; }
    void setRecordingStoppedCallback(std::function<void()> callback) { m_recordingStoppedCallback = callback; }
    void setPlaybackStartedCallback(std::function<void()> callback) { m_playbackStartedCallback = callback; }
    void setPlaybackStoppedCallback(std::function<void()> callback) { m_playbackStoppedCallback = callback; }
    void setPlaybackProgressCallback(std::function<void(int, int)> callback) { m_playbackProgressCallback = callback; }
    void setMacroClearedCallback(std::function<void()> callback) { m_macroClearedCallback = callback; }
    void setStateChangedCallback(std::function<void(State)> callback) { m_stateChangedCallback = callback; }

private:
    void onMousePositionChanged(const Point &position);
    void onMouseClicked(MouseButton button, const Point &position);
    void executeNextEvent();
    void setState(State newState);
    void recordEvent(const MacroEvent &event);
    void scheduleNextEvent();
    std::chrono::milliseconds getCurrentTimestamp() const;
    
    MouseController *m_mouseController;
    std::vector<MacroEvent> m_events;
    State m_state;
    
    // Recording settings
    bool m_recordMouseMovement;
    std::chrono::milliseconds m_recordingStartTime;
    Point m_lastRecordedPosition;
    
    // Playback settings
    double m_playbackSpeed;
    int m_currentEventIndex;
    std::chrono::milliseconds m_playbackStartTime;
    std::chrono::milliseconds m_playbackPauseOffset;
    
    // Callbacks
    std::function<void()> m_recordingStartedCallback;
    std::function<void()> m_recordingStoppedCallback;
    std::function<void()> m_playbackStartedCallback;
    std::function<void()> m_playbackStoppedCallback;
    std::function<void(int, int)> m_playbackProgressCallback;
    std::function<void()> m_macroClearedCallback;
    std::function<void(State)> m_stateChangedCallback;
};