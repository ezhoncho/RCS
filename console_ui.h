#pragma once

#include "mousecontroller_console.h"
#include "macrorecorder_console.h"
#include <memory>

class ConsoleUI
{
public:
    ConsoleUI();
    ~ConsoleUI();
    
    void run();

private:
    void showMenu();
    void handleInput();
    void showStatus();
    void showHelp();
    
    // Menu handlers
    void startRecording();
    void stopRecording();
    void playMacro();
    void stopPlayback();
    void clearMacro();
    void loadMacro();
    void saveMacro();
    void showSettings();
    void changeSettings();
    void simulateMouseActions();
    
    // Callbacks
    void onRecordingStarted();
    void onRecordingStopped();
    void onPlaybackStarted();
    void onPlaybackStopped();
    void onPlaybackProgress(int current, int total);
    void onMacroCleared();
    void onStateChanged(MacroRecorder::State state);
    void onMousePositionChanged(const Point& position);
    void onMouseClicked(MouseButton button, const Point& position);
    
    std::unique_ptr<MouseController> m_mouseController;
    std::unique_ptr<MacroRecorder> m_macroRecorder;
    bool m_running;
    Point m_lastMousePosition;
};