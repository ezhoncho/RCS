#include "console_ui.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>

ConsoleUI::ConsoleUI()
    : m_mouseController(std::make_unique<MouseController>())
    , m_macroRecorder(std::make_unique<MacroRecorder>(m_mouseController.get()))
    , m_running(true)
    , m_lastMousePosition(0, 0)
{
    // Set up callbacks
    m_macroRecorder->setRecordingStartedCallback([this]() { onRecordingStarted(); });
    m_macroRecorder->setRecordingStoppedCallback([this]() { onRecordingStopped(); });
    m_macroRecorder->setPlaybackStartedCallback([this]() { onPlaybackStarted(); });
    m_macroRecorder->setPlaybackStoppedCallback([this]() { onPlaybackStopped(); });
    m_macroRecorder->setPlaybackProgressCallback([this](int current, int total) { onPlaybackProgress(current, total); });
    m_macroRecorder->setMacroClearedCallback([this]() { onMacroCleared(); });
    m_macroRecorder->setStateChangedCallback([this](MacroRecorder::State state) { onStateChanged(state); });
    
    m_mouseController->setPositionCallback([this](const Point& pos) { onMousePositionChanged(pos); });
    m_mouseController->setClickCallback([this](MouseButton button, const Point& pos) { onMouseClicked(button, pos); });
}

ConsoleUI::~ConsoleUI()
{
}

void ConsoleUI::run()
{
    std::cout << "=== RCS - Record Control System v1.0 ===" << std::endl;
    std::cout << "A modern Macro Mouse Control program" << std::endl;
    std::cout << "==========================================" << std::endl << std::endl;
    
    while (m_running) {
        showStatus();
        showMenu();
        handleInput();
    }
    
    std::cout << "Thank you for using RCS!" << std::endl;
}

void ConsoleUI::showMenu()
{
    std::cout << std::endl << "=== MAIN MENU ===" << std::endl;
    std::cout << "1. Start Recording" << std::endl;
    std::cout << "2. Stop Recording" << std::endl;
    std::cout << "3. Play Macro" << std::endl;
    std::cout << "4. Stop Playback" << std::endl;
    std::cout << "5. Clear Macro" << std::endl;
    std::cout << "6. Load Macro" << std::endl;
    std::cout << "7. Save Macro" << std::endl;
    std::cout << "8. Settings" << std::endl;
    std::cout << "9. Simulate Mouse Actions" << std::endl;
    std::cout << "h. Help" << std::endl;
    std::cout << "q. Quit" << std::endl;
    std::cout << std::endl << "Enter choice: ";
}

void ConsoleUI::handleInput()
{
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return;
    }
    
    char choice = input[0];
    
    switch (choice) {
    case '1':
        startRecording();
        break;
    case '2':
        stopRecording();
        break;
    case '3':
        playMacro();
        break;
    case '4':
        stopPlayback();
        break;
    case '5':
        clearMacro();
        break;
    case '6':
        loadMacro();
        break;
    case '7':
        saveMacro();
        break;
    case '8':
        showSettings();
        break;
    case '9':
        simulateMouseActions();
        break;
    case 'h':
    case 'H':
        showHelp();
        break;
    case 'q':
    case 'Q':
        m_running = false;
        break;
    default:
        std::cout << "Invalid choice. Press 'h' for help." << std::endl;
        break;
    }
}

void ConsoleUI::showStatus()
{
    std::cout << std::endl << "=== STATUS ===" << std::endl;
    
    std::string stateStr;
    switch (m_macroRecorder->state()) {
    case MacroRecorder::Idle:
        stateStr = "Ready";
        break;
    case MacroRecorder::Recording:
        stateStr = "Recording...";
        break;
    case MacroRecorder::Playing:
        stateStr = "Playing...";
        break;
    }
    
    std::cout << "State: " << stateStr << std::endl;
    std::cout << "Events: " << m_macroRecorder->eventCount() << std::endl;
    std::cout << "Mouse Position: (" << m_lastMousePosition.x << ", " << m_lastMousePosition.y << ")" << std::endl;
    std::cout << "Record Mouse Movement: " << (m_macroRecorder->recordMouseMovement() ? "Yes" : "No") << std::endl;
    std::cout << "Playback Speed: " << std::fixed << std::setprecision(1) << m_macroRecorder->playbackSpeed() << "x" << std::endl;
    
    if (m_macroRecorder->hasMacro()) {
        auto duration = m_macroRecorder->totalDuration();
        std::cout << "Total Duration: " << duration.count() << "ms" << std::endl;
    }
}

void ConsoleUI::showHelp()
{
    std::cout << std::endl << "=== HELP ===" << std::endl;
    std::cout << "RCS (Record Control System) is a macro mouse control program." << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "- Record mouse movements and clicks" << std::endl;
    std::cout << "- Play back recorded macros" << std::endl;
    std::cout << "- Save/load macros to/from files" << std::endl;
    std::cout << "- Adjustable playback speed" << std::endl;
    std::cout << "- Optional mouse movement recording" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "1. Start recording (option 1)" << std::endl;
    std::cout << "2. Perform mouse actions" << std::endl;
    std::cout << "3. Stop recording (option 2)" << std::endl;
    std::cout << "4. Play back the macro (option 3)" << std::endl;
    std::cout << std::endl;
    std::cout << "Notes:" << std::endl;
    std::cout << "- In this console version, actual mouse control may be limited" << std::endl;
    std::cout << "- Use option 9 to simulate mouse actions for testing" << std::endl;
    std::cout << "- The Qt GUI version provides full functionality" << std::endl;
}

void ConsoleUI::startRecording()
{
    if (m_macroRecorder->state() != MacroRecorder::Idle) {
        std::cout << "Cannot start recording: already busy" << std::endl;
        return;
    }
    
    m_macroRecorder->startRecording();
}

void ConsoleUI::stopRecording()
{
    if (m_macroRecorder->state() != MacroRecorder::Recording) {
        std::cout << "Not currently recording" << std::endl;
        return;
    }
    
    m_macroRecorder->stopRecording();
}

void ConsoleUI::playMacro()
{
    if (m_macroRecorder->state() != MacroRecorder::Idle) {
        std::cout << "Cannot play macro: busy" << std::endl;
        return;
    }
    
    if (!m_macroRecorder->hasMacro()) {
        std::cout << "No macro to play. Record or load a macro first." << std::endl;
        return;
    }
    
    m_macroRecorder->playMacro();
}

void ConsoleUI::stopPlayback()
{
    if (m_macroRecorder->state() != MacroRecorder::Playing) {
        std::cout << "Not currently playing" << std::endl;
        return;
    }
    
    m_macroRecorder->stopPlayback();
}

void ConsoleUI::clearMacro()
{
    if (m_macroRecorder->state() != MacroRecorder::Idle) {
        std::cout << "Cannot clear macro: busy" << std::endl;
        return;
    }
    
    m_macroRecorder->clearMacro();
}

void ConsoleUI::loadMacro()
{
    if (m_macroRecorder->state() != MacroRecorder::Idle) {
        std::cout << "Cannot load macro: busy" << std::endl;
        return;
    }
    
    std::cout << "Enter filename to load: ";
    std::string filename;
    std::getline(std::cin, filename);
    
    if (!filename.empty()) {
        if (m_macroRecorder->loadMacro(filename)) {
            std::cout << "Macro loaded successfully!" << std::endl;
        } else {
            std::cout << "Failed to load macro." << std::endl;
        }
    }
}

void ConsoleUI::saveMacro()
{
    if (m_macroRecorder->state() != MacroRecorder::Idle) {
        std::cout << "Cannot save macro: busy" << std::endl;
        return;
    }
    
    if (!m_macroRecorder->hasMacro()) {
        std::cout << "No macro to save. Record a macro first." << std::endl;
        return;
    }
    
    std::cout << "Enter filename to save: ";
    std::string filename;
    std::getline(std::cin, filename);
    
    if (!filename.empty()) {
        if (m_macroRecorder->saveMacro(filename)) {
            std::cout << "Macro saved successfully!" << std::endl;
        } else {
            std::cout << "Failed to save macro." << std::endl;
        }
    }
}

void ConsoleUI::showSettings()
{
    std::cout << std::endl << "=== SETTINGS ===" << std::endl;
    std::cout << "1. Record Mouse Movement: " << (m_macroRecorder->recordMouseMovement() ? "Yes" : "No") << std::endl;
    std::cout << "2. Playback Speed: " << std::fixed << std::setprecision(1) << m_macroRecorder->playbackSpeed() << "x" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter setting number to change (or press Enter to return): ";
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input == "1") {
        bool current = m_macroRecorder->recordMouseMovement();
        m_macroRecorder->setRecordMouseMovement(!current);
        std::cout << "Record Mouse Movement set to: " << (!current ? "Yes" : "No") << std::endl;
    } else if (input == "2") {
        std::cout << "Enter new playback speed (0.1 - 10.0): ";
        std::string speedStr;
        std::getline(std::cin, speedStr);
        try {
            double speed = std::stod(speedStr);
            if (speed >= 0.1 && speed <= 10.0) {
                m_macroRecorder->setPlaybackSpeed(speed);
                std::cout << "Playback speed set to: " << std::fixed << std::setprecision(1) << speed << "x" << std::endl;
            } else {
                std::cout << "Invalid speed. Must be between 0.1 and 10.0" << std::endl;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid input." << std::endl;
        }
    }
}

void ConsoleUI::simulateMouseActions()
{
    std::cout << std::endl << "=== SIMULATE MOUSE ACTIONS ===" << std::endl;
    std::cout << "This simulates mouse actions for testing the macro system." << std::endl;
    std::cout << "1. Left click at (100, 100)" << std::endl;
    std::cout << "2. Right click at (200, 200)" << std::endl;
    std::cout << "3. Double click at (300, 300)" << std::endl;
    std::cout << "4. Move to (400, 400)" << std::endl;
    std::cout << "5. Sequence of actions" << std::endl;
    std::cout << std::endl << "Enter choice (or press Enter to return): ";
    
    std::string input;
    std::getline(std::cin, input);
    
    switch (input.empty() ? '0' : input[0]) {
    case '1':
        m_mouseController->leftClick(Point(100, 100));
        break;
    case '2':
        m_mouseController->rightClick(Point(200, 200));
        break;
    case '3':
        m_mouseController->doubleClick(Point(300, 300));
        break;
    case '4':
        m_mouseController->setMousePosition(Point(400, 400));
        break;
    case '5':
        std::cout << "Executing sequence..." << std::endl;
        m_mouseController->setMousePosition(Point(50, 50));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_mouseController->leftClick(Point(100, 100));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_mouseController->setMousePosition(Point(200, 200));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_mouseController->rightClick(Point(200, 200));
        std::cout << "Sequence completed." << std::endl;
        break;
    default:
        break;
    }
}

void ConsoleUI::onRecordingStarted()
{
    std::cout << "[EVENT] Recording started" << std::endl;
}

void ConsoleUI::onRecordingStopped()
{
    std::cout << "[EVENT] Recording stopped" << std::endl;
}

void ConsoleUI::onPlaybackStarted()
{
    std::cout << "[EVENT] Playback started" << std::endl;
}

void ConsoleUI::onPlaybackStopped()
{
    std::cout << "[EVENT] Playback stopped" << std::endl;
}

void ConsoleUI::onPlaybackProgress(int current, int total)
{
    if (total > 0) {
        int percentage = (current * 100) / total;
        std::cout << "[PROGRESS] " << current << "/" << total << " (" << percentage << "%)" << std::endl;
    }
}

void ConsoleUI::onMacroCleared()
{
    std::cout << "[EVENT] Macro cleared" << std::endl;
}

void ConsoleUI::onStateChanged(MacroRecorder::State state)
{
    // State changes are already reported by other callbacks
}

void ConsoleUI::onMousePositionChanged(const Point& position)
{
    m_lastMousePosition = position;
    // Don't print every position change as it would be too verbose
}

void ConsoleUI::onMouseClicked(MouseButton button, const Point& position)
{
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
    
    std::cout << "[CLICK] " << buttonName << " button at (" << position.x << ", " << position.y << ")" << std::endl;
}