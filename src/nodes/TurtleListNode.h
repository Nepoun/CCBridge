#pragma once
#include "NodeBase.h"
#include "TurtleRegistry.h"
#include "net/Server.h"

struct TurtleListNode : public NodeBase
{
    TurtleListNode() : NodeBase(NextID("node"), "Turtles", "") {}

    void draw(float delta) override
    {
        beginWindow();

        int toDisconnect = -1;

        for (auto& [id, turtle] : GetTurtleRegistry().turtles)
        {
            bool selected = GetTurtleRegistry().selectedTurtleId == turtle.id;
            ImVec4 color = turtle.state.online ? ImVec4(0.3f, 1.f, 0.3f, 1.f) : ImVec4(1.f, 0.3f, 0.3f, 1.f);

            ImGui::PushID(id);

            if (ImGui::Selectable("##sel", selected, 0, ImVec2(0, 0)))
                GetTurtleRegistry().selectedTurtleId = turtle.id;

            ImGui::SameLine();
            ImGui::TextColored(color, "[%d] %s", turtle.turtleId, turtle.turtleName.c_str());
            ImGui::Text(" fuel: %d  pos: %.0f %.0f %.0f", turtle.state.fuel, turtle.state.position.x, turtle.state.position.y, turtle.state.position.z);
            ImGui::SameLine();

            if (ImGui::Button("X"))
                toDisconnect = turtle.id;

            ImGui::Separator();
            ImGui::PopID();
        }

        if (toDisconnect != -1)
            g_server.DisconnectTurtle(toDisconnect);

        endWindow();
    }
};