--[[
    ASCII Mining Game - Roblox Implementation
    A 2D mining game with terminal-style aesthetics
--]]

-- Services
local Players = game:GetService("Players")
local UserInputService = game:GetService("UserInputService")
local TweenService = game:GetService("TweenService")
local RunService = game:GetService("RunService")

-- Player and GUI references
local player = Players.LocalPlayer
local playerGui = player:WaitForChild("PlayerGui")

-- Game state
local GameState = {
    resources = {
        iron = 0,
        gold = 0,
        rubble = 0,
        coal = 0,
        steel = 0
    },
    depth = 0,
    drillType = "Regular", -- Regular or Steel
    buildings = {
        miningCart = false,
        smelter = false,
        smelterTemp = 0
    },
    upgrades = {
        autoMine = false,
        betterDrill = false
    },
    mining = false
}

-- UI Elements
local mainGui
local resourceFrame
local depthLabel
local miningArea
local commandMenu
local currentPopup = nil

-- ASCII Art and Styling
local ASCII_CHARS = {
    drill = {
        "  /|\\  ",
        " | + | ",
        "  \\|/  "
    },
    rock = {
        " ##### ",
        "#######",
        " ##### "
    },
    miningCart = {
        " [___] ",
        " O   O "
    },
    smelter = {
        " |^^^| ",
        " |ooo| ",
        " |___| "
    }
}

-- Color scheme for ASCII aesthetic
local COLORS = {
    background = Color3.fromRGB(0, 0, 0),        -- Black background
    text = Color3.fromRGB(0, 255, 0),           -- Green text
    accent = Color3.fromRGB(255, 255, 0),       -- Yellow accents
    warning = Color3.fromRGB(255, 0, 0),        -- Red for warnings
    frame = Color3.fromRGB(20, 20, 20)          -- Dark grey for frames
}

-- Initialize the main GUI
local function createMainGUI()
    -- Create main ScreenGui
    mainGui = Instance.new("ScreenGui")
    mainGui.Name = "MiningGameGUI"
    mainGui.ResetOnSpawn = false
    mainGui.Parent = playerGui
    
    -- Main container frame
    local mainFrame = Instance.new("Frame")
    mainFrame.Name = "MainFrame"
    mainFrame.Size = UDim2.new(1, 0, 1, 0)
    mainFrame.Position = UDim2.new(0, 0, 0, 0)
    mainFrame.BackgroundColor3 = COLORS.background
    mainFrame.BorderSizePixel = 0
    mainFrame.Parent = mainGui
    
    return mainFrame
end

-- Create resource display
local function createResourceDisplay(parent)
    local resourceFrame = Instance.new("Frame")
    resourceFrame.Name = "ResourceFrame"
    resourceFrame.Size = UDim2.new(0, 300, 0, 150)
    resourceFrame.Position = UDim2.new(0, 10, 0, 10)
    resourceFrame.BackgroundColor3 = COLORS.frame
    resourceFrame.BorderColor3 = COLORS.text
    resourceFrame.BorderSizePixel = 2
    resourceFrame.Parent = parent
    
    -- Title
    local title = Instance.new("TextLabel")
    title.Name = "Title"
    title.Size = UDim2.new(1, 0, 0, 25)
    title.Position = UDim2.new(0, 0, 0, 0)
    title.BackgroundTransparency = 1
    title.Text = "=== RESOURCES ==="
    title.TextColor3 = COLORS.text
    title.TextSize = 14
    title.Font = Enum.Font.Code
    title.Parent = resourceFrame
    
    -- Resource labels
    local resources = {"Iron", "Gold", "Rubble", "Coal", "Steel"}
    for i, resource in ipairs(resources) do
        local label = Instance.new("TextLabel")
        label.Name = resource .. "Label"
        label.Size = UDim2.new(1, -10, 0, 20)
        label.Position = UDim2.new(0, 5, 0, 20 + (i * 22))
        label.BackgroundTransparency = 1
        label.Text = resource .. ": 0"
        label.TextColor3 = COLORS.text
        label.TextSize = 12
        label.Font = Enum.Font.Code
        label.TextXAlignment = Enum.TextXAlignment.Left
        label.Parent = resourceFrame
    end
    
    return resourceFrame
end

-- Create depth display
local function createDepthDisplay(parent)
    local depthFrame = Instance.new("Frame")
    depthFrame.Name = "DepthFrame"
    depthFrame.Size = UDim2.new(0, 200, 0, 60)
    depthFrame.Position = UDim2.new(0.5, -100, 0, 10)
    depthFrame.BackgroundColor3 = COLORS.frame
    depthFrame.BorderColor3 = COLORS.text
    depthFrame.BorderSizePixel = 2
    depthFrame.Parent = parent
    
    local depthLabel = Instance.new("TextLabel")
    depthLabel.Name = "DepthLabel"
    depthLabel.Size = UDim2.new(1, 0, 1, 0)
    depthLabel.Position = UDim2.new(0, 0, 0, 0)
    depthLabel.BackgroundTransparency = 1
    depthLabel.Text = "DEPTH: 0m"
    depthLabel.TextColor3 = COLORS.accent
    depthLabel.TextSize = 16
    depthLabel.Font = Enum.Font.Code
    depthLabel.Parent = depthFrame
    
    return depthLabel
end

-- Create mining area
local function createMiningArea(parent)
    local miningFrame = Instance.new("Frame")
    miningFrame.Name = "MiningFrame"
    miningFrame.Size = UDim2.new(0, 400, 0, 300)
    miningFrame.Position = UDim2.new(0.5, -200, 0.5, -150)
    miningFrame.BackgroundColor3 = COLORS.frame
    miningFrame.BorderColor3 = COLORS.text
    miningFrame.BorderSizePixel = 2
    miningFrame.Parent = parent
    
    -- Mining display area
    local miningDisplay = Instance.new("TextLabel")
    miningDisplay.Name = "MiningDisplay"
    miningDisplay.Size = UDim2.new(1, -10, 1, -10)
    miningDisplay.Position = UDim2.new(0, 5, 0, 5)
    miningDisplay.BackgroundTransparency = 1
    miningDisplay.Text = table.concat(ASCII_CHARS.drill, "\n") .. "\n\n" .. table.concat(ASCII_CHARS.rock, "\n")
    miningDisplay.TextColor3 = COLORS.text
    miningDisplay.TextSize = 16
    miningDisplay.Font = Enum.Font.Code
    miningDisplay.Parent = miningFrame
    
    return miningFrame
end

-- Create command menu
local function createCommandMenu(parent)
    local commandFrame = Instance.new("Frame")
    commandFrame.Name = "CommandFrame"
    commandFrame.Size = UDim2.new(1, -20, 0, 80)
    commandFrame.Position = UDim2.new(0, 10, 1, -90)
    commandFrame.BackgroundColor3 = COLORS.frame
    commandFrame.BorderColor3 = COLORS.text
    commandFrame.BorderSizePixel = 2
    commandFrame.Parent = parent
    
    -- Command instructions
    local instructions = Instance.new("TextLabel")
    instructions.Name = "Instructions"
    instructions.Size = UDim2.new(1, -10, 0, 25)
    instructions.Position = UDim2.new(0, 5, 0, 5)
    instructions.BackgroundTransparency = 1
    instructions.Text = "COMMANDS: [SPACE] Mine  [S] Store  [B] Build  [D] Drill Types"
    instructions.TextColor3 = COLORS.text
    instructions.TextSize = 12
    instructions.Font = Enum.Font.Code
    instructions.TextXAlignment = Enum.TextXAlignment.Left
    instructions.Parent = commandFrame
    
    -- Status line
    local status = Instance.new("TextLabel")
    status.Name = "Status"
    status.Size = UDim2.new(1, -10, 0, 20)
    status.Position = UDim2.new(0, 5, 0, 30)
    status.BackgroundTransparency = 1
    status.Text = "Ready to mine..."
    status.TextColor3 = COLORS.accent
    status.TextSize = 11
    status.Font = Enum.Font.Code
    status.TextXAlignment = Enum.TextXAlignment.Left
    status.Parent = commandFrame
    
    return commandFrame
end

-- Create popup menu
local function createPopupMenu(title, items, callbacks)
    if currentPopup then
        currentPopup:Destroy()
    end
    
    local popup = Instance.new("Frame")
    popup.Name = "PopupMenu"
    popup.Size = UDim2.new(0, 350, 0, 50 + (#items * 35))
    popup.Position = UDim2.new(0.5, -175, 0.5, -(25 + (#items * 17.5)))
    popup.BackgroundColor3 = COLORS.frame
    popup.BorderColor3 = COLORS.accent
    popup.BorderSizePixel = 3
    popup.Parent = mainGui
    
    -- Title
    local titleLabel = Instance.new("TextLabel")
    titleLabel.Name = "Title"
    titleLabel.Size = UDim2.new(1, 0, 0, 30)
    titleLabel.Position = UDim2.new(0, 0, 0, 0)
    titleLabel.BackgroundTransparency = 1
    titleLabel.Text = "=== " .. title .. " ==="
    titleLabel.TextColor3 = COLORS.accent
    titleLabel.TextSize = 14
    titleLabel.Font = Enum.Font.Code
    titleLabel.Parent = popup
    
    -- Menu items
    for i, item in ipairs(items) do
        local button = Instance.new("TextButton")
        button.Name = "Item" .. i
        button.Size = UDim2.new(1, -10, 0, 30)
        button.Position = UDim2.new(0, 5, 0, 25 + (i * 35))
        button.BackgroundColor3 = COLORS.background
        button.BorderColor3 = COLORS.text
        button.BorderSizePixel = 1
        button.Text = item.text
        button.TextColor3 = COLORS.text
        button.TextSize = 12
        button.Font = Enum.Font.Code
        button.Parent = popup
        
        -- Hover effects
        button.MouseEnter:Connect(function()
            button.BackgroundColor3 = COLORS.text
            button.TextColor3 = COLORS.background
        end)
        
        button.MouseLeave:Connect(function()
            button.BackgroundColor3 = COLORS.background
            button.TextColor3 = COLORS.text
        end)
        
        -- Click handler
        button.MouseButton1Click:Connect(function()
            if callbacks[i] then
                callbacks[i]()
            end
            popup:Destroy()
            currentPopup = nil
        end)
    end
    
    -- Close button
    local closeButton = Instance.new("TextButton")
    closeButton.Name = "CloseButton"
    closeButton.Size = UDim2.new(0, 30, 0, 20)
    closeButton.Position = UDim2.new(1, -35, 0, 5)
    closeButton.BackgroundColor3 = COLORS.warning
    closeButton.BorderSizePixel = 0
    closeButton.Text = "X"
    closeButton.TextColor3 = COLORS.background
    closeButton.TextSize = 12
    closeButton.Font = Enum.Font.Code
    closeButton.Parent = popup
    
    closeButton.MouseButton1Click:Connect(function()
        popup:Destroy()
        currentPopup = nil
    end)
    
    currentPopup = popup
    return popup
end

-- Initialize the game
local function initializeGame()
    local mainFrame = createMainGUI()
    resourceFrame = createResourceDisplay(mainFrame)
    depthLabel = createDepthDisplay(mainFrame)
    miningArea = createMiningArea(mainFrame)
    commandMenu = createCommandMenu(mainFrame)
    
    -- Update displays
    updateResourceDisplay()
    updateDepthDisplay()
end

-- Update resource display
function updateResourceDisplay()
    if resourceFrame then
        for resource, amount in pairs(GameState.resources) do
            local label = resourceFrame:FindFirstChild(resource:sub(1,1):upper() .. resource:sub(2) .. "Label")
            if label then
                label.Text = resource:sub(1,1):upper() .. resource:sub(2) .. ": " .. amount
            end
        end
    end
end

-- Update depth display
function updateDepthDisplay()
    if depthLabel then
        depthLabel.Text = "DEPTH: " .. GameState.depth .. "m"
    end
end

-- Update status
function updateStatus(message)
    if commandMenu then
        local status = commandMenu:FindFirstChild("Status")
        if status then
            status.Text = message
        end
    end
end

-- Mining function
local function mine()
    if GameState.mining then return end
    
    GameState.mining = true
    updateStatus("Mining...")
    
    -- Simulate mining delay
    wait(1)
    
    -- Random resource generation based on depth
    local resourceGained = {}
    local baseChance = 0.7
    local depthMultiplier = 1 + (GameState.depth * 0.1)
    
    if math.random() < baseChance then
        resourceGained.rubble = math.random(1, 3) * depthMultiplier
    end
    
    if math.random() < 0.3 * depthMultiplier then
        resourceGained.iron = math.random(1, 2)
    end
    
    if math.random() < 0.15 * depthMultiplier then
        resourceGained.coal = math.random(1, 2)
    end
    
    if GameState.depth > 5 and math.random() < 0.1 * depthMultiplier then
        resourceGained.gold = 1
    end
    
    -- Add resources
    for resource, amount in pairs(resourceGained) do
        GameState.resources[resource] = GameState.resources[resource] + amount
    end
    
    -- Increase depth occasionally
    if math.random() < 0.3 then
        GameState.depth = GameState.depth + 1
    end
    
    -- Update displays
    updateResourceDisplay()
    updateDepthDisplay()
    
    GameState.mining = false
    updateStatus("Mining complete! Press SPACE to mine again.")
end

-- Store menu
local function showStore()
    local items = {
        {text = "Auto-mine (50 Iron, 20 Coal) - " .. (GameState.upgrades.autoMine and "OWNED" or "BUY")},
        {text = "Better Drill (100 Iron, 50 Gold) - " .. (GameState.upgrades.betterDrill and "OWNED" or "BUY")},
        {text = "Descend (10 Rubble) - Always Available"}
    }
    
    local callbacks = {
        function() -- Auto-mine
            if not GameState.upgrades.autoMine and GameState.resources.iron >= 50 and GameState.resources.coal >= 20 then
                GameState.resources.iron = GameState.resources.iron - 50
                GameState.resources.coal = GameState.resources.coal - 20
                GameState.upgrades.autoMine = true
                updateResourceDisplay()
                updateStatus("Auto-mine purchased!")
            elseif GameState.upgrades.autoMine then
                updateStatus("Already owned!")
            else
                updateStatus("Insufficient resources!")
            end
        end,
        function() -- Better Drill
            if not GameState.upgrades.betterDrill and GameState.resources.iron >= 100 and GameState.resources.gold >= 50 then
                GameState.resources.iron = GameState.resources.iron - 100
                GameState.resources.gold = GameState.resources.gold - 50
                GameState.upgrades.betterDrill = true
                updateResourceDisplay()
                updateStatus("Better Drill purchased!")
            elseif GameState.upgrades.betterDrill then
                updateStatus("Already owned!")
            else
                updateStatus("Insufficient resources!")
            end
        end,
        function() -- Descend
            if GameState.resources.rubble >= 10 then
                GameState.resources.rubble = GameState.resources.rubble - 10
                GameState.depth = GameState.depth + 5
                updateResourceDisplay()
                updateDepthDisplay()
                updateStatus("Descended 5 meters!")
            else
                updateStatus("Need 10 Rubble to descend!")
            end
        end
    }
    
    createPopupMenu("STORE", items, callbacks)
end

-- Build menu
local function showBuild()
    local items = {
        {text = "Mining Cart (75 Iron, 25 Coal) - " .. (GameState.buildings.miningCart and "BUILT" or "BUILD")},
        {text = "Smelter (100 Iron, 50 Coal, 25 Rubble) - " .. (GameState.buildings.smelter and "BUILT" or "BUILD")}
    }
    
    local callbacks = {
        function() -- Mining Cart
            if not GameState.buildings.miningCart and GameState.resources.iron >= 75 and GameState.resources.coal >= 25 then
                GameState.resources.iron = GameState.resources.iron - 75
                GameState.resources.coal = GameState.resources.coal - 25
                GameState.buildings.miningCart = true
                updateResourceDisplay()
                updateStatus("Mining Cart built!")
            elseif GameState.buildings.miningCart then
                updateStatus("Already built!")
            else
                updateStatus("Insufficient resources!")
            end
        end,
        function() -- Smelter
            if not GameState.buildings.smelter and GameState.resources.iron >= 100 and GameState.resources.coal >= 50 and GameState.resources.rubble >= 25 then
                GameState.resources.iron = GameState.resources.iron - 100
                GameState.resources.coal = GameState.resources.coal - 50
                GameState.resources.rubble = GameState.resources.rubble - 25
                GameState.buildings.smelter = true
                updateResourceDisplay()
                updateStatus("Smelter built!")
            elseif GameState.buildings.smelter then
                updateStatus("Already built!")
            else
                updateStatus("Insufficient resources!")
            end
        end
    }
    
    createPopupMenu("BUILD", items, callbacks)
end

-- Drill Types menu
local function showDrillTypes()
    local items = {
        {text = "Regular Drill - " .. (GameState.drillType == "Regular" and "EQUIPPED" or "EQUIP")},
        {text = "Steel Drill - " .. (GameState.drillType == "Steel" and "EQUIPPED" or "EQUIP")}
    }
    
    local callbacks = {
        function() -- Regular Drill
            GameState.drillType = "Regular"
            updateStatus("Regular Drill equipped!")
        end,
        function() -- Steel Drill
            if GameState.resources.steel >= 1 then
                GameState.drillType = "Steel"
                updateStatus("Steel Drill equipped!")
            else
                updateStatus("Need Steel to use Steel Drill!")
            end
        end
    }
    
    createPopupMenu("DRILL TYPES", items, callbacks)
end

-- Input handling
local function setupInputHandling()
    UserInputService.InputBegan:Connect(function(input, gameProcessed)
        if gameProcessed then return end
        
        if input.KeyCode == Enum.KeyCode.Space then
            mine()
        elseif input.KeyCode == Enum.KeyCode.S then
            showStore()
        elseif input.KeyCode == Enum.KeyCode.B then
            showBuild()
        elseif input.KeyCode == Enum.KeyCode.D then
            showDrillTypes()
        end
    end)
end

-- Auto-mine system
local function startAutoMine()
    spawn(function()
        while true do
            wait(3) -- Auto-mine every 3 seconds
            if GameState.upgrades.autoMine and not GameState.mining then
                mine()
            end
        end
    end)
end

-- Smelter system
local function startSmelter()
    spawn(function()
        while true do
            wait(5) -- Smelter operation every 5 seconds
            if GameState.buildings.smelter and GameState.resources.iron >= 2 and GameState.resources.coal >= 1 then
                GameState.resources.iron = GameState.resources.iron - 2
                GameState.resources.coal = GameState.resources.coal - 1
                GameState.resources.steel = GameState.resources.steel + 1
                GameState.buildings.smelterTemp = math.random(800, 1200) -- Simulate temperature
                updateResourceDisplay()
                updateStatus("Smelter produced Steel! Temp: " .. GameState.buildings.smelterTemp .. "°C")
            end
        end
    end)
end

-- Initialize everything
initializeGame()
setupInputHandling()
startAutoMine()
startSmelter()

-- Welcome message
updateStatus("Welcome to ASCII Mining! Press SPACE to start mining.")