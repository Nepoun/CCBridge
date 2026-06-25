#pragma once
#include "Logger.h"
#include "NodeBase.h"
#include "Pallette.h"
#include "TurtleRegistry.h"
#include "Server.h"
#include "IdUtil.h"


struct ServerNode : public NodeBase
{
    int customPort = 4242;

    ServerNode() : NodeBase(NextID("node"), "Server", "") {}

    void draw(float delta) override
    {
        beginWindow();

        bool running = g_server.IsRunning();

        if (running)
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Palette(39)), "ONLINE | port %d", g_server.GetPort());
        else
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Palette(64)), "OFFLINE");  
    
        ImGui::Separator();

        if (running)
        {
            int online = 0;
            int total = 0;
            for (auto& [id, turtle] : GetTurtleRegistry().turtles)
            {
                if (turtle.state.online) online++;
                total++;
            }
            ImGui::Text("Connections: %d", g_server.GetConnectionCount());
            ImGui::Text("Registered: %d", total);
            ImGui::Text("Online: %d", online);
            ImGui::Spacing();

            if (ImGui::Button("Stop Server"))
                g_server.Stop();
        }
        else
        {
            ImGui::SetNextItemWidth(120);
            ImGui::InputInt("Port", &customPort);
            ImGui::Spacing();

            if (ImGui::Button("Start Server"))
                g_server.Start(customPort);
        }

        ImGui::Separator();
        ImGui::Text("Connected Turtles: ");
        ImGui::Spacing();

        if (GetTurtleRegistry().turtles.empty())
        {
            ImGui::TextDisabled("No turtles registered");
        }
        else
        {
            for (auto& [id, turtle] : GetTurtleRegistry().turtles)
            {
                ImVec4 color = turtle.state.online ? ImGui::ColorConvertU32ToFloat4(Palette(39)) : ImGui::ColorConvertU32ToFloat4(Palette(64));
                ImGui::TextColored(color, "[%d] %s", turtle.turtleId, turtle.turtleName.c_str());
            }
        }

        endWindow();
    
    }
};