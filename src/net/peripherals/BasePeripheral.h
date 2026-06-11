#pragma once
#include <string>
#include "Inventory.h"

// Prob work
struct BasePeripheral
{
    std::string type;
    ItemStack item;

    BasePeripheral() = default;

    BasePeripheral(const std::string& type, ItemStack item):
        type(type), item(item)
    {}

    virtual ~BasePeripheral() = default;
    virtual void OnUpdate() {}
};