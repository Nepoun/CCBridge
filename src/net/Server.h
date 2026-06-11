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

    int GetConnectionCount();
    void Start(int port = 4242);
    void Stop();
    void SendCommand(int turtleId, const std::string& command);
};

inline Server g_server;