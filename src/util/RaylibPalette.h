#pragma once
#include "raylib.h"
#include "Pallette.h"

inline Color GetColorFromPalette(int index)
{
    auto& palette = GetPalette();
    if (index < 0 || index >= (int)palette.size())
        return Color{255, 0, 255, 255};
    const auto& c = palette[index];
    return Color{(unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a};
}