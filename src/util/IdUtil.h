#pragma once
#include <string>
#include <iostream>
#include <unordered_map>

// This is originally supposed to be only for nodes but I might try also using for every type of object for later querying
inline int NextID(const std::string& type) {
    static std::unordered_map<std::string, int> counters;
    return counters[type]++;
}