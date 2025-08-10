#include "recoil_control.h"

RecoilControl::RecoilControl() 
    : active(false), verticalSensitivity(1.0f), horizontalSensitivity(1.0f),
      shotDelayMs(100), currentPatternIndex(0), flashDetected(false),
      toggleKey(VK_F8), lastToggleState(false) {
    
    resetPattern();
}

RecoilControl::~RecoilControl() {
    stop();
}

void RecoilControl::start() {
    if (!active.load()) {
        active = true;
        resetPattern();
    }
}

void RecoilControl::stop() {
    if (active.load()) {
        active = false;
        resetPattern();
    }
}

void RecoilControl::update() {
    // Check toggle key (F8)
    bool currentToggleState = inputController.isKeyPressed(toggleKey);
    if (!lastToggleState && currentToggleState) {
        toggle();
    }
    lastToggleState = currentToggleState;
    
    if (!active.load()) {
        return;
    }
    
    // Check for auto-reset based on reload time
    if (flashDetector.shouldResetPattern()) {
        resetPattern();
    }
    
    // Process recoil compensation
    processRecoilCompensation();
}

void RecoilControl::processRecoilCompensation() {
    // Detect muzzle flash
    if (flashDetector.detectFlash()) {
        flashDetected = true;
        
        // Apply recoil compensation
        const WeaponProfile& profile = weaponProfiles.getCurrentProfile();
        
        if (currentPatternIndex < static_cast<int>(profile.pattern.size())) {
            // Calculate time since last shot
            auto now = std::chrono::steady_clock::now();
            auto timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastShotTime).count();
            
            // Check if enough time has passed for next shot
            if (timeSinceLastShot >= shotDelayMs) {
                applyRecoil(profile.pattern[currentPatternIndex]);
                currentPatternIndex++;
                lastShotTime = now;
                
                // Add random delay for anti-detection
                inputController.waitRandomDelay(5, 10);
            }
        }
    }
}

void RecoilControl::applyRecoil(const RecoilPoint& point) {
    const WeaponProfile& profile = weaponProfiles.getCurrentProfile();
    
    // Calculate compensated movement (opposite of recoil)
    float compensatedX = -point.x * horizontalSensitivity * profile.horizontalSway;
    float compensatedY = -point.y * verticalSensitivity * profile.verticalControl;
    
    // Convert to integer pixel movement
    int deltaX = static_cast<int>(compensatedX);
    int deltaY = static_cast<int>(compensatedY);
    
    // Apply smooth mouse movement
    inputController.moveMouseSmooth(deltaX, deltaY, 3);
}

void RecoilControl::resetPattern() {
    currentPatternIndex = 0;
    flashDetected = false;
    lastShotTime = std::chrono::steady_clock::now();
    flashDetector.resetTimer();
}