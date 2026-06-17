#pragma once
#include <string>
#include <deque>
#include <chrono>

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

    void log(LogLevel level, const std::string& msg)
    {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        char buf[16];
        strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&t));
        entries.push_back({ level, msg, buf });
        if (entries.size() > MAX_ENTRIES)
            entries.pop_front();
    }

    void info(const std::string& msg)    { log(LogLevel::INFO, msg); }
    void warning(const std::string& msg) { log(LogLevel::WARNING, msg); }
    void error(const std::string& msg)   { log(LogLevel::ERR, msg); }
    void clear()                         { entries.clear(); }
};

inline DebugLogger g_logger;