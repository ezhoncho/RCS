#pragma once

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
// For Linux demonstration - basic types
typedef unsigned int COLORREF;
typedef void* HDC;
typedef void* HBITMAP;
typedef struct {
    // Minimal BITMAPINFO structure for demo
    int biSize;
    int biWidth;
    int biHeight;
    short biPlanes;
    short biBitCount;
    int biCompression;
} BITMAPINFOHEADER;
typedef struct {
    BITMAPINFOHEADER bmiHeader;
} BITMAPINFO;
#endif

#include <chrono>
#include <vector>
#include <cmath>

struct RGBColor {
    int r, g, b;
    RGBColor(int r = 0, int g = 0, int b = 0) : r(r), g(g), b(b) {}
    
    bool matches(const RGBColor& other, int tolerance = 10) const {
        return std::abs(r - other.r) <= tolerance &&
               std::abs(g - other.g) <= tolerance &&
               std::abs(b - other.b) <= tolerance;
    }
};

class MuzzleFlashDetector {
public:
    MuzzleFlashDetector();
    ~MuzzleFlashDetector();
    
    bool detectFlash();
    void updateFlashColor(const RGBColor& color) { flashColor = color; }
    void setScanRegion(int x, int y, int width, int height);
    
    // Auto-reset functionality
    bool shouldResetPattern();
    void resetTimer();
    
private:
    HDC screenDC;
    HBITMAP bitmap;
    HDC memoryDC;
    BITMAPINFO bitmapInfo;
    
    // Scan region (default: center 10x10)
    int scanX, scanY, scanWidth, scanHeight;
    
    // Flash detection
    RGBColor flashColor;
    int colorTolerance;
    
    // Auto-reset timer
    std::chrono::steady_clock::time_point lastShotTime;
    int reloadTimeMs;
    
    // Screen capture
    bool setupScreenCapture();
    void cleanup();
    RGBColor getPixelColor(int x, int y);
    bool scanRegionForFlash();
};