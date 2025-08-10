# RCS - Record Control System

A modern Macro Mouse Control program with both Qt GUI and console interfaces.

## Features

- **Mouse Movement Recording**: Capture precise mouse movements and positions
- **Click Recording**: Record left clicks, right clicks, and double clicks  
- **Macro Playback**: Play back recorded macros with adjustable speed
- **File Operations**: Save and load macros in RCS format (.rcs files)
- **Configurable Settings**: Toggle mouse movement recording, adjust playback speed
- **Modern UI**: Dark theme Qt interface with clean, intuitive controls
- **Cross-Platform**: Works on Windows, Linux, and macOS
- **Console Mode**: Text-based interface when Qt is not available

## Architecture

The RCS system is built with a clean, modular architecture:

### Core Components

1. **MouseController** (`mousecontroller.h/cpp`)
   - Handles low-level mouse operations
   - Cross-platform mouse positioning and clicking
   - Real-time mouse state monitoring

2. **MacroRecorder** (`macrorecorder.h/cpp`) 
   - Records and manages macro events
   - Handles playback with timing control
   - Supports save/load functionality

3. **MainWindow** (`mainwindow.h/cpp`)
   - Qt-based graphical user interface
   - Modern dark theme styling
   - Real-time status updates and controls

4. **Console UI** (`console_ui.h/cpp`)
   - Text-based interface for environments without Qt
   - Full functionality through menu system
   - Simulation mode for testing

## Building

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- Qt6 or Qt5 (optional, for GUI version)

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

The build system automatically detects Qt availability:
- **With Qt**: Builds the full GUI version with modern interface
- **Without Qt**: Builds the console version with text-based interface

### Running

```bash
# GUI version (when Qt is available)
./bin/rcs

# Console version  
./bin/rcs
```

## Usage

### GUI Version

1. **Recording**:
   - Click "Start Recording"
   - Perform mouse actions
   - Click "Stop Recording"

2. **Playback**:
   - Click "Play Macro" 
   - Adjust speed with the speed control
   - Click "Stop Playback" to cancel

3. **File Operations**:
   - Use "Load Macro" and "Save Macro" buttons
   - Macros are saved in .rcs format

### Console Version

Navigate through the text menu:
- Option 1: Start Recording
- Option 2: Stop Recording  
- Option 3: Play Macro
- Option 9: Simulate Mouse Actions (for testing)

## File Format

RCS uses a simple text-based format for macro files:

```
# RCS Macro File v1.0
# Format: type,x,y,timestamp,delay
# Types: 0=MouseMove, 1=LeftClick, 2=RightClick, 3=DoubleClick, 4=Delay
0,100,100,1000,0    # Mouse move to (100,100) at 1000ms
1,100,100,1500,0    # Left click at (100,100) at 1500ms
```

## Platform Support

- **Windows**: Full functionality with native Windows API
- **Linux**: Full functionality with X11 (requires X11 development libraries)
- **Simulation Mode**: Works on any platform for testing and development

## Styling

The Qt GUI version features a modern dark theme with:
- Dark color scheme with blue accents
- Smooth animations and hover effects
- Professional button styling
- Responsive layout design

All styling is defined in `style.qss` and can be customized.

## Development

### Code Structure

```
RCS/
├── main.cpp                     # Qt GUI entry point
├── main_console.cpp             # Console entry point
├── mousecontroller.h/cpp        # Qt-based mouse control
├── mousecontroller_console.h/cpp # Console mouse control  
├── macrorecorder.h/cpp          # Qt-based macro system
├── macrorecorder_console.h/cpp  # Console macro system
├── mainwindow.h/cpp             # Qt GUI implementation
├── console_ui.h/cpp             # Console interface
├── style.qss                    # Qt styling
├── CMakeLists.txt               # Build configuration
└── sample_macro.rcs             # Example macro file
```

### Building with Qt

To build with Qt support, ensure Qt6 or Qt5 development packages are installed:

```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-widgets-dev

# Or Qt5
sudo apt install qt5-default qtbase5-dev

# Windows
# Install Qt from https://www.qt.io/

# macOS  
brew install qt6
```

## License

This project is open source. See LICENSE file for details.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request