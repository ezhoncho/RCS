#!/usr/bin/env python3
"""
Create a sample chess board image for testing the chess scanner
"""

import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont
import chess

def create_chess_board_image(size=640, output_path="sample_chess_board.png"):
    """Create a sample chess board image with pieces."""
    
    # Create a board with alternating colors
    square_size = size // 8
    board_image = np.zeros((size, size, 3), dtype=np.uint8)
    
    # Define colors (light and dark squares)
    light_color = (240, 217, 181)  # Light brown
    dark_color = (181, 136, 99)    # Dark brown
    
    # Fill squares
    for row in range(8):
        for col in range(8):
            color = light_color if (row + col) % 2 == 0 else dark_color
            y1, y2 = row * square_size, (row + 1) * square_size
            x1, x2 = col * square_size, (col + 1) * square_size
            board_image[y1:y2, x1:x2] = color
    
    # Add grid lines for clarity
    for i in range(9):
        # Horizontal lines
        cv2.line(board_image, (0, i * square_size), (size, i * square_size), (50, 50, 50), 2)
        # Vertical lines  
        cv2.line(board_image, (i * square_size, 0), (i * square_size, size), (50, 50, 50), 2)
    
    # Add some pieces using simple circles/rectangles as placeholders
    # This is a simplified representation - in reality you'd use piece images
    
    # Starting position pieces (simplified representation)
    piece_positions = {
        # White pieces (bottom)
        (7, 0): 'R', (7, 1): 'N', (7, 2): 'B', (7, 3): 'Q', 
        (7, 4): 'K', (7, 5): 'B', (7, 6): 'N', (7, 7): 'R',
        # White pawns
        (6, 0): 'P', (6, 1): 'P', (6, 2): 'P', (6, 3): 'P',
        (6, 4): 'P', (6, 5): 'P', (6, 6): 'P', (6, 7): 'P',
        
        # Black pieces (top)
        (0, 0): 'r', (0, 1): 'n', (0, 2): 'b', (0, 3): 'q',
        (0, 4): 'k', (0, 5): 'b', (0, 6): 'n', (0, 7): 'r',
        # Black pawns
        (1, 0): 'p', (1, 1): 'p', (1, 2): 'p', (1, 3): 'p',
        (1, 4): 'p', (1, 5): 'p', (1, 6): 'p', (1, 7): 'p',
        
        # Add a few pieces in middle game position
        (4, 4): 'P',  # Advanced pawn
        (3, 3): 'n',  # Knight
    }
    
    # Draw pieces
    for (row, col), piece in piece_positions.items():
        center_x = col * square_size + square_size // 2
        center_y = row * square_size + square_size // 2
        
        # White pieces (uppercase) = white color, Black pieces (lowercase) = black color
        color = (255, 255, 255) if piece.isupper() else (0, 0, 0)
        border_color = (0, 0, 0) if piece.isupper() else (255, 255, 255)
        
        # Different shapes for different pieces
        if piece.lower() in ['k', 'q']:  # King, Queen - large circle
            cv2.circle(board_image, (center_x, center_y), square_size//3, color, -1)
            cv2.circle(board_image, (center_x, center_y), square_size//3, border_color, 2)
            # Add a small crown-like shape for kings
            if piece.lower() == 'k':
                cv2.rectangle(board_image, 
                            (center_x - square_size//6, center_y - square_size//3),
                            (center_x + square_size//6, center_y - square_size//4),
                            border_color, 2)
        
        elif piece.lower() in ['r']:  # Rook - rectangle
            cv2.rectangle(board_image,
                        (center_x - square_size//4, center_y - square_size//3),
                        (center_x + square_size//4, center_y + square_size//3),
                        color, -1)
            cv2.rectangle(board_image,
                        (center_x - square_size//4, center_y - square_size//3),
                        (center_x + square_size//4, center_y + square_size//3),
                        border_color, 2)
        
        elif piece.lower() in ['b']:  # Bishop - diamond
            points = np.array([
                [center_x, center_y - square_size//3],
                [center_x + square_size//4, center_y],
                [center_x, center_y + square_size//3],
                [center_x - square_size//4, center_y]
            ])
            cv2.fillPoly(board_image, [points], color)
            cv2.polylines(board_image, [points], True, border_color, 2)
        
        elif piece.lower() in ['n']:  # Knight - L shape
            points = np.array([
                [center_x - square_size//4, center_y + square_size//3],
                [center_x - square_size//4, center_y],
                [center_x, center_y - square_size//3],
                [center_x + square_size//4, center_y - square_size//3],
                [center_x + square_size//4, center_y + square_size//3]
            ])
            cv2.fillPoly(board_image, [points], color)
            cv2.polylines(board_image, [points], True, border_color, 2)
        
        else:  # Pawn - small circle
            cv2.circle(board_image, (center_x, center_y), square_size//5, color, -1)
            cv2.circle(board_image, (center_x, center_y), square_size//5, border_color, 2)
    
    # Add border around the entire board
    cv2.rectangle(board_image, (0, 0), (size-1, size-1), (100, 100, 100), 4)
    
    # Add coordinates (files a-h, ranks 1-8)
    font = cv2.FONT_HERSHEY_SIMPLEX
    font_size = 0.6
    font_color = (100, 100, 100)
    
    # Files (a-h)
    for i, file_letter in enumerate('abcdefgh'):
        x = i * square_size + square_size // 2 - 5
        cv2.putText(board_image, file_letter, (x, size + 20), font, font_size, font_color, 2)
    
    # Ranks (1-8) 
    for i, rank_number in enumerate('87654321'):
        y = i * square_size + square_size // 2 + 5
        cv2.putText(board_image, rank_number, (-20, y), font, font_size, font_color, 2)
    
    # Extend image to include coordinate labels
    extended_size = size + 40
    extended_image = np.ones((extended_size, extended_size, 3), dtype=np.uint8) * 255
    extended_image[20:size+20, 20:size+20] = board_image
    
    # Save the image
    cv2.imwrite(output_path, extended_image)
    print(f"Sample chess board image created: {output_path}")
    
    return extended_image

def create_chess_website_mockup(output_path="sample_chess_website.png"):
    """Create a mockup of a chess website with a board."""
    
    # Create a larger image simulating a website
    width, height = 1200, 800
    website_image = np.ones((height, width, 3), dtype=np.uint8) * 255
    
    # Add a header bar
    cv2.rectangle(website_image, (0, 0), (width, 80), (50, 50, 50), -1)
    cv2.putText(website_image, "Chess.com - Play Chess Online", (20, 50), 
                cv2.FONT_HERSHEY_SIMPLEX, 1.5, (255, 255, 255), 2)
    
    # Add sidebar areas
    cv2.rectangle(website_image, (0, 80), (200, height), (240, 240, 240), -1)
    cv2.rectangle(website_image, (1000, 80), (width, height), (240, 240, 240), -1)
    
    # Create the chess board in the center
    board_size = 480
    board_x = (width - board_size) // 2
    board_y = (height - board_size) // 2
    
    board_image = create_chess_board_image(board_size, "temp_board.png")
    # Remove the coordinate extension for the website mockup
    board_clean = board_image[20:board_size+20, 20:board_size+20]
    
    # Place the board on the website
    website_image[board_y:board_y+board_size, board_x:board_x+board_size] = board_clean
    
    # Add some UI elements around the board
    # Player names
    cv2.rectangle(website_image, (board_x, board_y-60), (board_x+board_size, board_y-20), (200, 200, 200), -1)
    cv2.putText(website_image, "Black Player (1800)", (board_x+10, board_y-35), 
                cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 0), 2)
    
    cv2.rectangle(website_image, (board_x, board_y+board_size+20), (board_x+board_size, board_y+board_size+60), (200, 200, 200), -1)
    cv2.putText(website_image, "White Player (1750)", (board_x+10, board_y+board_size+45), 
                cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 0), 2)
    
    # Side panel content
    cv2.putText(website_image, "Analysis", (20, 120), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
    cv2.putText(website_image, "Move List", (20, 160), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (100, 100, 100), 1)
    cv2.putText(website_image, "1. e4 e5", (20, 180), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (100, 100, 100), 1)
    cv2.putText(website_image, "2. Nf3 Nc6", (20, 200), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (100, 100, 100), 1)
    cv2.putText(website_image, "3. Bb5 ...", (20, 220), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (100, 100, 100), 1)
    
    # Right panel
    cv2.putText(website_image, "Chat", (1020, 120), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
    
    # Save the website mockup
    cv2.imwrite(output_path, website_image)
    print(f"Sample chess website mockup created: {output_path}")
    
    return website_image

if __name__ == "__main__":
    print("Creating sample chess images for testing...")
    
    # Create a simple chess board
    create_chess_board_image(output_path="sample_chess_board.png")
    
    # Create a website mockup
    create_chess_website_mockup(output_path="sample_chess_website.png")
    
    print("Sample images created successfully!")
    print("You can now test the chess scanner with:")
    print("  python chess_scanner.py --image sample_chess_board.png")
    print("  python chess_scanner.py --image sample_chess_website.png")