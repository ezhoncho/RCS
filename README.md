# RCS - Recoil Control System

A sophisticated ImGui-based external recoil control system for Roblox Fallen Survival, built with modern C++ and featuring a dark gray/white/purple themed interface.

## Features

### 🎯 External Control System
- **WinAPI Integration**: Uses `SendInput` for reliable mouse control without memory reading
- **Anti-Detection**: Randomized timing (±15ms) and smooth movement algorithms
- **Performance Optimized**: Low CPU usage with efficient update loops

### 🔫 Weapon Profile System
- **Pre-configured Profiles**: Assault Rifle, SMG, and Pistol with realistic patterns
- **Custom Pattern Builder**: Visual editor for creating {x, y} vector patterns
- **Adjustable Settings**: 
  - Vertical control (0.0-2.0)
  - Horizontal sway (0.0-1.0) 
  - Shot interval timing (50-200ms)

### 👁️ Muzzle Flash Detection
- **Screen Scanning**: 10x10 pixel center region monitoring
- **Color Detection**: RGB(240, 240, 240) flash recognition
- **Auto-Reset**: 3-second pattern reset after reload

### 🎮 User Interface
- **Modern ImGui Theme**: Dark gray background with vibrant white/purple accents
- **Real-time Controls**: Live parameter adjustment during gameplay
- **Status Indicators**: Clear ON/OFF and weapon status display
- **Collapsible Sections**: Organized, clean interface layout

### ⌨️ Hotkeys
- **F8**: Toggle recoil control system
- **F9**: Show/hide UI interface

## Build Instructions

### Windows (Primary Target)
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Linux (Demo/Development)
```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev
mkdir build && cd build
cmake ..
make -j4
```

## Architecture

### Core Components

1. **RecoilControl** - Main system controller
2. **WeaponProfiles** - Pattern and weapon management
3. **MuzzleFlashDetector** - Screen capture and detection
4. **InputController** - Mouse control and anti-detection
5. **UIManager** - ImGui interface management

### Example Weapon Pattern
```cpp
// Assault Rifle Pattern
{
    {0, 15},    // Initial kick
    {2, 17},    // Slight right drift
    {-3, 18},   // Left correction
    {4, 20},    // Right drift
    {-2, 22},   // Left correction
    // ... continues for full pattern
}
```

### Safety Features
- **Randomization**: Movement timing varies ±15ms
- **Smoothing**: Multi-step mouse movements
- **External Only**: No game memory access
- **Pattern Limits**: Realistic compensation only

## Usage

1. **Launch the application** as Administrator (Windows)
2. **Select weapon profile** from the dropdown
3. **Adjust sensitivity** settings as needed
4. **Press F8** to activate recoil control
5. **Press F9** to toggle UI visibility
6. **Start gaming** - the system detects muzzle flashes automatically

## Platform Support

- **Windows**: Full functionality with D3D11 + WinAPI
- **Linux**: Demo version with GLFW + OpenGL3 (development/testing)

## Code Structure

```
src/
├── main.cpp                 # Application entry point
├── recoil_control.{h,cpp}   # Main control system
├── weapon_profiles.{h,cpp}  # Weapon pattern management
├── muzzle_flash_detector.{h,cpp} # Screen detection
├── input_controller.{h,cpp} # Mouse control
└── ui_manager.{h,cpp}       # ImGui interface
```

## Configuration

The system supports extensive customization:
- Sensitivity multipliers for vertical/horizontal control
- Per-weapon pattern adjustments
- Flash detection color and tolerance settings
- Timing and randomization parameters

## Legal & Ethical Use

This software is intended for educational and research purposes. Users are responsible for ensuring compliance with game terms of service and applicable laws in their jurisdiction.

---

**Note**: This implementation prioritizes safety, performance, and user experience while maintaining external-only operation for maximum compatibility and security.