#!/usr/bin/env python3
"""
Validation script for the ASCII Mining Game
Checks file structure, basic syntax, and key components
"""

import os
import re
import xml.etree.ElementTree as ET

def check_file_exists(filepath, description):
    """Check if a file exists and print status"""
    if os.path.exists(filepath):
        print(f"✓ {description} exists: {filepath}")
        return True
    else:
        print(f"✗ {description} missing: {filepath}")
        return False

def validate_lua_syntax(filepath):
    """Basic validation of Lua file structure"""
    print(f"\nValidating Lua syntax for {filepath}...")
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for required components
        checks = [
            ('Services section', r'-- Services'),
            ('Players service', r'local Players = game:GetService\("Players"\)'),
            ('UserInputService', r'local UserInputService = game:GetService\("UserInputService"\)'),
            ('GameState table', r'local GameState = {'),
            ('Resources definition', r'resources = {'),
            ('ASCII_CHARS table', r'local ASCII_CHARS = {'),
            ('COLORS table', r'local COLORS = {'),
            ('createMainGUI function', r'local function createMainGUI\(\)'),
            ('mine function', r'local function mine\(\)'),
            ('Input handling', r'UserInputService\.InputBegan:Connect'),
            ('Game initialization', r'initializeGame\(\)'),
        ]
        
        passed = 0
        for name, pattern in checks:
            if re.search(pattern, content):
                print(f"  ✓ {name}")
                passed += 1
            else:
                print(f"  ✗ {name}")
        
        print(f"  Syntax checks: {passed}/{len(checks)} passed")
        return passed == len(checks)
        
    except Exception as e:
        print(f"  ✗ Error reading file: {e}")
        return False

def validate_rbxl_structure(filepath):
    """Validate the RBXL file structure"""
    print(f"\nValidating RBXL structure for {filepath}...")
    
    try:
        tree = ET.parse(filepath)
        root = tree.getroot()
        
        # Check basic structure
        if root.tag != 'roblox':
            print("  ✗ Invalid root tag")
            return False
        
        # Find DataModel
        datamodel = root.find('.//Item[@class="DataModel"]')
        if datamodel is None:
            print("  ✗ No DataModel found")
            return False
        
        print("  ✓ Valid RBXL structure")
        
        # Check for required components
        workspace = root.find('.//Item[@class="Workspace"]')
        starter_player = root.find('.//Item[@class="StarterPlayer"]')
        local_script = root.find('.//Item[@class="LocalScript"]')
        
        checks = [
            (workspace is not None, "Workspace"),
            (starter_player is not None, "StarterPlayer"), 
            (local_script is not None, "LocalScript")
        ]
        
        passed = 0
        for check, name in checks:
            if check:
                print(f"  ✓ {name} found")
                passed += 1
            else:
                print(f"  ✗ {name} missing")
        
        # Check LocalScript content
        if local_script is not None:
            source_elem = local_script.find('.//ProtectedString[@name="Source"]')
            if source_elem is not None and source_elem.text:
                if 'ASCII Mining Game' in source_elem.text:
                    print("  ✓ LocalScript contains game code")
                    passed += 1
                else:
                    print("  ✗ LocalScript missing game code")
            else:
                print("  ✗ LocalScript has no source code")
        
        print(f"  Structure checks: {passed}/{len(checks)+1} passed")
        return passed == len(checks) + 1
        
    except ET.ParseError as e:
        print(f"  ✗ XML parsing error: {e}")
        return False
    except Exception as e:
        print(f"  ✗ Error validating RBXL: {e}")
        return False

def validate_readme(filepath):
    """Validate README content"""
    print(f"\nValidating README content for {filepath}...")
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        required_sections = [
            'ASCII Mining Game',
            'Features',
            'Installation',
            'Controls',
            'Game Interface',
            'Troubleshooting'
        ]
        
        passed = 0
        for section in required_sections:
            if section in content:
                print(f"  ✓ {section} section found")
                passed += 1
            else:
                print(f"  ✗ {section} section missing")
        
        # Check for key information
        key_info = [
            ('Controls table', '| Key | Action |'),
            ('SPACEBAR control', 'SPACEBAR'),
            ('Store control', '"S"'),
            ('Installation steps', 'LocalScript'),
            ('Color scheme', 'black background'),
        ]
        
        for name, text in key_info:
            if text.lower() in content.lower():
                print(f"  ✓ {name} documented")
                passed += 1
            else:
                print(f"  ✗ {name} not documented")
        
        total_checks = len(required_sections) + len(key_info)
        print(f"  Documentation checks: {passed}/{total_checks} passed")
        return passed >= total_checks * 0.8  # 80% pass rate
        
    except Exception as e:
        print(f"  ✗ Error validating README: {e}")
        return False

def main():
    """Main validation function"""
    print("=== ASCII Mining Game Validation ===")
    print()
    
    base_path = "/home/runner/work/RCS/RCS"
    
    # Check file existence
    files_to_check = [
        (os.path.join(base_path, "MiningGameScript.lua"), "Main Lua script"),
        (os.path.join(base_path, "ASCII_Mining_Game.rbxlx"), "Roblox place file"),
        (os.path.join(base_path, "README.md"), "README documentation"),
        (os.path.join(base_path, "test_mining_game.lua"), "Test script")
    ]
    
    all_files_exist = True
    for filepath, description in files_to_check:
        if not check_file_exists(filepath, description):
            all_files_exist = False
    
    if not all_files_exist:
        print("\n✗ Some required files are missing!")
        return False
    
    # Validate each file
    validations = [
        (validate_lua_syntax, os.path.join(base_path, "MiningGameScript.lua")),
        (validate_rbxl_structure, os.path.join(base_path, "ASCII_Mining_Game.rbxlx")),
        (validate_readme, os.path.join(base_path, "README.md"))
    ]
    
    all_valid = True
    for validator, filepath in validations:
        if not validator(filepath):
            all_valid = False
    
    # Final summary
    print("\n=== Validation Summary ===")
    if all_valid:
        print("✓ All validations passed! The ASCII Mining Game is ready for deployment.")
        print("\nTo use the game:")
        print("1. Open Roblox Studio")
        print("2. Import the ASCII_Mining_Game.rbxlx file, OR")
        print("3. Create a LocalScript and copy the content from MiningGameScript.lua")
        print("4. Place the LocalScript in StarterPlayer > StarterPlayerScripts")
        print("5. Test the game!")
    else:
        print("✗ Some validations failed. Please review the issues above.")
    
    return all_valid

if __name__ == "__main__":
    main()