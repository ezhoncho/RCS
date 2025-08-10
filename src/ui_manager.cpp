#include "ui_manager.h"
#include <chrono>

#ifdef PLATFORM_WINDOWS
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#else
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

UIManager::UIManager() 
    : showUI(true), requestClose(false), uiToggleKey(VK_F9), lastUIToggleState(false) {
}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize(HWND hwnd) {
    (void)hwnd; // Suppress unused parameter warning
    
    // Setup theme
    setupTheme();
    
    // Platform-specific initialization is handled in main.cpp
    // This is just for any additional UI-specific setup
    
    return true;
}

void UIManager::shutdown() {
    // Platform-specific cleanup is handled in main.cpp
}

void UIManager::handleInput() {
#ifdef PLATFORM_WINDOWS
    // Check UI toggle key (F9)
    bool currentState = GetAsyncKeyState(uiToggleKey) & 0x8000;
    if (!lastUIToggleState && currentState) {
        toggle();
    }
    lastUIToggleState = currentState;
#else
    // For Linux demonstration - auto-toggle UI every 30 seconds
    static auto lastToggle = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastToggle).count();
    
    if (elapsed >= 30) {
        // Don't auto-toggle for demo, keep UI visible
        lastToggle = now;
    }
#endif
}

void UIManager::render(RecoilControl& recoilControl) {
    handleInput();
    
    if (!showUI) {
        return;
    }
    
    // Render main window - ImGui frame setup is handled in main.cpp
    renderMainPanel(recoilControl);
}

void UIManager::renderMainPanel(RecoilControl& recoilControl) {
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("RCS - Recoil Control System", &showUI)) {
        // Status panel
        renderStatusPanel(recoilControl);
        
        ImGui::Separator();
        
        // Control settings
        if (ImGui::CollapsingHeader("Control Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            renderControlSettings(recoilControl);
        }
        
        ImGui::Separator();
        
        // Weapon profiles
        if (ImGui::CollapsingHeader("Weapon Profiles", ImGuiTreeNodeFlags_DefaultOpen)) {
            renderWeaponProfiles(recoilControl.getWeaponProfiles());
        }
        
        ImGui::Separator();
        
        // Flash detection settings
        if (ImGui::CollapsingHeader("Muzzle Flash Detection")) {
            renderFlashDetectionSettings(recoilControl.getFlashDetector());
        }
        
        ImGui::Separator();
        
        // Pattern builder
        if (ImGui::CollapsingHeader("Custom Pattern Builder")) {
            renderPatternBuilder(recoilControl.getWeaponProfiles());
        }
        
        ImGui::Separator();
        
        // Exit button
        if (ImGui::Button("Exit Application", ImVec2(-1, 30))) {
            requestClose = true;
        }
    }
    ImGui::End();
}

void UIManager::renderStatusPanel(RecoilControl& recoilControl) {
    ImGui::Text("System Status");
    ImGui::SameLine();
    
    bool active = recoilControl.isActive();
    ImGui::TextColored(getStatusColor(active), "%s", getStatusText(active));
    
#ifdef PLATFORM_WINDOWS
    ImGui::SameLine();
    ImGui::Text("| Press F8 to toggle | Press F9 to hide UI");
#else
    ImGui::SameLine();
    ImGui::Text("| Linux Demo Mode | Auto-simulation active");
#endif
    
    if (ImGui::Button(active ? "Stop (F8)" : "Start (F8)", ImVec2(100, 25))) {
        recoilControl.toggle();
    }
    
    ImGui::SameLine();
    const char* weaponName = recoilControl.getWeaponProfiles().getCurrentProfile().name.c_str();
    ImGui::Text("Active Weapon: %s", weaponName);
}

void UIManager::renderControlSettings(RecoilControl& recoilControl) {
    float vertSens = recoilControl.getVerticalSensitivity();
    float horzSens = recoilControl.getHorizontalSensitivity();
    int shotDelay = recoilControl.getShotDelay();
    
    if (ImGui::SliderFloat("Vertical Sensitivity", &vertSens, 0.0f, 2.0f, "%.2f")) {
        recoilControl.setVerticalSensitivity(vertSens);
    }
    
    if (ImGui::SliderFloat("Horizontal Sensitivity", &horzSens, 0.0f, 2.0f, "%.2f")) {
        recoilControl.setHorizontalSensitivity(horzSens);
    }
    
    if (ImGui::SliderInt("Shot Delay (ms)", &shotDelay, 50, 200)) {
        recoilControl.setShotDelay(shotDelay);
    }
}

void UIManager::renderWeaponProfiles(WeaponProfiles& profiles) {
    const auto& profileList = profiles.getProfiles();
    int currentIndex = profiles.getCurrentProfileIndex();
    
    ImGui::Text("Select Weapon Profile:");
    
    for (int i = 0; i < static_cast<int>(profileList.size()); ++i) {
        bool selected = (i == currentIndex);
        if (ImGui::Selectable(profileList[i].name.c_str(), selected)) {
            profiles.setCurrentProfile(i);
        }
    }
    
    ImGui::Separator();
    
    // Current profile settings
    WeaponProfile& current = profiles.getCurrentProfile();
    ImGui::Text("Profile: %s", current.name.c_str());
    
    ImGui::SliderFloat("Vertical Control", &current.verticalControl, 0.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Horizontal Sway", &current.horizontalSway, 0.0f, 1.0f, "%.2f");
    ImGui::SliderInt("Shot Interval", &current.shotInterval, 50, 200);
    
    ImGui::Text("Pattern Length: %zu points", current.pattern.size());
}

void UIManager::renderFlashDetectionSettings(MuzzleFlashDetector& detector) {
    (void)detector; // Suppress unused parameter warning
    
    ImGui::Text("Scan Region: Center 10x10 pixels");
    ImGui::Text("Flash Color: RGB(240, 240, 240)");
    ImGui::Text("Auto-reset: 3 seconds after last shot");
    
#ifdef PLATFORM_WINDOWS
    if (ImGui::Button("Test Flash Detection")) {
        // This would trigger a test detection
        ImGui::Text("Testing... (Look for muzzle flash)");
    }
#else
    ImGui::Text("Linux Demo: Simulated flash detection every 5 seconds");
#endif
}

void UIManager::renderPatternBuilder(WeaponProfiles& profiles) {
    static std::vector<RecoilPoint> customPattern;
    static char patternName[64] = "Custom Pattern";
    
    ImGui::InputText("Pattern Name", patternName, sizeof(patternName));
    
    if (ImGui::Button("Add Point")) {
        customPattern.push_back(RecoilPoint(0.0f, 0.0f));
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Clear Pattern")) {
        customPattern.clear();
    }
    
    // Edit existing points
    for (int i = 0; i < static_cast<int>(customPattern.size()); ++i) {
        ImGui::PushID(i);
        
        float x = customPattern[i].x;
        float y = customPattern[i].y;
        
        ImGui::Text("Point %d:", i + 1);
        ImGui::SameLine();
        
        if (ImGui::DragFloat("X", &x, 0.1f, -10.0f, 10.0f, "%.1f")) {
            customPattern[i].x = x;
        }
        
        ImGui::SameLine();
        if (ImGui::DragFloat("Y", &y, 0.1f, 0.0f, 30.0f, "%.1f")) {
            customPattern[i].y = y;
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            customPattern.erase(customPattern.begin() + i);
            --i;
        }
        
        ImGui::PopID();
    }
    
    if (!customPattern.empty() && ImGui::Button("Save Custom Profile")) {
        WeaponProfile customProfile(patternName);
        customProfile.pattern = customPattern;
        profiles.addCustomProfile(customProfile);
        customPattern.clear();
    }
}

void UIManager::setupTheme() {
    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // Dark gray background theme with white/purple accents
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    
    // White text
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    
    // Purple accents
    colors[ImGuiCol_Header] = ImVec4(0.45f, 0.25f, 0.70f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.35f, 0.80f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.45f, 0.90f, 1.00f);
    
    colors[ImGuiCol_Button] = ImVec4(0.35f, 0.15f, 0.60f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.25f, 0.70f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.35f, 0.80f, 1.00f);
    
    colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.25f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.45f, 0.90f, 1.00f);
    
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    
    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    
    // Window rounding
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 2.0f;
}

const char* UIManager::getStatusText(bool active) {
    return active ? "ACTIVE" : "INACTIVE";
}

ImVec4 UIManager::getStatusColor(bool active) {
    return active ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
}