# ASCII Mining Game - Roblox Implementation

A 2D mining game for Roblox that replicates the ASCII-style terminal aesthetic of classic mining games.

## Features

### Core Gameplay
- **Mining Mechanics**: Press SPACEBAR to mine resources
- **Resource System**: Collect Iron, Gold, Rubble, Coal, and Steel
- **Depth Progression**: Mine deeper to find better resources
- **ASCII-Style Interface**: Terminal-inspired black background with green text

### Resource Management
- **Iron**: Common metal used for most purchases
- **Gold**: Rare metal found at deeper levels
- **Rubble**: Basic material from mining
- **Coal**: Fuel for smelting and buildings
- **Steel**: Produced by smelting Iron and Coal

### Store System (Press 'S')
- **Auto-mine**: Automatically mines every 3 seconds (50 Iron, 20 Coal)
- **Better Drill**: Improved mining efficiency (100 Iron, 50 Gold)
- **Descend**: Instantly go 5 meters deeper (10 Rubble)

### Building System (Press 'B')
- **Mining Cart**: Improves resource collection (75 Iron, 25 Coal)
- **Smelter**: Converts Iron + Coal into Steel every 5 seconds (100 Iron, 50 Coal, 25 Rubble)

### Drill Types (Press 'D')
- **Regular Drill**: Standard mining tool
- **Steel Drill**: Requires Steel to use, more effective

## Installation

1. **Create a new Roblox Studio place** or open an existing one
2. **Insert the script**:
   - In Roblox Studio, go to ServerStorage or StarterPlayer > StarterPlayerScripts
   - Create a new **LocalScript** (important: must be LocalScript, not Script)
   - Copy the entire contents of `MiningGameScript.lua` into the LocalScript
   - Rename the LocalScript to "MiningGame" or similar

3. **Test the game**:
   - Click "Play" in Roblox Studio
   - The ASCII mining interface should appear immediately
   - Use the controls listed below to play

## Controls

| Key | Action |
|-----|---------|
| **SPACEBAR** | Mine resources |
| **S** | Open Store menu |
| **B** | Open Build menu |
| **D** | Open Drill Types menu |

## Game Interface

### Main Screen Layout
```
=== RESOURCES ===     DEPTH: 0m
Iron: 0
Gold: 0               [Mining Area]
Rubble: 0              /|\
Coal: 0               | + |
Steel: 0               \|/

                     #####
                    #######
                     #####

COMMANDS: [SPACE] Mine [S] Store [B] Build [D] Drill Types
Ready to mine...
```

### Pop-up Menus
All menus appear as centered pop-ups with ASCII-style borders:
- Green text on black background
- Yellow accents for titles and important information
- Hover effects for interactive elements

## Game Strategy

1. **Start Mining**: Press SPACEBAR repeatedly to collect initial resources
2. **Build Infrastructure**: Use Iron and Coal to build a Smelter for Steel production
3. **Upgrade Equipment**: Purchase Better Drill and Auto-mine from the Store
4. **Go Deeper**: Use Rubble to descend and find rare resources like Gold
5. **Expand Operations**: Build Mining Cart for better collection efficiency

## Technical Details

### Roblox Services Used
- `Players` - For player and GUI management
- `UserInputService` - For keyboard input handling
- `TweenService` - For smooth UI animations
- `RunService` - For game loop management

### Color Scheme
- **Background**: Pure black (0, 0, 0)
- **Text**: Bright green (0, 255, 0)
- **Accents**: Yellow (255, 255, 0)
- **Warnings**: Red (255, 0, 0)
- **Frames**: Dark grey (20, 20, 20)

## Customization

The game is designed to be easily customizable:

### Adding New Resources
1. Add the resource to `GameState.resources`
2. Update the resource display creation in `createResourceDisplay()`
3. Modify mining logic in `mine()` function

### Adding New Buildings
1. Add building to `GameState.buildings`
2. Create new menu items in `showBuild()`
3. Implement building functionality

### Modifying ASCII Art
Edit the `ASCII_CHARS` table to change visual elements:
```lua
ASCII_CHARS = {
    drill = {"  /|\\  ", " | + | ", "  \\|/  "},
    rock = {" ##### ", "#######", " ##### "}
    -- Add more ASCII art here
}
```

## Troubleshooting

### Script Not Working
- Ensure it's a **LocalScript**, not a regular Script
- Place it in `StarterPlayer > StarterPlayerScripts` for automatic loading
- Check the Output window for any error messages

### UI Not Appearing
- Make sure `ResetOnSpawn` is set to false in the ScreenGui
- Verify the script is running on the client side (LocalScript)

### Controls Not Responding
- Check that `UserInputService` is properly connected
- Ensure no other scripts are intercepting input events

## Future Enhancements

Potential features that could be added:
- Sound effects for mining and building
- Particle effects for mining operations
- More complex building chains
- Multiplayer mining competition
- Save/load functionality using DataStore
- Achievement system
- Dynamic ASCII animations