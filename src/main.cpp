#include "DebugNode.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "net/Server.h"
#include "TurtleRegistry.h"
#include "IdUtil.h"
#include "timeUtil.h"

static void SetupDefaultLayout(ImGuiID dockspace)
{
    ImGui::DockBuilderRemoveNode(dockspace);
    ImGui::DockBuilderAddNode(dockspace, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace, ImGui::GetMainViewport()->Size);

    ImGuiID left, center, right;
    ImGui::DockBuilderSplitNode(dockspace, ImGuiDir_Left,  0.2f, &left,   &center);
    ImGui::DockBuilderSplitNode(center,    ImGuiDir_Right, 0.3f, &right,  &center);

    ImGuiID right_top, right_bottom;
    ImGui::DockBuilderSplitNode(right, ImGuiDir_Up, 0.5f, &right_top, &right_bottom);

    ImGuiID bottom;
    ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.25f, &bottom, &center);

    ImGui::DockBuilderDockWindow("Turtles", left);
    ImGui::DockBuilderDockWindow("Viewport", center);
    ImGui::DockBuilderDockWindow("Details", right_top);
    ImGui::DockBuilderDockWindow("Inventory", right_top);
    ImGui::DockBuilderDockWindow("Commands", right_bottom);
    ImGui::DockBuilderDockWindow("Tasks", right_bottom);
    ImGui::DockBuilderDockWindow("Debug & Profiler", bottom);

    ImGui::DockBuilderFinish(dockspace);
}


static void SeedTestData()
{
    TurtleBase t1(NextID("turtle"), 1, "Mineradora", "Mina carvao");
    t1.state.x = 100; t1.state.y = 64; t1.state.z = -200;
    t1.state.fuel = 800;
    t1.state.online = true;

    TurtleBase t2(NextID("turtle"), 2, "Exploradora", "");
    t2.state.x = 0; t2.state.y = 70; t2.state.z = 0;
    t2.state.fuel = 0;
    t2.state.online = false;

    GetTurtleRegistry().Register(t1);
    GetTurtleRegistry().Register(t2);
}

void TimeoutClock(){
    for (auto& [id, turtle] : GetTurtleRegistry().turtles)
    {
        if (turtle.state.online && GetBridgeCurrentTime() - turtle.state.lastUpdateTime > 10.0)
            turtle.state.online = false;
    }
}


int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 800, "CCBridge");
    SetTargetFPS(180);
    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    RenderTexture2D viewport = LoadRenderTexture(800, 600);

    SeedTestData();
    g_server.Start(4242);
    g_debug.serverPort = 4242;
    g_alloc_count = g_dealloc_count = g_alloc_bytes = 0;
    while (!WindowShouldClose())
    {
        BeginTextureMode(viewport);
        ClearBackground(BLACK);
        DrawCircle(400, 300, 50, RED);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        TimeoutClock();

        ImGuiID dockspace = ImGui::DockSpaceOverViewport();
        if (ImGui::DockBuilderGetNode(dockspace) == nullptr)
            SetupDefaultLayout(dockspace);

        g_debug.serverConnections = g_server.GetConnectionCount();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Reset Layout"))
                {
                    ImGui::DockBuilderRemoveNode(dockspace);
                    SetupDefaultLayout(dockspace);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Turtles");
        for (auto& [id, turtle] : GetTurtleRegistry().turtles)
        {
            ImVec4 color = turtle.state.online ? ImVec4(0.3f, 1.f, 0.3f, 1.f) : ImVec4(1.f, 0.3f, 0.3f, 1.f);
            ImGui::TextColored(color, "[%d] %s", turtle.turtleId, turtle.turtleName.c_str());
            ImGui::Text("  fuel: %d  pos: %.0f %.0f %.0f", turtle.state.fuel, turtle.state.x, turtle.state.y, turtle.state.z);
            ImGui::Separator();
        }
        ImGui::End();

        ImGui::Begin("Details");
        ImGui::End();

        ImGui::Begin("Inventory");
        ImGui::End();

        ImGui::Begin("Viewport");
        rlImGuiImageRenderTextureFit(&viewport, true);
        ImGui::End();

        ImGui::Begin("Commands");
        ImGui::End();

        ImGui::Begin("Tasks");
        ImGui::End();

        g_debug.draw();

        rlImGuiEnd();
        EndDrawing();
    }

    g_server.Stop();
    rlImGuiShutdown();
    UnloadRenderTexture(viewport);
    CloseWindow();
    return 0;
}