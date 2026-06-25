#pragma once
#include "NodeBase.h"
#include "TurtleRegistry.h"
#include "net/Server.h"
#include <string>
#include <Vec3.h>
struct CommandsNode : public NodeBase
{
    char inputBuf[256] = {};
    Vec3 setPos = Vec3::ZERO;
    int setFacing = 0;

    // adiciona novos comandos aqui
    struct QuickCommand
    {
        const char *label;
        const char *command;
        const char *moveName;
    };
    QuickCommand quickCommands[7] = {
        {"Forward", "turtle.forward()", "forward"},
        {"Back", "turtle.back()", "back"},
        {"Turn Left", "turtle.turnLeft()", "turn_left"},
        {"Turn Right", "turtle.turnRight()", "turn_right"},
        {"Up", "turtle.up()", "up"},
        {"Down", "turtle.down()", "down"},
        {"Refuel", "turtle.refuel(9999)", ""},
    };

    CommandsNode() : NodeBase(NextID("node"), "Commands", "") {}

    void draw(float delta) override
    {
        beginWindow();

        TurtleBase *turtle = GetTurtleRegistry().Get(GetTurtleRegistry().selectedTurtleId);
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

        for (auto &qc : quickCommands)
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
        const char* facingOptions[] = { "north", "east", "south", "west" };
        ImGui::Combo("Facing", &setFacing, facingOptions, 4);
        
        if (ImGui::CollapsingHeader("Coordinates Settings")){
            ImGui::Separator();
            ImGui::Text("Position Fix");
            ImGui::Spacing();

            ImGui::InputFloat("x", &setPos.x);
            ImGui::InputFloat("y", &setPos.y);
            ImGui::InputFloat("z", &setPos.z);
        }

        if (ImGui::Button("Set") && turtle)
        {
            turtle->state.position = setPos;
            turtle->state.expectedPosition = setPos;
            turtle->state.facing = setFacing;
        }
        if (!active)
            ImGui::EndDisabled();


        endWindow();
    }
};