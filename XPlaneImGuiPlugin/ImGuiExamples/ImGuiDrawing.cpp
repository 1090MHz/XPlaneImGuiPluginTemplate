#include "imgui.h"

void RenderDrawing()
{
    ImGui::Begin("Drawing Example");
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();
    draw_list->AddCircleFilled(ImVec2(p.x + 50, p.y + 50), 40, IM_COL32(255, 0, 0, 255));
    ImGui::End();
}