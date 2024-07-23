#include "imgui.h"

void RenderLayouts()
{
    ImGui::Begin("Layout Example");
    ImGui::Text("Horizontal Layout");
    ImGui::SameLine();
    ImGui::Button("Right of Text");

    ImGui::Text("Vertical Layout");
    ImGui::Button("Below Text");
    ImGui::End();
}