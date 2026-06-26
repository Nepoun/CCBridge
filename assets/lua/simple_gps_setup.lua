local function input(prompt)
    io.write(prompt)
    return io.read()
end

local function turnOnComputerBelow()
    local comp = peripheral.wrap("bottom")
    if comp and comp.turnOn then
        comp.turnOn()
    end
end

local function turnToFacing(current, target)
    local diff = (target - current) % 4
    if diff == 1 then
        turtle.turnRight()
    elseif diff == 2 then
        turtle.turnRight()
        turtle.turnRight()
    elseif diff == 3 then
        turtle.turnLeft()
    end
    return target
end

local function tryMove(fn)
    local ok, err = fn()
    if not ok then
        print("Blocked: " .. tostring(err))
        return false
    end
    return true
end

local function moveTo(cx, cy, cz, facing, tx, ty, tz)
    local dx = tx - cx
    local dy = ty - cy
    local dz = tz - cz

    if dy > 0 then
        for _ = 1, dy do
            if not tryMove(turtle.up) then return nil end
            cy = cy + 1
        end
    elseif dy < 0 then
        for _ = 1, -dy do
            if not tryMove(turtle.down) then return nil end
            cy = cy - 1
        end
    end

    if dx > 0 then
        facing = turnToFacing(facing, 1)
        for _ = 1, dx do
            if not tryMove(turtle.forward) then return nil end
            cx = cx + 1
        end
    elseif dx < 0 then
        facing = turnToFacing(facing, 3)
        for _ = 1, -dx do
            if not tryMove(turtle.forward) then return nil end
            cx = cx - 1
        end
    end

    if dz > 0 then
        facing = turnToFacing(facing, 2)
        for _ = 1, dz do
            if not tryMove(turtle.forward) then return nil end
            cz = cz + 1
        end
    elseif dz < 0 then
        facing = turnToFacing(facing, 0)
        for _ = 1, -dz do
            if not tryMove(turtle.forward) then return nil end
            cz = cz - 1
        end
    end

    return cx, cy, cz, facing
end

local function refuelIfNeeded()
    if turtle.getFuelLevel() < 100 then
        turtle.select(1)
        turtle.refuel()
        print("Refueled: " .. turtle.getFuelLevel())
    end
end

local function writeFloppyStartup(drive, x, y, z)
    local path = drive.getMountPath()
    local file = fs.open(path .. "/startup.lua", "w")
    file.write('local x, y, z = ' .. x .. ', ' .. y .. ', ' .. z .. '\n')
    file.write('shell.run("gps", "host", x, y, z)\n')
    file.close()
end

local function setupStation(i, pos, floppy_slot)
    -- place computer
    turtle.select(2)
    if not turtle.placeDown() then
        print("Failed to place computer " .. i)
        return false
    end
    os.sleep(0.5)

    -- move right, place disk drive below
    turtle.turnRight()
    if not tryMove(turtle.forward) then
        print("Cannot move right at " .. i)
        return false
    end

    turtle.select(3)
    if not turtle.placeDown() then
        print("Failed to place disk drive " .. i)
        return false
    end
    os.sleep(0.5)

    local drive = peripheral.wrap("bottom")
    if not drive then
        print("Failed to wrap disk drive " .. i)
        return false
    end

    turtle.select(floppy_slot)
    turtle.dropDown()
    os.sleep(0.5)

    if not drive.isDiskPresent() then
        print("Disk not inserted " .. i)
        return false
    end

    writeFloppyStartup(drive, pos.x, pos.y - 1, pos.z)
    print("Floppy " .. i .. " written.")

    -- move past computer
    turtle.turnLeft()
    turtle.turnLeft()
    if not tryMove(turtle.forward) then return false end
    if not tryMove(turtle.forward) then return false end
    if not tryMove(turtle.forward) then return false end

    -- go down to level
    if not tryMove(turtle.down) then return false end

    -- face computer
    turtle.turnRight()
    turtle.turnRight()

    -- place modem facing computer
    turtle.select(8)
    turtle.place()
    os.sleep(0.3)
    print("Modem placed for computer " .. i)

    -- return above computer
    if not tryMove(turtle.up) then return false end
    if not tryMove(turtle.forward) then return false end
    if not tryMove(turtle.forward) then return false end

    -- boot computer
    turnOnComputerBelow()
    print("Computer " .. i .. " booting...")
    os.sleep(2)

    -- remove disk drive
    if not tryMove(turtle.forward) then return false end
    turtle.digDown()
    os.sleep(0.2)
    turtle.suckDown()
    print("Disk drive removed.")

    -- return above computer
    turtle.turnLeft()
    turtle.turnLeft()
    if not tryMove(turtle.forward) then return false end
    turtle.turnRight()

    print("Position " .. i .. " complete.")
    return true
end

print("=== GPS Constellation Setup ===")
local sx = tonumber(input("X: "))
local sy = tonumber(input("Y: "))
local sz = tonumber(input("Z: "))
local size = tonumber(input("Size (blocks): "))
local facing = tonumber(input("Facing (0=N 1=E 2=S 3=W): "))

local positions = {
    { x = sx,        y = sy + 2, z = sz        },
    { x = sx + size, y = sy + 2, z = sz        },
    { x = sx,        y = sy + 2, z = sz + size },
    { x = sx,        y = sy + size + 2, z = sz },
}

print("Slot 1: Fuel | Slot 2: Computers x4 | Slot 3: Disk Drives x4")
print("Slots 4-7: Floppy Disks | Slot 8: Ender Modems x4")
input("Press Enter when ready...")

turtle.select(1)
turtle.refuel()
print("Fuel: " .. turtle.getFuelLevel())

print("Checking area...")

local cx, cy, cz = sx, sy + 1, sz

for i, pos in ipairs(positions) do
    refuelIfNeeded()
    local result = { moveTo(cx, cy, cz, facing, pos.x, pos.y, pos.z) }
    if not result[1] then
        print("Cannot reach position " .. i .. ", aborting.")
        return
    end
    cx, cy, cz, facing = result[1], result[2], result[3], result[4]

    local blocked = turtle.inspectDown()
    if blocked then
        print("Position " .. i .. " is occupied, aborting.")
        return
    end
end

print("Area clear, returning to start...")
local ret = { moveTo(cx, cy, cz, facing, sx, sy + 1, sz) }
if not ret[1] then
    print("Failed to return to start, aborting.")
    return
end
cx, cy, cz, facing = ret[1], ret[2], ret[3], ret[4]

print("Placing computers...")

for i, pos in ipairs(positions) do
    refuelIfNeeded()
    local nav = { moveTo(cx, cy, cz, facing, pos.x, pos.y, pos.z) }
    if not nav[1] then
        print("Failed to reach position " .. i)
        return
    end
    cx, cy, cz, facing = nav[1], nav[2], nav[3], nav[4]

    if not setupStation(i, pos, 3 + i) then
        print("Setup failed at position " .. i)
        return
    end
end

print("Done. Returning to start...")
moveTo(cx, cy, cz, facing, sx, sy + 1, sz)
print("GPS constellation active.")