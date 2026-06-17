#include "asio.hpp"
#include "crow_all.h"
#include "Server.h"
#include "Turtle.h"
#include "TurtleRegistry.h"
#include "IdUtil.h"
#include "json.hpp"
#include "timeUtil.h"
#include "Logger.h"

static crow::SimpleApp app;
static std::unordered_map<int, crow::websocket::connection*> connections;

static std::string BuildCommandsJson(std::queue<std::string>& queue)
{
    std::string result = "[";
    while (!queue.empty())
    {
        result += "\"" + queue.front() + "\"";
        queue.pop();
        if (!queue.empty()) result += ",";
    }
    return result + "]";
}

void Server::Start(int port)
{
    CROW_ROUTE(app, "/ws")
        .websocket(&app)
        .onopen([](crow::websocket::connection& conn) {
            g_logger.info("New connection opened");
            // turtle conectou, aguarda mensagem de registro
        })
        .onclose([](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
            g_logger.warning("Connection closed - code: " + std::to_string(code) + " reason: " + reason);
            std::lock_guard<std::mutex> lock(g_server.mutex);
            for (auto& [ccId, c] : connections)
            {
                if (c == &conn)
                {
                    TurtleBase* turtle = GetTurtleRegistry().GetByCCId(ccId);
                    if (turtle) turtle->state.online = false;
                    connections.erase(ccId);
                    break;
                }
            }
        })
        .onmessage([this](crow::websocket::connection& conn, const std::string& data, bool isBinary) {
            std::lock_guard<std::mutex> lock(mutex);

            auto body = nlohmann::json::parse(data, nullptr, false);
            if (body.is_discarded()) return;

            std::string type = body.value("type", "");

            if (type == "register")
            {
                int ccId = body["ccId"];
                std::string name = body.value("name", "unnamed");
                std::string desc = body.value("desc", "");
                g_logger.info("New turtle named " + name + " connected to CCBridge");

                TurtleBase* existing = GetTurtleRegistry().GetByCCId(ccId);
                if (existing){
                    existing->state.online = true;
                    connections[ccId] = &conn;
                    conn.send_text("{\"ok\":true}");
                    g_logger.info("Turtle " + name + " reconnected to network");
                    return;
                }
                
                TurtleBase turtle(NextID("turtle"), ccId, name.c_str(), desc.c_str());
                turtle.state.online = true;
                GetTurtleRegistry().Register(turtle);
                connections[ccId] = &conn;
                g_logger.info("Registering new turtle " + name + " into network");

                conn.send_text("{\"ok\":true}");
            }
            else if (type == "unregister")
            {
                int ccId = body["ccId"];
                TurtleBase* turtle = GetTurtleRegistry().GetByCCId(ccId);
                if (!turtle) return;
                g_logger.info("Turtle " + turtle->turtleName + " unregistered from network");
                GetTurtleRegistry().Remove(turtle->id);
                connections.erase(ccId);
                conn.send_text("{\"ok\":true}");
            }
            else if (type == "update")
            {
                int ccId = body["ccId"];
                TurtleBase* turtle = GetTurtleRegistry().GetByCCId(ccId);
                if (!turtle) return;
                
                //g_logger.info("Turtle " + turtle->turtleName + " sent an update");
                turtle->state.fuel = body.value("fuel", 0);
                turtle->state.x = body.value("x", 0.0f);
                turtle->state.y = body.value("y", 0.0f);
                turtle->state.z = body.value("z", 0.0f);
                turtle->state.online = true;

                
                turtle->state.lastUpdateTime = GetBridgeCurrentTime();
                std::string commands = BuildCommandsJson(commandQueues[turtle->id]);
                conn.send_text("{\"commands\":" + commands + "}");
            }
        });

    thread = std::thread([port]() {
        app.port(port).run();
    });
    thread.detach();
}

void Server::Stop()
{
    app.stop();
}

void Server::DisconnectTurtle(int turtleId)
{
    std::lock_guard<std::mutex> lock(mutex);
    
    TurtleBase* turtle = GetTurtleRegistry().Get(turtleId);
    if (!turtle) return;
    
    auto found = connections.find(turtle->turtleId);
    if (found != connections.end())
    {
        g_logger.info("Turtle " + turtle->turtleName + " forcefully disconnected from network");
        found->second->send_text("{\"type\":\"disconnect\"}");
        connections.erase(found);
    }

    GetTurtleRegistry().Remove(turtleId);
}

void Server::SendCommand(int turtleId, const std::string& command)
{
    std::lock_guard<std::mutex> lock(mutex);
    TurtleBase* turtle = GetTurtleRegistry().Get(turtleId);
    if (!turtle)
    {
        g_logger.warning("Unable to queue {" + command + "} - turtle not found");
        return;
    }
    commandQueues[turtleId].push(command);
    g_logger.info("Queued {" + command + "} for turtle " + turtle->turtleName);
}

int Server::GetConnectionCount()
{
    std::lock_guard<std::mutex> lock(mutex);
    return static_cast<int>(connections.size());
}