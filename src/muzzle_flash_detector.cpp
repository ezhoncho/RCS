#include "muzzle_flash_detector.h"
#include <chrono>
#include <iostream>

MuzzleFlashDetector::MuzzleFlashDetector() 
    : screenDC(nullptr), bitmap(nullptr), memoryDC(nullptr),
      scanX(0), scanY(0), scanWidth(10), scanHeight(10),
      flashColor(240, 240, 240), colorTolerance(20),
      reloadTimeMs(3000) {
    
    setupScreenCapture();
    
#ifdef PLATFORM_WINDOWS
    // Set default scan region to screen center
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    setScanRegion(screenWidth / 2 - 5, screenHeight / 2 - 5, 10, 10);
#else
    // For Linux demonstration - assume 1920x1080 screen
    setScanRegion(960 - 5, 540 - 5, 10, 10);
#endif
    
    resetTimer();
}

MuzzleFlashDetector::~MuzzleFlashDetector() {
    cleanup();
}

bool MuzzleFlashDetector::setupScreenCapture() {
#ifdef PLATFORM_WINDOWS
    screenDC = GetDC(nullptr);
    if (!screenDC) return false;
    
    memoryDC = CreateCompatibleDC(screenDC);
    if (!memoryDC) {
        ReleaseDC(nullptr, screenDC);
        return false;
    }
    
    // Setup bitmap info for pixel reading
    memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = scanWidth;
    bitmapInfo.bmiHeader.biHeight = -scanHeight; // Negative for top-down
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    return true;
#else
    // For Linux demonstration - just return true
    std::cout << "Screen capture setup (Linux demo mode)" << std::endl;
    return true;
#endif
}

void MuzzleFlashDetector::cleanup() {
#ifdef PLATFORM_WINDOWS
    if (bitmap) {
        DeleteObject(bitmap);
        bitmap = nullptr;
    }
    if (memoryDC) {
        DeleteDC(memoryDC);
        memoryDC = nullptr;
    }
    if (screenDC) {
        ReleaseDC(nullptr, screenDC);
        screenDC = nullptr;
    }
#endif
}

void MuzzleFlashDetector::setScanRegion(int x, int y, int width, int height) {
    scanX = x;
    scanY = y;
    scanWidth = width;
    scanHeight = height;
    
#ifdef PLATFORM_WINDOWS
    // Update bitmap info for new size
    bitmapInfo.bmiHeader.biWidth = scanWidth;
    bitmapInfo.bmiHeader.biHeight = -scanHeight;
#endif
}

bool MuzzleFlashDetector::detectFlash() {
#ifdef PLATFORM_WINDOWS
    if (!screenDC || !memoryDC) {
        return false;
    }
    
    bool flashFound = scanRegionForFlash();
    
    if (flashFound) {
        lastShotTime = std::chrono::steady_clock::now();
    }
    
    return flashFound;
#else
    // For Linux demonstration - simulate flash detection occasionally
    static auto lastSimulatedFlash = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastSimulatedFlash).count();
    
    // Simulate a flash every 5 seconds for demo
    if (elapsed >= 5) {
        lastSimulatedFlash = now;
        lastShotTime = now;
        std::cout << "Simulated muzzle flash detected!" << std::endl;
        return true;
    }
    
    return false;
#endif
}

bool MuzzleFlashDetector::scanRegionForFlash() {
#ifdef PLATFORM_WINDOWS
    // Create a temporary bitmap for the scan region
    HBITMAP tempBitmap = CreateCompatibleBitmap(screenDC, scanWidth, scanHeight);
    if (!tempBitmap) return false;
    
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memoryDC, tempBitmap);
    
    // Copy screen region to memory DC
    bool success = BitBlt(memoryDC, 0, 0, scanWidth, scanHeight, 
                         screenDC, scanX, scanY, SRCCOPY);
    
    if (!success) {
        SelectObject(memoryDC, oldBitmap);
        DeleteObject(tempBitmap);
        return false;
    }
    
    // Check pixels for flash color
    bool flashDetected = false;
    int flashPixelCount = 0;
    int totalPixels = scanWidth * scanHeight;
    int requiredFlashPixels = totalPixels / 4; // 25% of pixels must match
    
    for (int y = 0; y < scanHeight && !flashDetected; ++y) {
        for (int x = 0; x < scanWidth; ++x) {
            COLORREF pixel = GetPixel(memoryDC, x, y);
            RGBColor pixelColor(GetRValue(pixel), GetGValue(pixel), GetBValue(pixel));
            
            if (flashColor.matches(pixelColor, colorTolerance)) {
                flashPixelCount++;
                if (flashPixelCount >= requiredFlashPixels) {
                    flashDetected = true;
                    break;
                }
            }
        }
    }
    
    // Cleanup
    SelectObject(memoryDC, oldBitmap);
    DeleteObject(tempBitmap);
    
    return flashDetected;
#else
    // For Linux demo - just return false as this is handled in detectFlash()
    return false;
#endif
}

bool MuzzleFlashDetector::shouldResetPattern() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastShot = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastShotTime).count();
    
    return timeSinceLastShot >= reloadTimeMs;
}

void MuzzleFlashDetector::resetTimer() {
    lastShotTime = std::chrono::steady_clock::now();
}