#!/usr/bin/env python3
"""
Chess Board Scanner and AI Move Suggester

A comprehensive Python script that scans websites for chess boards and provides
AI-powered move suggestions using the Stockfish engine.

Features:
- Automatic chess board detection on websites
- Position recognition using computer vision
- Stockfish integration for best move analysis
- Move quality classification (brilliant, excellent, great, good)
- Visual overlays with colored highlighting
- Support for multiple chess websites

Author: Chess Scanner AI
Version: 1.0.0
"""

import cv2
import numpy as np
import chess
import chess.engine
import chess.pgn
from PIL import Image, ImageDraw, ImageFont
import matplotlib.pyplot as plt
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager
import requests
import time
import os
import sys
import argparse
import json
import logging
from typing import List, Tuple, Dict, Optional, Union
from dataclasses import dataclass
from enum import Enum


class MoveQuality(Enum):
    """Enumeration for move quality classifications."""
    BRILLIANT = "brilliant"
    EXCELLENT = "excellent"
    GREAT = "great"
    GOOD = "good"
    INACCURATE = "inaccurate"
    MISTAKE = "mistake"
    BLUNDER = "blunder"


@dataclass
class ChessMove:
    """Data class representing a chess move with analysis."""
    move: chess.Move
    evaluation: float
    quality: MoveQuality
    depth: int = 15
    pv_line: List[chess.Move] = None


@dataclass
class BoardRegion:
    """Data class representing a detected chess board region."""
    x: int
    y: int
    width: int
    height: int
    confidence: float


class ChessboardDetector:
    """Chess board detection using computer vision techniques."""
    
    def __init__(self, min_board_size=200):
        self.min_board_size = min_board_size
        self.board_aspect_ratio = 1.0  # Chess boards are square
        self.tolerance = 0.15  # Aspect ratio tolerance
        
    def detect_chessboard(self, image: np.ndarray) -> List[BoardRegion]:
        """
        Detect chess boards in the given image using multiple techniques.
        
        Args:
            image: Input image as numpy array
            
        Returns:
            List of detected board regions with confidence scores
        """
        boards = []
        
        # Method 1: Template matching with chessboard patterns
        boards.extend(self._detect_by_lines(image))
        
        # Method 2: Contour-based detection
        boards.extend(self._detect_by_contours(image))
        
        # Method 3: Color-based detection (alternating squares)
        boards.extend(self._detect_by_colors(image))
        
        # Filter and merge overlapping detections
        return self._merge_detections(boards)
    
    def _detect_by_lines(self, image: np.ndarray) -> List[BoardRegion]:
        """Detect chess boards by finding grid lines."""
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        
        # Detect lines using HoughLines
        edges = cv2.Canny(gray, 50, 150, apertureSize=3)
        lines = cv2.HoughLines(edges, 1, np.pi/180, threshold=100)
        
        boards = []
        if lines is not None:
            # Group lines into horizontal and vertical
            horizontal_lines = []
            vertical_lines = []
            
            for line in lines:
                rho, theta = line[0]
                if abs(theta) < np.pi/4 or abs(theta - np.pi) < np.pi/4:
                    horizontal_lines.append((rho, theta))
                else:
                    vertical_lines.append((rho, theta))
            
            # If we have enough lines, try to form a grid
            if len(horizontal_lines) >= 7 and len(vertical_lines) >= 7:
                boards.append(self._extract_grid_region(horizontal_lines, vertical_lines, image.shape))
        
        return boards
    
    def _detect_by_contours(self, image: np.ndarray) -> List[BoardRegion]:
        """Detect chess boards by finding square contours."""
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        blurred = cv2.GaussianBlur(gray, (5, 5), 0)
        
        # Adaptive threshold
        thresh = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, 
                                     cv2.THRESH_BINARY, 11, 2)
        
        # Find contours
        contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        
        boards = []
        for contour in contours:
            # Approximate contour
            epsilon = 0.02 * cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, epsilon, True)
            
            if len(approx) == 4:  # Quadrilateral
                x, y, w, h = cv2.boundingRect(approx)
                
                # Check if it's roughly square and large enough
                aspect_ratio = w / h
                if (abs(aspect_ratio - 1.0) < self.tolerance and 
                    w >= self.min_board_size and h >= self.min_board_size):
                    
                    confidence = self._calculate_confidence(image[y:y+h, x:x+w])
                    boards.append(BoardRegion(x, y, w, h, confidence))
        
        return boards
    
    def _detect_by_colors(self, image: np.ndarray) -> List[BoardRegion]:
        """Detect chess boards by looking for alternating light/dark square patterns."""
        # Convert to HSV for better color analysis
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        
        # Look for regions with alternating brightness patterns
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        
        boards = []
        # Use template matching with a simple checkerboard pattern
        template_size = 64
        checkerboard = self._create_checkerboard_template(template_size)
        
        result = cv2.matchTemplate(gray, checkerboard, cv2.TM_CCOEFF_NORMED)
        locations = np.where(result >= 0.3)
        
        for pt in zip(*locations[::-1]):
            x, y = pt
            w = h = template_size * 8  # 8x8 board
            
            if x + w <= image.shape[1] and y + h <= image.shape[0]:
                confidence = result[y, x]
                boards.append(BoardRegion(x, y, w, h, confidence))
        
        return boards
    
    def _create_checkerboard_template(self, square_size: int) -> np.ndarray:
        """Create a checkerboard pattern template for matching."""
        size = square_size * 8
        template = np.zeros((size, size), dtype=np.uint8)
        
        for i in range(8):
            for j in range(8):
                if (i + j) % 2 == 0:
                    y1, y2 = i * square_size, (i + 1) * square_size
                    x1, x2 = j * square_size, (j + 1) * square_size
                    template[y1:y2, x1:x2] = 255
        
        return template
    
    def _calculate_confidence(self, board_region: np.ndarray) -> float:
        """Calculate confidence score for a detected board region."""
        # Simple confidence based on edge density and alternating patterns
        gray = cv2.cvtColor(board_region, cv2.COLOR_BGR2GRAY)
        edges = cv2.Canny(gray, 50, 150)
        edge_density = np.sum(edges) / (edges.shape[0] * edges.shape[1])
        
        return min(edge_density / 100.0, 1.0)
    
    def _extract_grid_region(self, h_lines: List, v_lines: List, image_shape: Tuple) -> BoardRegion:
        """Extract board region from detected grid lines."""
        # Simple implementation - find bounding box of line intersections
        # This is a simplified version; a full implementation would be more complex
        height, width = image_shape[:2]
        
        # Estimate board region (placeholder implementation)
        x, y = width // 4, height // 4
        size = min(width, height) // 2
        
        return BoardRegion(x, y, size, size, 0.5)
    
    def _merge_detections(self, boards: List[BoardRegion]) -> List[BoardRegion]:
        """Merge overlapping board detections and return best candidates."""
        if not boards:
            return []
        
        # Sort by confidence
        boards.sort(key=lambda b: b.confidence, reverse=True)
        
        merged = []
        for board in boards:
            # Check if this board overlaps significantly with any existing board
            overlaps = False
            for existing in merged:
                if self._calculate_overlap(board, existing) > 0.5:
                    overlaps = True
                    break
            
            if not overlaps:
                merged.append(board)
        
        return merged[:3]  # Return top 3 candidates
    
    def _calculate_overlap(self, board1: BoardRegion, board2: BoardRegion) -> float:
        """Calculate overlap ratio between two board regions."""
        x1 = max(board1.x, board2.x)
        y1 = max(board1.y, board2.y)
        x2 = min(board1.x + board1.width, board2.x + board2.width)
        y2 = min(board1.y + board1.height, board2.y + board2.height)
        
        if x2 <= x1 or y2 <= y1:
            return 0.0
        
        intersection = (x2 - x1) * (y2 - y1)
        area1 = board1.width * board1.height
        area2 = board2.width * board2.height
        union = area1 + area2 - intersection
        
        return intersection / union if union > 0 else 0.0


class PositionRecognizer:
    """Recognize chess piece positions from detected board images."""
    
    def __init__(self):
        self.piece_templates = self._load_piece_templates()
        
    def _load_piece_templates(self) -> Dict[str, np.ndarray]:
        """Load piece templates for recognition. In a real implementation, 
        this would load actual piece image templates."""
        # Placeholder - in reality, you'd load actual piece templates
        templates = {}
        pieces = ['P', 'R', 'N', 'B', 'Q', 'K', 'p', 'r', 'n', 'b', 'q', 'k']
        
        for piece in pieces:
            # Create placeholder templates (in reality, load from files)
            templates[piece] = np.zeros((64, 64, 3), dtype=np.uint8)
        
        return templates
    
    def recognize_position(self, board_image: np.ndarray) -> str:
        """
        Recognize the current chess position from a board image.
        
        Args:
            board_image: Image of the chess board
            
        Returns:
            FEN string representing the position
        """
        # Divide the board into 8x8 squares
        squares = self._extract_squares(board_image)
        
        # Recognize pieces in each square
        board_matrix = []
        for row in range(8):
            board_row = []
            for col in range(8):
                square = squares[row][col]
                piece = self._recognize_piece_in_square(square)
                board_row.append(piece)
            board_matrix.append(board_row)
        
        # Convert to FEN
        return self._matrix_to_fen(board_matrix)
    
    def _extract_squares(self, board_image: np.ndarray) -> List[List[np.ndarray]]:
        """Extract individual squares from the board image."""
        height, width = board_image.shape[:2]
        square_height = height // 8
        square_width = width // 8
        
        squares = []
        for row in range(8):
            square_row = []
            for col in range(8):
                y1 = row * square_height
                y2 = (row + 1) * square_height
                x1 = col * square_width
                x2 = (col + 1) * square_width
                
                square = board_image[y1:y2, x1:x2]
                square_row.append(square)
            squares.append(square_row)
        
        return squares
    
    def _recognize_piece_in_square(self, square: np.ndarray) -> Optional[str]:
        """Recognize which piece (if any) is in the given square."""
        # Simplified piece recognition using template matching
        # In a real implementation, this would use more sophisticated methods
        
        # Check if square is empty (using color analysis)
        if self._is_empty_square(square):
            return None
        
        # Try to match against piece templates
        best_match = None
        best_score = 0
        
        gray_square = cv2.cvtColor(square, cv2.COLOR_BGR2GRAY)
        
        for piece, template in self.piece_templates.items():
            if len(template.shape) == 3:
                template_gray = cv2.cvtColor(template, cv2.COLOR_BGR2GRAY)
            else:
                template_gray = template
            
            # Resize template to match square size
            template_resized = cv2.resize(template_gray, 
                                        (square.shape[1], square.shape[0]))
            
            # Template matching
            result = cv2.matchTemplate(gray_square, template_resized, cv2.TM_CCOEFF_NORMED)
            score = np.max(result)
            
            if score > best_score:
                best_score = score
                best_match = piece
        
        # Return piece if confidence is high enough
        return best_match if best_score > 0.5 else None
    
    def _is_empty_square(self, square: np.ndarray) -> bool:
        """Check if a square appears to be empty."""
        # Simple check based on color variance
        gray = cv2.cvtColor(square, cv2.COLOR_BGR2GRAY)
        variance = np.var(gray)
        return variance < 500  # Threshold for "empty" squares
    
    def _matrix_to_fen(self, board_matrix: List[List[Optional[str]]]) -> str:
        """Convert board matrix to FEN string."""
        fen_rows = []
        
        for row in board_matrix:
            fen_row = ""
            empty_count = 0
            
            for piece in row:
                if piece is None:
                    empty_count += 1
                else:
                    if empty_count > 0:
                        fen_row += str(empty_count)
                        empty_count = 0
                    fen_row += piece
            
            if empty_count > 0:
                fen_row += str(empty_count)
            
            fen_rows.append(fen_row)
        
        # Join rows with '/' and add default game state info
        board_fen = "/".join(fen_rows)
        return f"{board_fen} w - - 0 1"  # Assume white to move, no castling, etc.


class StockfishAnalyzer:
    """Chess position analysis using Stockfish engine."""
    
    def __init__(self, stockfish_path: str = None, depth: int = 15):
        self.depth = depth
        self.stockfish_path = stockfish_path or self._find_stockfish()
        self.engine = None
        
        try:
            self.engine = chess.engine.SimpleEngine.popen_uci(self.stockfish_path)
            self.engine.configure({"Threads": 4, "Hash": 256})
        except Exception as e:
            logging.error(f"Failed to initialize Stockfish: {e}")
            raise
    
    def _find_stockfish(self) -> str:
        """Try to find Stockfish executable."""
        possible_paths = [
            "stockfish",
            "/usr/local/bin/stockfish",
            "/usr/bin/stockfish",
            "/opt/homebrew/bin/stockfish",
            "C:/Program Files/Stockfish/stockfish.exe",
            "C:/stockfish/stockfish.exe"
        ]
        
        for path in possible_paths:
            try:
                # Test if the path works
                engine = chess.engine.SimpleEngine.popen_uci(path)
                engine.quit()
                return path
            except:
                continue
        
        raise FileNotFoundError("Stockfish executable not found. Please install Stockfish or specify the path.")
    
    def analyze_position(self, fen: str, num_moves: int = 5) -> List[ChessMove]:
        """
        Analyze a chess position and return top moves with evaluations.
        
        Args:
            fen: FEN string of the position
            num_moves: Number of top moves to return
            
        Returns:
            List of ChessMove objects with analysis
        """
        if not self.engine:
            raise RuntimeError("Stockfish engine not initialized")
        
        try:
            board = chess.Board(fen)
            
            # Get multi-PV analysis
            info = self.engine.analyse(board, chess.engine.Limit(depth=self.depth), 
                                     multipv=num_moves)
            
            moves = []
            for i, pv_info in enumerate(info):
                if "pv" in pv_info and len(pv_info["pv"]) > 0:
                    move = pv_info["pv"][0]
                    score = pv_info.get("score", chess.engine.PovScore(chess.engine.Mate(0), chess.WHITE))
                    
                    # Convert score to centipawns
                    if score.is_mate():
                        eval_score = 1000 if score.mate() > 0 else -1000
                    else:
                        eval_score = score.relative.score() / 100.0  # Convert to pawns
                    
                    # Classify move quality
                    quality = self._classify_move_quality(i, eval_score, info[0].get("score") if i > 0 else None)
                    
                    chess_move = ChessMove(
                        move=move,
                        evaluation=eval_score,
                        quality=quality,
                        depth=self.depth,
                        pv_line=pv_info["pv"][:5]  # First 5 moves of principal variation
                    )
                    moves.append(chess_move)
            
            return moves
            
        except Exception as e:
            logging.error(f"Error analyzing position: {e}")
            return []
    
    def _classify_move_quality(self, move_index: int, eval_score: float, best_score) -> MoveQuality:
        """Classify move quality based on evaluation difference from best move."""
        if move_index == 0:  # Best move
            if best_score and best_score.is_mate() and best_score.mate() > 0:
                return MoveQuality.BRILLIANT
            return MoveQuality.EXCELLENT
        
        if not best_score:
            return MoveQuality.GOOD
        
        # Calculate evaluation difference
        if best_score.is_mate():
            best_eval = 1000 if best_score.mate() > 0 else -1000
        else:
            best_eval = best_score.relative.score() / 100.0
        
        diff = abs(best_eval - eval_score)
        
        if diff < 0.1:
            return MoveQuality.EXCELLENT
        elif diff < 0.3:
            return MoveQuality.GREAT
        elif diff < 0.7:
            return MoveQuality.GOOD
        elif diff < 1.5:
            return MoveQuality.INACCURATE
        elif diff < 3.0:
            return MoveQuality.MISTAKE
        else:
            return MoveQuality.BLUNDER
    
    def close(self):
        """Close the Stockfish engine."""
        if self.engine:
            self.engine.quit()


class VisualOverlay:
    """Create visual overlays on chess board images."""
    
    def __init__(self):
        self.colors = {
            MoveQuality.BRILLIANT: (255, 215, 0),    # Gold
            MoveQuality.EXCELLENT: (0, 255, 0),      # Green
            MoveQuality.GREAT: (173, 216, 230),      # Light Blue
            MoveQuality.GOOD: (144, 238, 144),       # Light Green
            MoveQuality.INACCURATE: (255, 165, 0),   # Orange
            MoveQuality.MISTAKE: (255, 99, 71),      # Tomato
            MoveQuality.BLUNDER: (255, 0, 0)         # Red
        }
    
    def create_overlay(self, image: np.ndarray, board_region: BoardRegion, 
                      moves: List[ChessMove], board_fen: str) -> np.ndarray:
        """
        Create visual overlay showing move suggestions on the chess board.
        
        Args:
            image: Original image
            board_region: Detected board region
            moves: List of move suggestions
            board_fen: Current board position in FEN
            
        Returns:
            Image with overlays
        """
        overlay_image = image.copy()
        
        # Draw board detection box (red)
        cv2.rectangle(overlay_image, 
                     (board_region.x, board_region.y),
                     (board_region.x + board_region.width, board_region.y + board_region.height),
                     (0, 0, 255), 3)
        
        # Draw move suggestions
        if moves:
            self._draw_moves(overlay_image, board_region, moves, board_fen)
        
        return overlay_image
    
    def _draw_moves(self, image: np.ndarray, board_region: BoardRegion, 
                   moves: List[ChessMove], board_fen: str):
        """Draw move suggestions on the board."""
        try:
            board = chess.Board(board_fen)
            square_width = board_region.width // 8
            square_height = board_region.height // 8
            
            for i, move in enumerate(moves[:3]):  # Show top 3 moves
                if move.move in board.legal_moves:
                    from_square = move.move.from_square
                    to_square = move.move.to_square
                    
                    # Calculate square positions
                    from_col = from_square % 8
                    from_row = 7 - (from_square // 8)
                    to_col = to_square % 8
                    to_row = 7 - (to_square // 8)
                    
                    # Calculate pixel positions
                    from_x = board_region.x + from_col * square_width + square_width // 2
                    from_y = board_region.y + from_row * square_height + square_height // 2
                    to_x = board_region.x + to_col * square_width + square_width // 2
                    to_y = board_region.y + to_row * square_height + square_height // 2
                    
                    # Choose color based on move quality
                    color = self.colors.get(move.quality, (0, 255, 0))
                    
                    # Draw arrow for the move
                    self._draw_arrow(image, (from_x, from_y), (to_x, to_y), color, i + 1)
                    
                    # Highlight squares
                    self._highlight_square(image, board_region, from_col, from_row, color, alpha=0.3)
                    
                    # Different color for capture moves
                    if board.piece_at(to_square):
                        capture_color = (0, 0, 255)  # Red for captures
                        self._highlight_square(image, board_region, to_col, to_row, capture_color, alpha=0.5)
                    else:
                        self._highlight_square(image, board_region, to_col, to_row, color, alpha=0.3)
        
        except Exception as e:
            logging.error(f"Error drawing moves: {e}")
    
    def _draw_arrow(self, image: np.ndarray, start: Tuple[int, int], 
                   end: Tuple[int, int], color: Tuple[int, int, int], move_number: int):
        """Draw an arrow showing the move."""
        thickness = max(3, 6 - move_number)  # Thicker arrows for better moves
        
        # Draw line
        cv2.line(image, start, end, color, thickness)
        
        # Draw arrowhead
        angle = np.arctan2(end[1] - start[1], end[0] - start[0])
        arrow_length = 15
        
        # Calculate arrowhead points
        pt1 = (
            int(end[0] - arrow_length * np.cos(angle - np.pi / 6)),
            int(end[1] - arrow_length * np.sin(angle - np.pi / 6))
        )
        pt2 = (
            int(end[0] - arrow_length * np.cos(angle + np.pi / 6)),
            int(end[1] - arrow_length * np.sin(angle + np.pi / 6))
        )
        
        cv2.line(image, end, pt1, color, thickness)
        cv2.line(image, end, pt2, color, thickness)
        
        # Add move number
        cv2.putText(image, str(move_number), 
                   (start[0] - 10, start[1] - 10),
                   cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
    
    def _highlight_square(self, image: np.ndarray, board_region: BoardRegion, 
                         col: int, row: int, color: Tuple[int, int, int], alpha: float = 0.3):
        """Highlight a square on the chess board."""
        square_width = board_region.width // 8
        square_height = board_region.height // 8
        
        x1 = board_region.x + col * square_width
        y1 = board_region.y + row * square_height
        x2 = x1 + square_width
        y2 = y1 + square_height
        
        # Create overlay
        overlay = image.copy()
        cv2.rectangle(overlay, (x1, y1), (x2, y2), color, -1)
        
        # Blend with original image
        cv2.addWeighted(overlay, alpha, image, 1 - alpha, 0, image)


class ChessScanner:
    """Main chess scanning application."""
    
    def __init__(self, stockfish_path: str = None):
        self.detector = ChessboardDetector()
        self.recognizer = PositionRecognizer()
        self.analyzer = StockfishAnalyzer(stockfish_path)
        self.overlay = VisualOverlay()
        self.driver = None
        
        # Setup logging
        logging.basicConfig(level=logging.INFO, 
                          format='%(asctime)s - %(levelname)s - %(message)s')
        
    def setup_webdriver(self, headless: bool = False) -> webdriver.Chrome:
        """Setup Chrome webdriver for web automation."""
        chrome_options = Options()
        if headless:
            chrome_options.add_argument("--headless")
        chrome_options.add_argument("--no-sandbox")
        chrome_options.add_argument("--disable-dev-shm-usage")
        chrome_options.add_argument("--window-size=1920,1080")
        
        service = Service(ChromeDriverManager().install())
        driver = webdriver.Chrome(service=service, options=chrome_options)
        self.driver = driver
        return driver
    
    def scan_website(self, url: str, output_path: str = "chess_analysis.png") -> bool:
        """
        Scan a chess website and provide move suggestions.
        
        Args:
            url: URL of the chess website
            output_path: Path to save the analysis image
            
        Returns:
            True if successful, False otherwise
        """
        try:
            logging.info(f"Scanning chess website: {url}")
            
            # Setup webdriver if not already done
            if not self.driver:
                self.setup_webdriver()
            
            # Navigate to the website
            self.driver.get(url)
            time.sleep(3)  # Wait for page to load
            
            # Take screenshot
            screenshot = self.driver.get_screenshot_as_png()
            image = cv2.imdecode(np.frombuffer(screenshot, np.uint8), cv2.IMREAD_COLOR)
            
            # Detect chess boards
            board_regions = self.detector.detect_chessboard(image)
            
            if not board_regions:
                logging.warning("No chess boards detected on the webpage")
                return False
            
            # Use the best detected board
            best_board = board_regions[0]
            logging.info(f"Detected chess board with confidence: {best_board.confidence:.2f}")
            
            # Extract board region
            board_image = image[best_board.y:best_board.y + best_board.height,
                              best_board.x:best_board.x + best_board.width]
            
            # Recognize position
            fen = self.recognizer.recognize_position(board_image)
            logging.info(f"Recognized position: {fen}")
            
            # Analyze position
            moves = self.analyzer.analyze_position(fen)
            
            if moves:
                logging.info(f"Found {len(moves)} move suggestions:")
                for i, move in enumerate(moves):
                    logging.info(f"  {i+1}. {move.move} ({move.quality.value}) - Eval: {move.evaluation:.2f}")
            
            # Create overlay
            result_image = self.overlay.create_overlay(image, best_board, moves, fen)
            
            # Save result
            cv2.imwrite(output_path, result_image)
            logging.info(f"Analysis saved to: {output_path}")
            
            # Display result
            self.display_result(result_image, moves)
            
            return True
            
        except Exception as e:
            logging.error(f"Error scanning website: {e}")
            return False
    
    def scan_image(self, image_path: str, output_path: str = "chess_analysis.png") -> bool:
        """
        Scan a chess board image file and provide move suggestions.
        
        Args:
            image_path: Path to the chess board image
            output_path: Path to save the analysis image
            
        Returns:
            True if successful, False otherwise
        """
        try:
            logging.info(f"Scanning chess image: {image_path}")
            
            # Load image
            image = cv2.imread(image_path)
            if image is None:
                logging.error(f"Could not load image: {image_path}")
                return False
            
            # Detect chess boards
            board_regions = self.detector.detect_chessboard(image)
            
            if not board_regions:
                logging.warning("No chess boards detected in the image")
                return False
            
            # Use the best detected board
            best_board = board_regions[0]
            logging.info(f"Detected chess board with confidence: {best_board.confidence:.2f}")
            
            # Extract board region
            board_image = image[best_board.y:best_board.y + best_board.height,
                              best_board.x:best_board.x + best_board.width]
            
            # Recognize position
            fen = self.recognizer.recognize_position(board_image)
            logging.info(f"Recognized position: {fen}")
            
            # Analyze position
            moves = self.analyzer.analyze_position(fen)
            
            if moves:
                logging.info(f"Found {len(moves)} move suggestions:")
                for i, move in enumerate(moves):
                    logging.info(f"  {i+1}. {move.move} ({move.quality.value}) - Eval: {move.evaluation:.2f}")
            
            # Create overlay
            result_image = self.overlay.create_overlay(image, best_board, moves, fen)
            
            # Save result
            cv2.imwrite(output_path, result_image)
            logging.info(f"Analysis saved to: {output_path}")
            
            # Display result
            self.display_result(result_image, moves)
            
            return True
            
        except Exception as e:
            logging.error(f"Error scanning image: {e}")
            return False
    
    def display_result(self, image: np.ndarray, moves: List[ChessMove]):
        """Display the analysis result."""
        # Convert BGR to RGB for matplotlib
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        
        plt.figure(figsize=(12, 8))
        plt.imshow(image_rgb)
        plt.title("Chess Board Analysis")
        plt.axis('off')
        
        # Add move suggestions as text
        if moves:
            move_text = "Move Suggestions:\n"
            for i, move in enumerate(moves[:5]):
                move_text += f"{i+1}. {move.move} ({move.quality.value}) - Eval: {move.evaluation:.2f}\n"
            
            plt.figtext(0.02, 0.02, move_text, fontsize=10, 
                       bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
        
        plt.tight_layout()
        plt.show()
    
    def close(self):
        """Clean up resources."""
        if self.driver:
            self.driver.quit()
        self.analyzer.close()


def main():
    """Main entry point for the chess scanner."""
    parser = argparse.ArgumentParser(description="Chess Board Scanner and AI Move Suggester")
    parser.add_argument("--url", type=str, help="URL of chess website to scan")
    parser.add_argument("--image", type=str, help="Path to chess board image file")
    parser.add_argument("--output", type=str, default="chess_analysis.png", 
                       help="Output path for analysis image")
    parser.add_argument("--stockfish", type=str, help="Path to Stockfish executable")
    parser.add_argument("--headless", action="store_true", help="Run browser in headless mode")
    
    args = parser.parse_args()
    
    if not args.url and not args.image:
        print("Please provide either --url or --image parameter")
        parser.print_help()
        return
    
    try:
        # Initialize chess scanner
        scanner = ChessScanner(stockfish_path=args.stockfish)
        
        if args.url:
            # Scan website
            success = scanner.scan_website(args.url, args.output)
        else:
            # Scan image file
            success = scanner.scan_image(args.image, args.output)
        
        if success:
            print(f"Analysis completed successfully! Results saved to: {args.output}")
        else:
            print("Analysis failed. Check the logs for more details.")
        
    except KeyboardInterrupt:
        print("\nScan interrupted by user.")
    except Exception as e:
        print(f"Error: {e}")
        logging.error(f"Unexpected error: {e}")
    finally:
        # Clean up
        try:
            scanner.close()
        except:
            pass


if __name__ == "__main__":
    main()