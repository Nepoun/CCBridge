#include "asio.hpp"
#include "crow_all.h"
#include "Server.h"
#include "Turtle.h"
#include "TurtleRegistry.h"
#include "IdUtil.h"
#include "json.hpp"
#include "timeUtil.h"

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
            // turtle conectou, aguarda mensagem de registro
        })
        .onclose([](crow::websocket::connection& conn, const std::string& reason, uint16_t) {
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

                TurtleBase turtle(NextID("turtle"), ccId, name.c_str(), desc.c_str());
                turtle.state.online = true;
                GetTurtleRegistry().Register(turtle);
                connections[ccId] = &conn;

                conn.send_text("{\"ok\":true}");
            }
            else if (type == "unregister")
            {
                int ccId = body["ccId"];
                TurtleBase* turtle = GetTurtleRegistry().GetByCCId(ccId);
                if (!turtle) return;
                GetTurtleRegistry().Remove(turtle->id);
                connections.erase(ccId);
                conn.send_text("{\"ok\":true}");
            }
            else if (type == "update")
            {
                int ccId = body["ccId"];
                TurtleBase* turtle = GetTurtleRegistry().GetByCCId(ccId);
                if (!turtle) return;

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

void Server::SendCommand(int turtleId, const std::string& command)
{
    std::lock_guard<std::mutex> lock(mutex);
    commandQueues[turtleId].push(command);

    TurtleBase* turtle = GetTurtleRegistry().Get(turtleId);
    if (!turtle) return;

    auto found = connections.find(turtle->turtleId);
    if (found != connections.end())
    {
        std::string commands = BuildCommandsJson(commandQueues[turtleId]);
        found->second->send_text("{\"commands\":" + commands + "}");
    }
}

int Server::GetConnectionCount()
{
    std::lock_guard<std::mutex> lock(mutex);
    return static_cast<int>(connections.size());
}