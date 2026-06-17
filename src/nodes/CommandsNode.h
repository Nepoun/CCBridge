#pragma once
#include "NodeBase.h"
#include "TurtleRegistry.h"
#include "net/Server.h"
#include <string>

struct CommandsNode : public NodeBase
{
    char inputBuf[256] = {};

    // adiciona novos comandos aqui
    struct QuickCommand { const char* label; const char* command; };
    QuickCommand quickCommands[7] = {
        { "Forward",   "turtle.forward()"   },
        { "Back",      "turtle.back()"      },
        { "Turn Left", "turtle.turnLeft()"  },
        { "Turn Right","turtle.turnRight()" },
        { "Up",        "turtle.up()"        },
        { "Down",      "turtle.down()"      },
        { "Refuel",    "turtle.refuel(9999)"},
    };

    CommandsNode() : NodeBase(NextID("node"), "Commands", "") {}

    void draw(float delta) override
    {
        beginWindow();

        TurtleBase* turtle = GetTurtleRegistry().Get(GetTurtleRegistry().selectedTurtleId);
        bool active = turtle && turtle->state.online;

        if (!active)
            ImGui::BeginDisabled();

        if (turtle)
            ImGui::Text("Target: [%d] %s", turtle->turtleId, turtle->turtleName.c_str());
        else
            ImGui::TextDisabled("No turtle selected");

        ImGui::Separator();

        ImGui::Text("Quick commands");
        ImGui::Spacing();

        for (auto& qc : quickCommands)
        {
            if (ImGui::Button(qc.label))
                g_server.SendCommand(turtle->id, qc.command);
            ImGui::SameLine();
        }

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text("Custom command");
        ImGui::Spacing();

        ImGui::SetNextItemWidth(-80);
        ImGui::InputText("##cmd", inputBuf, sizeof(inputBuf));
        ImGui::SameLine();

        if (ImGui::Button("Send"))
        {
            if (inputBuf[0] != '\0')
            {
                g_server.SendCommand(turtle->id, inputBuf);
                inputBuf[0] = '\0';
            }
        }

        if (!active)
            ImGui::EndDisabled();

        endWindow();
    }
};