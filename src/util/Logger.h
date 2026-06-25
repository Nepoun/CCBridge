#pragma once
#include <string>
#include <deque>
#include <chrono>
#include <iostream>
#include <format>
#include <mutex>

enum class LogLevel { INFO, WARNING, ERR };

struct LogEntry
{
    LogLevel level;
    std::string message;
    std::string timestamp;
};

struct DebugLogger
{
    static constexpr size_t MAX_ENTRIES = 200;
    std::deque<LogEntry> entries;
    std::mutex logMutex;

    void log(LogLevel level, const std::string& msg)
    {
        PrintToConsole(f("[LOG] {}", msg));
        std::lock_guard<std::mutex> lock(logMutex);
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        char buf[16];
        strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&t));
        entries.push_back({ level, msg, buf });
        if (entries.size() > MAX_ENTRIES)
            entries.pop_front();
    }

    template <typename... Args>
    std::string f(std::format_string<Args...> fmt, Args&&... args) {
        return std::format(fmt, std::forward<Args>(args)...);
    }

    void PrintToConsole(const std::string& msg)
    {
        printf("%s\n", msg.c_str());
        fflush(stdout);
    }


    void info(const std::string& msg)    { log(LogLevel::INFO, msg); }
    void warning(const std::string& msg) { log(LogLevel::WARNING, msg); }
    void error(const std::string& msg)   { log(LogLevel::ERR, msg); }
    void clear()                         { 
        std::lock_guard<std::mutex> lock(logMutex);
        entries.clear(); 
    }
};

inline DebugLogger g_logger;