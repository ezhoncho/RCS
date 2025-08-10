#include "weapon_profiles.h"

WeaponProfiles::WeaponProfiles() : currentProfileIndex(0) {
    initializeDefaultProfiles();
}

void WeaponProfiles::initializeDefaultProfiles() {
    // Assault Rifle Profile
    WeaponProfile assaultRifle("Assault Rifle", 1.2f, 0.8f, 90);
    assaultRifle.pattern = getAssaultRiflePattern();
    profiles.push_back(assaultRifle);
    
    // SMG Profile
    WeaponProfile smg("SMG", 0.8f, 1.0f, 75);
    smg.pattern = getSMGPattern();
    profiles.push_back(smg);
    
    // Pistol Profile
    WeaponProfile pistol("Pistol", 0.6f, 0.4f, 120);
    pistol.pattern = getPistolPattern();
    profiles.push_back(pistol);
}

void WeaponProfiles::setCurrentProfile(int index) {
    if (index >= 0 && index < static_cast<int>(profiles.size())) {
        currentProfileIndex = index;
    }
}

void WeaponProfiles::addCustomProfile(const WeaponProfile& profile) {
    profiles.push_back(profile);
}

std::vector<RecoilPoint> WeaponProfiles::getAssaultRiflePattern() {
    return {
        {0, 15},    // Initial kick
        {2, 17},    // Slight right drift
        {-3, 18},   // Left correction
        {4, 20},    // Right drift
        {-2, 22},   // Left correction
        {1, 20},    // Stabilizing
        {-1, 18},   // Minor left
        {3, 16},    // Right drift
        {-4, 14},   // Strong left correction
        {2, 12},    // Settle right
        {-1, 10},   // Final stabilization
        {0, 8},     // Reduced recoil
        {1, 6},     // Minor adjustments
        {-1, 4},    // Continue pattern
        {0, 2}      // Minimal recoil
    };
}

std::vector<RecoilPoint> WeaponProfiles::getSMGPattern() {
    return {
        {0, 12},    // Initial kick (less than AR)
        {1, 14},    // Quick right
        {-2, 16},   // Left correction
        {3, 18},    // Right drift
        {-3, 20},   // Left correction
        {2, 18},    // Right stabilize
        {-1, 16},   // Minor left
        {1, 14},    // Right balance
        {-2, 12},   // Left correction
        {1, 10},    // Settle
        {0, 8},     // Stabilizing
        {-1, 6},    // Minor left
        {1, 4},     // Minor right
        {0, 2},     // Minimal
        {0, 1}      // End pattern
    };
}

std::vector<RecoilPoint> WeaponProfiles::getPistolPattern() {
    return {
        {0, 10},    // Initial kick
        {1, 12},    // Slight right
        {-1, 14},   // Left correction
        {2, 15},    // Right drift
        {-2, 16},   // Left correction
        {1, 14},    // Stabilize
        {0, 12},    // Center
        {-1, 10},   // Minor left
        {1, 8},     // Minor right
        {0, 6}      // End pattern
    };
}