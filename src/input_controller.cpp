#include "input_controller.h"
#include <thread>
#include <iostream>
#include <cstring>

InputController::InputController() 
    : randomGenerator(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
      randomDistribution(-15, 15) {
    // Initialize key states
    memset(lastKeyStates, 0, sizeof(lastKeyStates));
}

void InputController::moveMouse(int deltaX, int deltaY) {
    // Add randomization for anti-detection
    addRandomization(deltaX, deltaY);
    
#ifdef PLATFORM_WINDOWS
    // Use SendInput for more reliable input simulation on Windows
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = deltaX;
    input.mi.dy = deltaY;
    
    SendInput(1, &input, sizeof(INPUT));
#else
    // For Linux demonstration - just log the movement
    std::cout << "Mouse move: dx=" << deltaX << ", dy=" << deltaY << std::endl;
    // In a real Linux implementation, you'd use X11 or other methods
#endif
}

void InputController::moveMouseSmooth(int deltaX, int deltaY, int steps) {
    if (steps <= 1) {
        moveMouse(deltaX, deltaY);
        return;
    }
    
    float stepX = static_cast<float>(deltaX) / steps;
    float stepY = static_cast<float>(deltaY) / steps;
    
    for (int i = 0; i < steps; ++i) {
        int currentDeltaX = static_cast<int>(stepX * (i + 1)) - static_cast<int>(stepX * i);
        int currentDeltaY = static_cast<int>(stepY * (i + 1)) - static_cast<int>(stepY * i);
        
        moveMouse(currentDeltaX, currentDeltaY);
        
        // Small delay between steps for smoothness
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

bool InputController::isKeyPressed(int virtualKey) {
#ifdef PLATFORM_WINDOWS
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
#else
    // For Linux demonstration - simulate key state
    // In real implementation, you'd check X11 key state or use other methods
    static bool simulatedF8 = false;
    static auto lastToggle = std::chrono::steady_clock::now();
    
    if (virtualKey == VK_F8) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastToggle).count();
        
        // Simulate F8 toggle every 10 seconds for demo
        if (elapsed >= 10) {
            simulatedF8 = !simulatedF8;
            lastToggle = now;
            std::cout << "Simulated F8 key toggle: " << (simulatedF8 ? "ON" : "OFF") << std::endl;
        }
        return simulatedF8;
    }
    
    return false;
#endif
}

bool InputController::isKeyToggled(int virtualKey) {
    updateKeyStates();
    
    bool currentState = isKeyPressed(virtualKey);
    bool wasPressed = !lastKeyStates[virtualKey] && currentState;
    
    lastKeyStates[virtualKey] = currentState;
    return wasPressed;
}

void InputController::addRandomization(int& deltaX, int& deltaY) {
    // Add small random variations to avoid detection
    int randX = randomDistribution(randomGenerator) / 10; // ±1.5 pixels
    int randY = randomDistribution(randomGenerator) / 10;
    
    deltaX += randX;
    deltaY += randY;
}

void InputController::waitRandomDelay(int baseMs, int variationMs) {
    int variation = randomDistribution(randomGenerator) % (variationMs * 2 + 1) - variationMs;
    int totalDelay = baseMs + variation;
    
    if (totalDelay > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(totalDelay));
    }
}

void InputController::updateKeyStates() {
    // This method is called to update the key state tracking
    // The actual state checking is done in isKeyToggled
}