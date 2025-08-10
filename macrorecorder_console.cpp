#include "macrorecorder_console.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <cmath>

MacroRecorder::MacroRecorder(MouseController *mouseController)
    : m_mouseController(mouseController)
    , m_state(Idle)
    , m_recordMouseMovement(true)
    , m_recordingStartTime(std::chrono::milliseconds(0))
    , m_lastRecordedPosition(0, 0)
    , m_playbackSpeed(1.0)
    , m_currentEventIndex(0)
    , m_playbackStartTime(std::chrono::milliseconds(0))
    , m_playbackPauseOffset(std::chrono::milliseconds(0))
{
    // Set up mouse controller callbacks
    m_mouseController->setPositionCallback([this](const Point& pos) {
        onMousePositionChanged(pos);
    });
    
    m_mouseController->setClickCallback([this](MouseButton button, const Point& pos) {
        onMouseClicked(button, pos);
    });
}

MacroRecorder::~MacroRecorder()
{
    if (m_state != Idle) {
        stopRecording();
        stopPlayback();
    }
}

void MacroRecorder::startRecording()
{
    if (m_state != Idle) {
        return;
    }
    
    clearMacro();
    m_recordingStartTime = getCurrentTimestamp();
    m_lastRecordedPosition = m_mouseController->getCurrentMousePosition();
    setState(Recording);
    
    m_mouseController->startMonitoring();
    
    std::cout << "Recording started..." << std::endl;
    if (m_recordingStartedCallback) {
        m_recordingStartedCallback();
    }
}

void MacroRecorder::stopRecording()
{
    if (m_state != Recording) {
        return;
    }
    
    m_mouseController->stopMonitoring();
    setState(Idle);
    
    std::cout << "Recording stopped. Captured " << m_events.size() << " events." << std::endl;
    if (m_recordingStoppedCallback) {
        m_recordingStoppedCallback();
    }
}

void MacroRecorder::pauseRecording()
{
    if (m_state == Recording) {
        stopRecording();
    }
}

void MacroRecorder::resumeRecording()
{
    startRecording();
}

void MacroRecorder::playMacro()
{
    if (m_state != Idle || m_events.empty()) {
        std::cout << "Cannot play macro: " << (m_state != Idle ? "busy" : "no macro recorded") << std::endl;
        return;
    }
    
    m_currentEventIndex = 0;
    m_playbackStartTime = getCurrentTimestamp();
    m_playbackPauseOffset = std::chrono::milliseconds(0);
    setState(Playing);
    
    std::cout << "Playback started..." << std::endl;
    if (m_playbackStartedCallback) {
        m_playbackStartedCallback();
    }
    
    // Start playback in a separate thread
    std::thread([this]() {
        while (m_state == Playing && m_currentEventIndex < static_cast<int>(m_events.size())) {
            executeNextEvent();
            
            if (m_currentEventIndex < static_cast<int>(m_events.size())) {
                scheduleNextEvent();
            }
        }
        
        if (m_state == Playing) {
            stopPlayback();
        }
    }).detach();
}

void MacroRecorder::stopPlayback()
{
    if (m_state != Playing) {
        return;
    }
    
    setState(Idle);
    
    std::cout << "Playback stopped." << std::endl;
    if (m_playbackStoppedCallback) {
        m_playbackStoppedCallback();
    }
}

void MacroRecorder::pausePlayback()
{
    if (m_state == Playing) {
        m_playbackPauseOffset = getCurrentTimestamp() - m_playbackStartTime;
    }
}

void MacroRecorder::resumePlayback()
{
    if (m_state == Playing) {
        m_playbackStartTime = getCurrentTimestamp() - m_playbackPauseOffset;
    }
}

void MacroRecorder::clearMacro()
{
    m_events.clear();
    std::cout << "Macro cleared." << std::endl;
    if (m_macroClearedCallback) {
        m_macroClearedCallback();
    }
}

bool MacroRecorder::loadMacro(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return false;
    }
    
    clearMacro();
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments and empty lines
        
        // Simple format: type,x,y,timestamp,delay
        // Example: 0,100,200,1000,0 (MouseMove at 100,200 at 1000ms)
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 5) {
            MacroEvent event;
            event.type = static_cast<MacroEvent::Type>(std::stoi(tokens[0]));
            event.position = Point(std::stoi(tokens[1]), std::stoi(tokens[2]));
            event.timestamp = std::chrono::milliseconds(std::stoll(tokens[3]));
            event.delayMs = std::stoi(tokens[4]);
            m_events.push_back(event);
        }
    }
    
    std::cout << "Loaded macro with " << m_events.size() << " events from " << filename << std::endl;
    return true;
}

bool MacroRecorder::saveMacro(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "# RCS Macro File v1.0" << std::endl;
    file << "# Format: type,x,y,timestamp,delay" << std::endl;
    file << "# Types: 0=MouseMove, 1=LeftClick, 2=RightClick, 3=DoubleClick, 4=Delay" << std::endl;
    
    for (const MacroEvent &event : m_events) {
        file << static_cast<int>(event.type) << ","
             << event.position.x << ","
             << event.position.y << ","
             << event.timestamp.count() << ","
             << event.delayMs << std::endl;
    }
    
    std::cout << "Saved macro with " << m_events.size() << " events to " << filename << std::endl;
    return true;
}

std::chrono::milliseconds MacroRecorder::totalDuration() const
{
    if (m_events.empty()) {
        return std::chrono::milliseconds(0);
    }
    
    return m_events.back().timestamp - m_events.front().timestamp;
}

void MacroRecorder::onMousePositionChanged(const Point &position)
{
    if (m_state != Recording || !m_recordMouseMovement) {
        return;
    }
    
    // Only record significant movements to avoid cluttering
    Point diff;
    diff.x = position.x - m_lastRecordedPosition.x;
    diff.y = position.y - m_lastRecordedPosition.y;
    
    if (std::abs(diff.x) > 5 || std::abs(diff.y) > 5) {
        MacroEvent event(MacroEvent::MouseMove, position, getCurrentTimestamp());
        recordEvent(event);
        m_lastRecordedPosition = position;
    }
}

void MacroRecorder::onMouseClicked(MouseButton button, const Point &position)
{
    if (m_state != Recording) {
        return;
    }
    
    MacroEvent::Type eventType;
    switch (button) {
    case MouseButton::Left:
        eventType = MacroEvent::LeftClick;
        break;
    case MouseButton::Right:
        eventType = MacroEvent::RightClick;
        break;
    default:
        return; // Ignore other buttons
    }
    
    MacroEvent event(eventType, position, getCurrentTimestamp());
    recordEvent(event);
}

void MacroRecorder::executeNextEvent()
{
    if (m_state != Playing || m_currentEventIndex >= static_cast<int>(m_events.size())) {
        return;
    }
    
    const MacroEvent &event = m_events[m_currentEventIndex];
    
    switch (event.type) {
    case MacroEvent::MouseMove:
        m_mouseController->setMousePosition(event.position);
        break;
    case MacroEvent::LeftClick:
        m_mouseController->leftClick(event.position);
        break;
    case MacroEvent::RightClick:
        m_mouseController->rightClick(event.position);
        break;
    case MacroEvent::DoubleClick:
        m_mouseController->doubleClick(event.position);
        break;
    case MacroEvent::Delay:
        std::this_thread::sleep_for(std::chrono::milliseconds(event.delayMs));
        break;
    }
    
    if (m_playbackProgressCallback) {
        m_playbackProgressCallback(m_currentEventIndex, static_cast<int>(m_events.size()));
    }
    
    m_currentEventIndex++;
}

void MacroRecorder::setState(State newState)
{
    if (m_state != newState) {
        m_state = newState;
        if (m_stateChangedCallback) {
            m_stateChangedCallback(newState);
        }
    }
}

void MacroRecorder::recordEvent(const MacroEvent &event)
{
    m_events.push_back(event);
}

void MacroRecorder::scheduleNextEvent()
{
    if (m_currentEventIndex >= static_cast<int>(m_events.size())) {
        return;
    }
    
    int delay = 50; // Default delay between events
    
    if (m_currentEventIndex > 0) {
        auto timeDiff = m_events[m_currentEventIndex].timestamp - 
                       m_events[m_currentEventIndex - 1].timestamp;
        delay = static_cast<int>(timeDiff.count() / m_playbackSpeed);
        delay = std::max(10, std::min(5000, delay)); // Clamp between 10ms and 5s
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

std::chrono::milliseconds MacroRecorder::getCurrentTimestamp() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
}