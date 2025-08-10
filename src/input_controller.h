#pragma once

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
// For Linux demonstration - stub implementations
#define VK_F8 119  // F8 key code
#define VK_F9 120  // F9 key code
#endif

#include <chrono>
#include <random>

class InputController {
public:
    InputController();
    
    // Mouse control methods
    void moveMouse(int deltaX, int deltaY);
    void moveMouseSmooth(int deltaX, int deltaY, int steps = 5);
    
    // Key state checking
    bool isKeyPressed(int virtualKey);
    bool isKeyToggled(int virtualKey);
    
    // Anti-detection features
    void addRandomization(int& deltaX, int& deltaY);
    void waitRandomDelay(int baseMs, int variationMs = 15);
    
private:
    std::mt19937 randomGenerator;
    std::uniform_int_distribution<int> randomDistribution;
    
    // For key state tracking
    bool lastKeyStates[256];
    
    void updateKeyStates();
};