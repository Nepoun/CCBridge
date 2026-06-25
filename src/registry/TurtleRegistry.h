#pragma once
#include <unordered_map>
#include <vector>
#include "Turtle.h"

struct TurtleRegistry
{
    std::unordered_map<int, TurtleBase> turtles;
    int selectedTurtleId = -1;
    void Register(TurtleBase turtle){
        turtles[turtle.id] = turtle;
    }

    void Remove(int id){
        turtles.erase(id);
    }

    TurtleBase* Get(int id)
    {
        auto found = turtles.find(id);
        return found != turtles.end() ? &found->second : nullptr;
    }

    TurtleBase* GetByCCId(int ccId)
    {
        for (auto& [id, turtle] : turtles)
            if (turtle.turtleId == ccId) return &turtle;
        return nullptr;
    }

    void SetOnline(int id, bool online)
    {
        TurtleBase* turtle = Get(id);
        if (turtle){
            turtle->state.online = online;
        }
    }

    void UpdateState(int id, TurtleState state)
    {
        TurtleBase* turtle = Get(id);
        if (turtle) turtle->state = state;
    }

    void UpdateExpectedPosition(int id, const std::string& command)
    {
        TurtleBase* turtle = Get(id);
        if (!turtle) return;

        Vec3& pos = turtle->state.expectedPosition;
        int& facing = turtle->state.facing;
        // Terrible method, need to move this calculation to its own function later
        if (command == "forward")
        {
            if (facing == 0) pos.z -= 1;
            if (facing == 1) pos.x += 1;
            if (facing == 2) pos.z += 1;
            if (facing == 3) pos.x -= 1;
        }
        else if (command == "back")
        {
            if (facing == 0) pos.z += 1;
            if (facing == 1) pos.x -= 1;
            if (facing == 2) pos.z -= 1;
            if (facing == 3) pos.x += 1;
        }
        else if (command == "up") pos.y += 1;
        else if (command == "down") pos.y -= 1;
        else if (command == "turn_left") facing = (facing - 1 + 4) % 4;
        else if (command == "turn_right") facing = (facing + 1) % 4;
    }
    
    std::vector<TurtleBase*> GetAll(){
        std::vector<TurtleBase*> result;
        for (auto& [id, turtle] : turtles){
            result.push_back(&turtle);
        }
        return result;
    }
};

inline TurtleRegistry& GetTurtleRegistry()
{
    static TurtleRegistry registry;
    return registry;
}