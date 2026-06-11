#pragma once
#include "BasePeripheral.h"

// Test peripheral prob needed for turtle connection (?)
struct ModemPeripheral : public BasePeripheral
{
    bool isWireless = false;

    ModemPeripheral(ItemStack item, bool isWireless):
        BasePeripheral("modem", item), isWireless(isWireless)
    {}
};