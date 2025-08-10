#pragma once

#include <vector>
#include <string>

struct RecoilPoint {
    float x, y;
    RecoilPoint(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct WeaponProfile {
    std::string name;
    std::vector<RecoilPoint> pattern;
    float verticalControl;
    float horizontalSway;
    int shotInterval;
    
    WeaponProfile(const std::string& name = "Default", 
                  float vertical = 1.0f, 
                  float horizontal = 0.5f, 
                  int interval = 100)
        : name(name), verticalControl(vertical), horizontalSway(horizontal), shotInterval(interval) {}
};

class WeaponProfiles {
public:
    WeaponProfiles();
    
    const std::vector<WeaponProfile>& getProfiles() const { return profiles; }
    WeaponProfile& getCurrentProfile() { return profiles[currentProfileIndex]; }
    const WeaponProfile& getCurrentProfile() const { return profiles[currentProfileIndex]; }
    
    void setCurrentProfile(int index);
    int getCurrentProfileIndex() const { return currentProfileIndex; }
    void addCustomProfile(const WeaponProfile& profile);
    
    // Predefined patterns
    static std::vector<RecoilPoint> getAssaultRiflePattern();
    static std::vector<RecoilPoint> getSMGPattern();
    static std::vector<RecoilPoint> getPistolPattern();
    
private:
    std::vector<WeaponProfile> profiles;
    int currentProfileIndex;
    
    void initializeDefaultProfiles();
};