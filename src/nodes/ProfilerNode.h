#pragma once
#include "NodeBase.h"
#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"
#include "TurtleRegistry.h"
#include "Logger.h"
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#include <psapi.h>

static size_t g_alloc_count = 0;
static size_t g_dealloc_count = 0;
static size_t g_alloc_bytes = 0;

void* operator new(size_t size)
{
    g_alloc_count++;
    g_alloc_bytes += size;
    return malloc(size);
}

void operator delete(void* ptr) noexcept
{
    g_dealloc_count++;
    free(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
    g_dealloc_count++;
    free(ptr);
}

static float GetProcessMemoryMB()
{
    PROCESS_MEMORY_COUNTERS pmc{};
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return static_cast<float>(pmc.WorkingSetSize) / (1024.f * 1024.f);
    return 0.f;
}

static float GetProcessCpuPercent()
{
    static ULARGE_INTEGER lastCpu{}, lastSysCpu{}, lastUserCpu{};
    static int numProcessors = 0;
    static bool initialized = false;

    if (!initialized)
    {
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        numProcessors = static_cast<int>(sysInfo.dwNumberOfProcessors);

        FILETIME ftime, fsys, fuser;
        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCpu, &ftime, sizeof(FILETIME));

        HANDLE self = GetCurrentProcess();
        GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCpu, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCpu, &fuser, sizeof(FILETIME));
        initialized = true;
        return 0.f;
    }

    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    HANDLE self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    float percent = 0.f;
    if (now.QuadPart != lastCpu.QuadPart)
    {
        percent = static_cast<float>(
            (sys.QuadPart - lastSysCpu.QuadPart) +
            (user.QuadPart - lastUserCpu.QuadPart)
        ) / static_cast<float>(now.QuadPart - lastCpu.QuadPart)
          / static_cast<float>(numProcessors)
          * 100.f;
    }

    lastCpu = now;
    lastSysCpu = sys;
    lastUserCpu = user;
    return std::min(percent, 100.f);
}

struct ProfilerNode : public NodeBase
{
    static constexpr int HISTORY_SIZE = 120;

    float fpsHistory[HISTORY_SIZE] = {};
    float frameHistory[HISTORY_SIZE] = {};
    float memHistory[HISTORY_SIZE] = {};
    float cpuHistory[HISTORY_SIZE] = {};
    int historyOffset = 0;

    float refreshTimer = 0.f;
    float refreshInterval = 0.1f;

    bool pauseLog = false;
    bool autoScroll = true;
    int filterLevel = 0;

    int serverPort = 0;
    int serverConnections = 0;

    float logHeight = 200.f;

    ProfilerNode() : NodeBase(NextID("node"), "Debug & Profiler", "") {}

    void update(float delta)
    {
        refreshTimer += delta;
        if (refreshTimer < refreshInterval) return;
        refreshTimer = 0.f;

        fpsHistory[historyOffset] = static_cast<float>(GetFPS());
        frameHistory[historyOffset] = GetFrameTime() * 1000.f;
        memHistory[historyOffset] = GetProcessMemoryMB();
        cpuHistory[historyOffset] = GetProcessCpuPercent();
        historyOffset = (historyOffset + 1) % HISTORY_SIZE;
    }

    void draw(float delta) override
    {
        update(delta);

        ImGui::SetNextWindowSize({520, 600}, ImGuiCond_Once);
        beginWindow();

        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float fps = static_cast<float>(GetFPS());
            float frameMs = GetFrameTime() * 1000.f;
            float memMB = GetProcessMemoryMB();
            float cpuPct = GetProcessCpuPercent();

            ImGui::Text("FPS:   %.1f", fps);
            ImGui::SameLine(160); ImGui::Text("Frame: %.2f ms", frameMs);
            ImGui::Text("RAM:   %.1f MB", memMB);
            ImGui::SameLine(160); ImGui::Text("CPU:   %.1f %%", cpuPct);
            ImGui::Spacing();

            char overlay[32];
            snprintf(overlay, sizeof(overlay), "%.0f fps", fps);
            ImGui::PlotLines("##fps", fpsHistory, HISTORY_SIZE, historyOffset,
                             overlay, 0.f, 240.f, {-1, 50});
            ImGui::SameLine(); ImGui::TextUnformatted("FPS");

            snprintf(overlay, sizeof(overlay), "%.2f ms", frameMs);
            ImGui::PlotLines("##frame", frameHistory, HISTORY_SIZE, historyOffset,
                             overlay, 0.f, 33.f, {-1, 50});
            ImGui::SameLine(); ImGui::TextUnformatted("Frame");

            ImGui::PlotLines("##mem", memHistory, HISTORY_SIZE, historyOffset,
                             nullptr, 0.f, 512.f, {-1, 50});
            ImGui::SameLine(); ImGui::TextUnformatted("RAM (MB)");

            ImGui::PlotLines("##cpu", cpuHistory, HISTORY_SIZE, historyOffset,
                             nullptr, 0.f, 100.f, {-1, 50});
            ImGui::SameLine(); ImGui::TextUnformatted("CPU %");
        }

        if (ImGui::CollapsingHeader("Allocations", ImGuiTreeNodeFlags_DefaultOpen))
        {
            long long diff = static_cast<long long>(g_alloc_count) -
                             static_cast<long long>(g_dealloc_count);

            ImGui::Text("new:    %zu", g_alloc_count);
            ImGui::Text("delete: %zu", g_dealloc_count);
            ImGui::Text("total allocated: %.2f MB",
                        static_cast<float>(g_alloc_bytes) / (1024.f * 1024.f));
            ImGui::Spacing();

            if (diff == 0)
                ImGui::TextColored({0.3f, 1.f, 0.3f, 1.f}, "OK - balanced");
            else
                ImGui::TextColored({1.f, 0.3f, 0.3f, 1.f}, "WARNING - %lld unfreed allocations", diff);

            ImGui::SameLine(300);
            if (ImGui::SmallButton("Reset"))
                g_alloc_count = g_dealloc_count = g_alloc_bytes = 0;
        }

        if (ImGui::CollapsingHeader("Server", ImGuiTreeNodeFlags_DefaultOpen))
        {
            int online = 0;
            int total = 0;
            for (auto& [id, turtle] : GetTurtleRegistry().turtles)
            {
                if (turtle.state.online) online++;
                total++;
            }
            ImGui::Text("Port:        %d", serverPort);
            ImGui::Text("Connections: %d", serverConnections);
            ImGui::Text("Registered:  %d", total);
            ImGui::Text("Online:      %d", online);
        }

        if (ImGui::CollapsingHeader("Log", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Pause", &pauseLog);
            ImGui::SameLine();
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            const char* levels[] = { "All", "Warning+", "Error" };
            ImGui::Combo("Filter", &filterLevel, levels, 3);
            ImGui::SameLine();
            if (ImGui::SmallButton("Clear")) g_logger.clear();

            ImGui::Separator();
            ImGui::BeginChild("##log", {-1, logHeight}, false, ImGuiWindowFlags_HorizontalScrollbar);

            for (auto& e : g_logger.entries)
            {
                if (filterLevel == 1 && e.level == LogLevel::INFO) continue;
                if (filterLevel == 2 && e.level != LogLevel::ERR)  continue;

                ImVec4 color = {0.85f, 0.85f, 0.85f, 1.f};
                const char* prefix = "[INFO]";
                if (e.level == LogLevel::WARNING) { color = {1.f, 0.85f, 0.f, 1.f};  prefix = "[WARN]"; }
                if (e.level == LogLevel::ERR)     { color = {1.f, 0.3f, 0.3f, 1.f};  prefix = "[ERR]";  }
                ImGui::PushTextWrapPos(0.f);
                ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s", e.timestamp.c_str());
                ImGui::PopTextWrapPos();
                ImGui::SameLine();
                ImGui::TextColored(color, "%s %s", prefix, e.message.c_str());
            }

            if (g_logger.entries.empty())
                ImGui::TextDisabled("No entries yet!");

            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.f);

            ImGui::EndChild();
            ImGui::SliderFloat("##logHeight", &logHeight, 80.f, 600.f, "height: %.0f");
        }

        endWindow();
    }
};