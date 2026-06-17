#pragma once
#include <string>
#include "imgui.h"
#include "Pallette.h"
#include "IdUtil.h"

struct NodeBase{
    int id;
    const char* title;
    const char* description;
    int backgroundPaletteIndex = 18;

    NodeBase(int id, const char* title, const char* description, int backgroundPaletteIndex = 18):
        id(id), title(title), description(description), backgroundPaletteIndex(backgroundPaletteIndex)
    {}

    virtual ~NodeBase() = default;
    virtual void draw(float delta) {}

protected:
    void beginWindow(){
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(Palette(backgroundPaletteIndex)));
        ImGui::Begin(title);
    }

    void endWindow()
    {
        ImGui::End();
        ImGui::PopStyleColor();
    }
};