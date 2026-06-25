#pragma once
#include <string>

inline int FacingFromString(const std::string& facing)
{
    if (facing == "north") return 0;
    if (facing == "east")  return 1;
    if (facing == "south") return 2;
    if (facing == "west")  return 3;
    return 0;
}

inline std::string FacingToString(int facing)
{
    switch (facing)
    {
        case 0: return "north";
        case 1: return "east";
        case 2: return "south";
        case 3: return "west";
    }
    return "north";
}