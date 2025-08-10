#pragma once

#include "recoil_control.h"
#include <imgui.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
// For Linux demonstration
typedef void* HWND;
#define VK_F9 120
#define GetAsyncKeyState(key) false  // Stub for Linux
#endif

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    bool initialize(HWND hwnd);
    void shutdown();
    
    void render(RecoilControl& recoilControl);
    void handleInput();
    
    bool isVisible() const { return showUI; }
    void toggle() { showUI = !showUI; }
    
    // UI state
    bool shouldClose() const { return requestClose; }
    
private:
    bool showUI;
    bool requestClose;
    int uiToggleKey; // F9
    bool lastUIToggleState;
    
    // UI rendering methods
    void renderMainPanel(RecoilControl& recoilControl);
    void renderWeaponProfiles(WeaponProfiles& profiles);
    void renderControlSettings(RecoilControl& recoilControl);
    void renderFlashDetectionSettings(MuzzleFlashDetector& detector);
    void renderStatusPanel(RecoilControl& recoilControl);
    void renderPatternBuilder(WeaponProfiles& profiles);
    
    // Theme setup
    void setupTheme();
    
    // Helper methods
    const char* getStatusText(bool active);
    ImVec4 getStatusColor(bool active);
};