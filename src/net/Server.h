#pragma once
#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <string>

struct Server
{
    std::unordered_map<int, std::queue<std::string>> commandQueues;
    std::mutex mutex;
    std::thread thread;

    bool running = false;
    int port = 0;

    int GetConnectionCount();
    void Start(int port = 4242);
    void Stop();
    void DisconnectTurtle(int turtleId);
    void SendCommand(int turtleId, const std::string& command);
    bool IsRunning() { return running; }
    int GetPort() { return port; }
};

inline Server g_server;