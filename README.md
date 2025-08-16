# Chess Board Scanner & AI Move Suggester 🎯♟️

A comprehensive Python application that automatically detects chess boards on websites, recognizes piece positions, and provides AI-powered move suggestions using the Stockfish engine.

## 🚀 Features

- **Automatic Chess Board Detection**: Detects chess boards on websites like chess.com, lichess.org, and others using advanced computer vision
- **Position Recognition**: Extracts current chess positions from detected boards using image analysis
- **Stockfish Integration**: Leverages the powerful Stockfish engine for deep position analysis
- **Move Quality Classification**: Classifies moves as brilliant, excellent, great, good, inaccurate, mistake, or blunder
- **Visual Overlays**: 
  - Red box outlining detected chess boards
  - Colored arrows showing suggested moves
  - Green highlighting for piece origins and destinations
  - Red highlighting for capturing moves
- **Multi-Website Support**: Compatible with various chess platforms
- **Cross-Platform**: Runs on Windows, macOS, and Linux

## 📋 Requirements

- Python 3.8 or higher
- Stockfish chess engine
- Chrome browser (for web scanning)

## 🛠️ Installation

### 1. Install Stockfish Engine

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install stockfish
```

**macOS (with Homebrew):**
```bash
brew install stockfish
```

**Windows:**
1. Download from [Stockfish official website](https://stockfishchess.org/download/)
2. Extract to `C:\stockfish\` or your preferred location
3. Add the path to your system PATH or use `--stockfish` parameter

### 2. Install Python Dependencies

```bash
# Clone or download the repository
git clone https://github.com/ezhoncho/RCS.git
cd RCS

# Install required packages
pip install -r requirements.txt
```

### 3. Chrome Browser
Ensure Google Chrome is installed on your system. The application uses Chrome WebDriver for web automation.

## 🎮 Usage

### Command Line Interface

#### Scan a Chess Website
```bash
python chess_scanner.py --url "https://www.chess.com/play/online"
```

#### Scan a Local Image
```bash
python chess_scanner.py --image "path/to/chess_board.png"
```

#### Advanced Options
```bash
# Specify custom Stockfish path
python chess_scanner.py --url "https://lichess.org" --stockfish "/custom/path/to/stockfish"

# Run in headless mode (no browser window)
python chess_scanner.py --url "https://www.chess.com/play" --headless

# Custom output file
python chess_scanner.py --image "board.jpg" --output "my_analysis.png"
```

### Python API

```python
from chess_scanner import ChessScanner

# Initialize scanner
scanner = ChessScanner()

# Scan a website
success = scanner.scan_website("https://www.chess.com/play/online")

# Scan an image file  
success = scanner.scan_image("chess_board.png")

# Clean up
scanner.close()
```

## 📊 Output

The application generates:

1. **Analysis Image**: Visual overlay showing:
   - Red rectangle around detected chess board
   - Numbered colored arrows for top move suggestions
   - Color-coded move quality indicators
   - Square highlighting for piece movement

2. **Console Output**: Detailed analysis including:
   - Detected board confidence score
   - Recognized FEN position
   - Top move suggestions with evaluations
   - Move quality classifications

3. **Interactive Display**: Matplotlib window showing the analysis results

## 🎨 Move Quality Color Coding

| Quality | Color | Description |
|---------|--------|-------------|
| Brilliant | Gold | Exceptional move, often unexpected |
| Excellent | Green | Best or near-best move |
| Great | Light Blue | Very strong move |
| Good | Light Green | Solid, reasonable move |
| Inaccurate | Orange | Slightly suboptimal |
| Mistake | Tomato | Clear error |
| Blunder | Red | Major mistake |

## 🌐 Supported Websites

The chess scanner is designed to work with popular chess platforms:

- **Chess.com** - Live games, puzzles, analysis boards
- **Lichess.org** - Live games, studies, analysis
- **Chess24** - Live games and analysis
- **Custom chess boards** - Any website with visual chess boards

## 🔧 Configuration

### Custom Piece Templates

For improved piece recognition accuracy, you can add custom piece templates:

1. Create a `templates/` directory
2. Add piece images named as: `white_king.png`, `black_queen.png`, etc.
3. The system will automatically load and use these templates

### Stockfish Settings

The application automatically configures Stockfish with:
- **Depth**: 15 moves (configurable)
- **Threads**: 4 (utilizes multiple CPU cores)
- **Hash**: 256MB (memory allocation)

## 🐛 Troubleshooting

### Common Issues

**1. Stockfish not found**
```bash
# Specify the full path to Stockfish
python chess_scanner.py --url "https://chess.com" --stockfish "/usr/local/bin/stockfish"
```

**2. Chrome WebDriver issues**
- Ensure Chrome browser is installed and updated
- The webdriver-manager will automatically download the correct ChromeDriver

**3. No chess board detected**
- Ensure the chess board is clearly visible on screen
- Try different websites or adjust browser window size
- Check if the board has sufficient contrast

**4. Poor piece recognition**
- Chess board should be reasonably large and clear
- Avoid boards with unusual themes or heavy transparency
- Consider using standard piece sets for better accuracy

### Debug Mode

Enable detailed logging for troubleshooting:

```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

## 🔄 Algorithm Overview

### 1. Board Detection
- **Line Detection**: Uses HoughLines to find grid patterns
- **Contour Analysis**: Identifies square/rectangular shapes
- **Template Matching**: Matches against checkerboard patterns
- **Confidence Scoring**: Ranks detections by likelihood

### 2. Position Recognition
- **Square Extraction**: Divides board into 8×8 grid
- **Piece Classification**: Template matching with piece images
- **FEN Generation**: Converts board matrix to standard notation

### 3. Move Analysis
- **Stockfish Integration**: Multi-PV analysis for top moves
- **Evaluation Scoring**: Converts engine output to readable format
- **Quality Classification**: Categorizes moves by strength difference

### 4. Visual Enhancement
- **Overlay Generation**: Creates colored indicators
- **Move Arrows**: Shows piece movement paths
- **Square Highlighting**: Emphasizes important squares

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

- Enhanced piece recognition algorithms
- Support for more chess variants
- Mobile app integration
- Real-time video analysis
- Machine learning-based board detection

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **Stockfish Team** - For the powerful chess engine
- **Python-Chess** - Excellent chess programming library
- **OpenCV Community** - Computer vision capabilities
- **Chess.com & Lichess** - Inspiration for chess platform integration

## 📞 Support

For issues, questions, or feature requests:
1. Check the troubleshooting section above
2. Search existing GitHub issues
3. Create a new issue with detailed description
4. Include sample images/URLs when reporting bugs

---

**Happy Chess Analysis!** 🎉♟️

*"In chess, as in life, the best moves are often the ones you don't see coming."* - Chess Scanner AI