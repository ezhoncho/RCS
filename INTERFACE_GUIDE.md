# ASCII Mining Game - Visual Interface Guide

This document shows what the game interface will look like when running in Roblox.

## Main Game Interface

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║ === RESOURCES ===     ┌─────────────┐                                        ║
║ Iron: 145            │   DEPTH: 12m │                                        ║
║ Gold: 23             └─────────────┘                                        ║
║ Rubble: 89                                                                   ║
║ Coal: 67                        ┌─────────────────┐                         ║
║ Steel: 8                        │      /|\       │                         ║
║                                 │     | + |      │                         ║
║                                 │      \|/       │                         ║
║                                 │                 │                         ║
║                                 │     #####       │                         ║
║                                 │    #######      │                         ║
║                                 │     #####       │                         ║
║                                 └─────────────────┘                         ║
║                                                                              ║
║ ┌──────────────────────────────────────────────────────────────────────────┐ ║
║ │ COMMANDS: [SPACE] Mine  [S] Store  [B] Build  [D] Drill Types           │ ║
║ │ Mining complete! Press SPACE to mine again.                             │ ║
║ └──────────────────────────────────────────────────────────────────────────┘ ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

## Store Menu Pop-up

```
                    ┌─────────────────────────────────┐
                    │        === STORE ===            │
                    ├─────────────────────────────────┤
                    │ Auto-mine (50 Iron, 20 Coal)   │
                    │ - BUY                           │
                    ├─────────────────────────────────┤
                    │ Better Drill (100 Iron, 50 Gold)│
                    │ - BUY                           │
                    ├─────────────────────────────────┤
                    │ Descend (10 Rubble)             │
                    │ - Always Available              │
                    ├─────────────────────────────────┤
                    │                              [X]│
                    └─────────────────────────────────┘
```

## Build Menu Pop-up

```
                    ┌─────────────────────────────────┐
                    │        === BUILD ===            │
                    ├─────────────────────────────────┤
                    │ Mining Cart (75 Iron, 25 Coal) │
                    │ - BUILD                         │
                    ├─────────────────────────────────┤
                    │ Smelter (100 Iron, 50 Coal,    │
                    │ 25 Rubble) - BUILD              │
                    ├─────────────────────────────────┤
                    │                              [X]│
                    └─────────────────────────────────┘
```

## Drill Types Menu Pop-up

```
                    ┌─────────────────────────────────┐
                    │     === DRILL TYPES ===        │
                    ├─────────────────────────────────┤
                    │ Regular Drill - EQUIPPED        │
                    ├─────────────────────────────────┤
                    │ Steel Drill - EQUIP             │
                    ├─────────────────────────────────┤
                    │                              [X]│
                    └─────────────────────────────────┘
```

## Visual Features

### Color Scheme
- **Background**: Pure black (RGB: 0, 0, 0) - Classic terminal look
- **Text**: Bright green (RGB: 0, 255, 0) - Matrix/terminal style
- **Accents**: Yellow (RGB: 255, 255, 0) - Important information
- **Warnings/Errors**: Red (RGB: 255, 0, 0) - Error messages
- **Frame Borders**: Dark grey (RGB: 20, 20, 20) - Subtle frames

### ASCII Art Elements

#### Mining Drill
```
  /|\  
 | + | 
  \|/  
```

#### Rock Formation
```
 ##### 
#######
 ##### 
```

#### Mining Cart (when built)
```
 [___] 
 O   O 
```

#### Smelter (when built)
```
 |^^^| 
 |ooo| 
 |___| 
```

### Interactive Elements

1. **Hover Effects**: Menu items change color when mouse hovers over them
   - Background becomes green, text becomes black

2. **Status Updates**: Bottom status bar shows real-time information:
   - "Mining..." (while mining operation is active)
   - "Auto-mine purchased!" (after buying upgrade)
   - "Smelter produced Steel! Temp: 1087°C" (smelter operation)

3. **Dynamic Resource Display**: Numbers update in real-time as resources are gained

4. **Pop-up Menus**: Center-screen overlays with ASCII-style borders

## Gameplay Flow Visual

### Initial State
```
Resources: All 0
Depth: 0m
Status: "Welcome to ASCII Mining! Press SPACE to start mining."
```

### After First Mining
```
Resources: Rubble +2
Depth: 0m (may increase to 1m)
Status: "Mining complete! Press SPACE to mine again."
```

### With Buildings Built
When smelter is built, the mining area might show:
```
      /|\       |^^^|
     | + |      |ooo|  <- Smelter with temperature indicator
      \|/       |___|

     #####
    #######
     #####
```

### Auto-mine Active
Status rotates between:
- "Auto-mining in progress..."
- "Mining complete! Auto-mine will continue."

This interface maintains the authentic ASCII/terminal aesthetic while providing all the functionality described in the requirements. The black background with green text creates the classic "hacker terminal" feel that makes the game distinctive and engaging.