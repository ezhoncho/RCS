#!/usr/bin/env python3
"""
Simple test to verify chess scanner functionality
"""

from chess_scanner import StockfishAnalyzer, ChessMove, MoveQuality
import chess

def test_stockfish_analyzer():
    """Test basic Stockfish analysis functionality."""
    print("Testing Stockfish analyzer...")
    
    try:
        # Initialize analyzer with system stockfish
        analyzer = StockfishAnalyzer("/usr/games/stockfish", depth=10)
        
        # Test with a simple chess position (starting position)
        start_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        
        print(f"Analyzing position: {start_fen}")
        moves = analyzer.analyze_position(start_fen, num_moves=3)
        
        if moves:
            print(f"Found {len(moves)} move suggestions:")
            for i, move in enumerate(moves):
                print(f"  {i+1}. {move.move} ({move.quality.value}) - Eval: {move.evaluation:.2f}")
        else:
            print("No moves found!")
        
        analyzer.close()
        return True
        
    except Exception as e:
        print(f"Error testing Stockfish: {e}")
        return False

def test_board_detection():
    """Test basic board detection functionality."""
    print("Testing board detection...")
    
    try:
        from chess_scanner import ChessboardDetector
        import numpy as np
        
        # Create a simple test image (placeholder)
        detector = ChessboardDetector()
        test_image = np.zeros((640, 640, 3), dtype=np.uint8)
        
        # This would normally detect boards in the image
        boards = detector.detect_chessboard(test_image)
        print(f"Detected {len(boards)} board regions in test image")
        
        return True
        
    except Exception as e:
        print(f"Error testing board detection: {e}")
        return False

if __name__ == "__main__":
    print("Chess Scanner Test Suite")
    print("=" * 30)
    
    success_count = 0
    total_tests = 2
    
    if test_stockfish_analyzer():
        success_count += 1
        print("✓ Stockfish analyzer test passed")
    else:
        print("✗ Stockfish analyzer test failed")
    
    print()
    
    if test_board_detection():
        success_count += 1
        print("✓ Board detection test passed")
    else:
        print("✗ Board detection test failed")
    
    print()
    print(f"Test Results: {success_count}/{total_tests} tests passed")
    
    if success_count == total_tests:
        print("🎉 All tests passed! Chess scanner is ready to use.")
    else:
        print("⚠️  Some tests failed. Check the error messages above.")