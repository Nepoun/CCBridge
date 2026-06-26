local server = "ws://0.0.0.0:4242/ws"

print("Connecting to server...")
local ws, err = http.websocket(server)

if not ws then
    print("Failed to connect: " .. tostring(err))
    return
end


local function getPosition()
    local x, y, z = gps.locate(2)
    if x then return x, y, z end
    return 0, 0, 0
end

print("Connected!")

ws.send(textutils.serialiseJSON({
    type = "register",
    ccId = os.getComputerID(),
    name = os.getComputerLabel() or "unnamed",
    desc = ""
}))

local commandQueue = {}

os.startTimer(0.5)

while true do
    local event, p1, p2 = os.pullEvent()

    if event == "websocket_message" then
        local data = textutils.unserialiseJSON(p2)
        if data then
            if data.type == "disconnect" then
                ws.close()
                print("Disconnected by server!")
                return
            end
            if data.commands then
                for _, cmd in ipairs(data.commands) do
                    table.insert(commandQueue, cmd)
                end
            end
        end

    elseif event == "websocket_closed" then
        print("Connection lost!")
        return

    elseif event == "terminate" then
        ws.send(textutils.serialiseJSON({
            type = "unregister",
            ccId = os.getComputerID()
        }))
        ws.close()
        print("Disconnected!")
        return

    elseif event == "timer" then
        local x, y, z = getPosition()
        ws.send(textutils.serialiseJSON({
            type = "update",
            ccId = os.getComputerID(),
            fuel = turtle.getFuelLevel(),
            x = x, y = y, z = z
        }))
        os.startTimer(0.5)

        if #commandQueue > 0 then
            local cmd = table.remove(commandQueue, 1)
            print("Running: " .. cmd)
            local fn, loadErr = load(cmd)
            if fn then
                local ok, result = pcall(fn)
                if not ok then print("Error: " .. tostring(result)) end
            else
                print("Load error: " .. tostring(loadErr))
            end
        end
    end
end