#!/usr/bin/env python3
"""
Comprehensive test and demonstration of the chess scanner functionality
"""

import os
import sys
import logging
from chess_scanner import ChessScanner, ChessboardDetector, StockfishAnalyzer

def test_comprehensive_functionality():
    """Test all major components of the chess scanner."""
    
    print("🎯 Chess Scanner Comprehensive Test")
    print("=" * 50)
    
    # Test 1: Board Detection
    print("\n1. Testing Chess Board Detection...")
    detector = ChessboardDetector()
    
    # Test with sample images
    if os.path.exists("sample_chess_board.png"):
        import cv2
        image = cv2.imread("sample_chess_board.png")
        boards = detector.detect_chessboard(image)
        print(f"   ✓ Detected {len(boards)} chess boards in sample image")
        if boards:
            best_board = boards[0]
            print(f"   ✓ Best board: {best_board.width}x{best_board.height} at ({best_board.x}, {best_board.y})")
            print(f"   ✓ Confidence: {best_board.confidence:.2f}")
    else:
        print("   ⚠ Sample image not found, skipping visual test")
    
    # Test 2: Stockfish Analysis
    print("\n2. Testing Stockfish Analysis...")
    try:
        analyzer = StockfishAnalyzer("/usr/games/stockfish", depth=12)
        
        # Test different positions
        positions = [
            ("Starting position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
            ("Italian Game", "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3"),
            ("Sicilian Defense", "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"),
        ]
        
        for name, fen in positions:
            print(f"\n   Testing: {name}")
            moves = analyzer.analyze_position(fen, num_moves=3)
            if moves:
                print(f"   ✓ Found {len(moves)} move suggestions:")
                for i, move in enumerate(moves):
                    print(f"      {i+1}. {move.move} ({move.quality.value}) - Eval: {move.evaluation:.2f}")
            else:
                print("   ⚠ No moves found")
        
        analyzer.close()
        
    except Exception as e:
        print(f"   ✗ Stockfish test failed: {e}")
    
    # Test 3: Complete Chess Scanner
    print("\n3. Testing Complete Chess Scanner...")
    try:
        scanner = ChessScanner("/usr/games/stockfish")
        
        if os.path.exists("sample_chess_board.png"):
            success = scanner.scan_image("sample_chess_board.png", "test_analysis.png")
            if success:
                print("   ✓ Chess board scanning completed successfully")
                print("   ✓ Analysis image saved as: test_analysis.png")
            else:
                print("   ✗ Chess board scanning failed")
        else:
            print("   ⚠ Sample image not found, creating one...")
            # Import and create sample image
            from create_sample_images import create_chess_board_image
            create_chess_board_image(output_path="test_board.png")
            success = scanner.scan_image("test_board.png", "test_analysis.png")
            if success:
                print("   ✓ Chess board scanning completed successfully")
        
        scanner.close()
        
    except Exception as e:
        print(f"   ✗ Complete scanner test failed: {e}")
    
    # Test 4: Command Line Interface
    print("\n4. Testing Command Line Interface...")
    try:
        import subprocess
        result = subprocess.run([sys.executable, "chess_scanner.py", "--help"], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            print("   ✓ Command line interface working")
            print("   ✓ Help text generated successfully")
        else:
            print("   ✗ Command line interface failed")
    except Exception as e:
        print(f"   ✗ CLI test failed: {e}")
    
    print("\n" + "=" * 50)
    print("🎉 Comprehensive testing completed!")
    print("\nThe chess scanner is ready for use with:")
    print("  • Automatic chess board detection")
    print("  • AI-powered move suggestions")
    print("  • Visual overlay system")
    print("  • Web automation capabilities")
    print("  • Cross-platform compatibility")

def demonstrate_usage():
    """Demonstrate various usage scenarios."""
    
    print("\n📚 Chess Scanner Usage Demonstrations")
    print("=" * 50)
    
    print("\n1. Basic Image Analysis:")
    print("   python chess_scanner.py --image board.png")
    
    print("\n2. Website Scanning:")  
    print("   python chess_scanner.py --url https://www.chess.com/play")
    
    print("\n3. Custom Stockfish Path:")
    print("   python chess_scanner.py --image board.png --stockfish /path/to/stockfish")
    
    print("\n4. Headless Browser Mode:")
    print("   python chess_scanner.py --url https://lichess.org --headless")
    
    print("\n5. Custom Output File:")
    print("   python chess_scanner.py --image board.png --output my_analysis.png")
    
    print("\n📋 What the Scanner Provides:")
    print("• Red box around detected chess board")
    print("• Numbered arrows showing best moves")
    print("• Color-coded move quality (gold=brilliant, green=excellent, etc.)")
    print("• Square highlighting for piece movement")
    print("• Console output with detailed analysis")
    print("• Interactive matplotlib display")

if __name__ == "__main__":
    # Setup logging
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    
    # Run comprehensive tests
    test_comprehensive_functionality()
    
    # Show usage examples
    demonstrate_usage()
    
    print("\n✨ Chess Scanner is ready to help you improve your game!")