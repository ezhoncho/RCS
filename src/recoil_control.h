#pragma once

#include "weapon_profiles.h"
#include "input_controller.h"
#include "muzzle_flash_detector.h"
#include <chrono>
#include <atomic>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
// For Linux demonstration - key constants
#define VK_F8 119
#define VK_F9 120
#endif

class RecoilControl {
public:
    RecoilControl();
    ~RecoilControl();
    
    // Main control methods
    void start();
    void stop();
    bool isActive() const { return active; }
    void toggle() { active ? stop() : start(); }
    
    // Settings
    void setVerticalSensitivity(float sensitivity) { verticalSensitivity = sensitivity; }
    void setHorizontalSensitivity(float sensitivity) { horizontalSensitivity = sensitivity; }
    void setShotDelay(int delayMs) { shotDelayMs = delayMs; }
    
    float getVerticalSensitivity() const { return verticalSensitivity; }
    float getHorizontalSensitivity() const { return horizontalSensitivity; }
    int getShotDelay() const { return shotDelayMs; }
    
    // Weapon profile management
    WeaponProfiles& getWeaponProfiles() { return weaponProfiles; }
    MuzzleFlashDetector& getFlashDetector() { return flashDetector; }
    
    // Main update loop (should be called frequently)
    void update();
    
private:
    std::atomic<bool> active;
    
    // Core components
    WeaponProfiles weaponProfiles;
    InputController inputController;
    MuzzleFlashDetector flashDetector;
    
    // Settings
    float verticalSensitivity;
    float horizontalSensitivity;
    int shotDelayMs;
    
    // Pattern state
    int currentPatternIndex;
    std::chrono::steady_clock::time_point lastShotTime;
    bool flashDetected;
    
    // Control keys
    int toggleKey; // F8
    bool lastToggleState;
    
    void processRecoilCompensation();
    void resetPattern();
    void applyRecoil(const RecoilPoint& point);
};