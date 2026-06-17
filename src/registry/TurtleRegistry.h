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