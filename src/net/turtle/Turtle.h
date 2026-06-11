#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "inventory.h"
#include <memory>
#include "BasePeripheral.h"

struct TurtleState
{
    float x = 0;
    float y = 0;
    float z = 0;
    double lastUpdateTime = 0.0;
    // Max possible fuel = 100k
    int fuel = 0;
    bool online = false;
    Inventory inventory;
    // shared_prt in case nothing is equipped
    std::shared_ptr<BasePeripheral> leftPeripheral;
    std::shared_ptr<BasePeripheral> rightPeripheral;
};

struct TurtleBase {
    int id;
    int turtleId;
    std::string turtleName;
    std::string turtleDesc;
    
    //Idk if i actually need this: ImVec2 startingWorldPos;
    TurtleState state;

    TurtleBase() = default;

    TurtleBase(int id, int turtleId, const char* turtleName, const char* turtleDesc):
        id(id), turtleId(turtleId), turtleName(turtleName), turtleDesc(turtleDesc)
    {}

    virtual ~TurtleBase() = default;

    virtual void PingConnection() {}


};