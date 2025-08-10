#include "console_ui.h"
#include <iostream>

int main(int argc, char *argv[])
{
    // Note: This is the console version of RCS
    // The Qt GUI version is available when Qt is installed
    
    std::cout << "RCS - Record Control System" << std::endl;
    std::cout << "Console Version (Qt GUI version available with Qt installation)" << std::endl;
    std::cout << "===============================================================" << std::endl;
    
    try {
        ConsoleUI ui;
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}