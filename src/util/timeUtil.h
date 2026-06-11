#pragma once
#include <chrono>

inline double GetBridgeCurrentTime(){
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    std::chrono::duration<double> secondsDouble = duration;
    return secondsDouble.count();
}