#include "Nodes.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "net/Server.h"
#include "TurtleRegistry.h"
#include "IdUtil.h"
#include "timeUtil.h"
#include "RaylibPalette.h"
#include <vector>

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

void TimeoutClock()
{
    double now = GetBridgeCurrentTime();
    for (auto& [id, turtle] : GetTurtleRegistry().turtles)
    {
        if (!turtle.state.online) continue;
        if (turtle.state.lastUpdateTime < 0) continue;
        if (now - turtle.state.lastUpdateTime > 10.0)
        {
            turtle.state.online = false;
            if (GetTurtleRegistry().selectedTurtleId == turtle.id)
                GetTurtleRegistry().selectedTurtleId = -1;
        }
    }
}

int main()
{

    // Initialization
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 800, "CCBridge");
    LoadPalette("assets/default.pal");
    SetTargetFPS(180);
    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    RenderTexture2D viewport = LoadRenderTexture(800, 600);

    g_server.Start(4242);
    g_debugNode.serverPort = 4242;
    g_alloc_count = g_dealloc_count = g_alloc_bytes = 0;

    std::vector<NodeBase*> nodes = GetNodes();

    // Loop
    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();

        // Logic based region
        TimeoutClock();

        // Render
        BeginTextureMode(viewport);
            ClearBackground(GetColorFromPalette(19));
            // TODO: Trocar para a renderização maneirissima de turtles
            // For now  you can leave this red circle that looks like TADC Caine lol
            DrawCircle(400, 300, 50, RED);
        EndTextureMode();

        // Frame
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        // Layout (Dockspace)
        ImGuiID dockspace = ImGui::DockSpaceOverViewport();
        if (ImGui::DockBuilderGetNode(dockspace) == nullptr)
            SetupDefaultLayout(dockspace);


        //Menu bar
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

        g_debugNode.serverConnections = g_server.GetConnectionCount();

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
    
        for (auto* node : nodes)
            node->draw(delta);

        rlImGuiEnd();
        EndDrawing();
    }

    g_server.Stop();
    rlImGuiShutdown();
    UnloadRenderTexture(viewport);
    CloseWindow();
    return 0;
}
