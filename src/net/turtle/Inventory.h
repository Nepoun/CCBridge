#pragma once
#include <string>
#include <array>


// Basically every data from blocks/items come from here
struct ItemData
{
    int id;
    std::string itemId;
    //#TODO: Add model, texture, icon, etc, etc
};

struct ItemStack
{
    int id;
    int count;
};

struct Inventory
{
    std::array<ItemStack, 16> slots;
};