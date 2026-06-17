#pragma once
#include "ProfilerNode.h"
#include "TurtleListNode.h"
#include "CommandsNode.h"

inline TurtleListNode g_turtleListNode;
inline ProfilerNode g_debugNode;
inline CommandsNode g_commandsNode;

inline std::vector<NodeBase*> GetNodes()
{
    return {
        &g_turtleListNode,
        &g_debugNode,
        &g_commandsNode
    };
};