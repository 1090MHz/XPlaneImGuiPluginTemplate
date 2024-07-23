#include <imgui.h>

void RenderDrawing()
{

    // Set initial size and position
    ImGui::SetNextWindowSize(ImVec2(330, 150), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2((1920 - 330) / 2, (1080 - 150) / 2), ImGuiCond_FirstUseEver);

    ImGui::Begin("Drawing Example");

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    // Circle - Red (Tritanopia friendly)
    draw_list->AddCircleFilled(ImVec2(p.x + 50, p.y + 50), 40, IM_COL32(255, 0, 0, 255));

    // Triangle - Blue (Deuteranopia friendly)
    ImVec2 tri[3] = {
        ImVec2(p.x + 150, p.y + 10),
        ImVec2(p.x + 200, p.y + 90),
        ImVec2(p.x + 100, p.y + 90)};
    draw_list->AddTriangleFilled(tri[0], tri[1], tri[2], IM_COL32(0, 0, 255, 255));

    // Square - Green (Protanopia friendly)
    draw_list->AddRectFilled(ImVec2(p.x + 250, p.y + 10), ImVec2(p.x + 310, p.y + 70), IM_COL32(0, 255, 0, 255));

    ImGui::End();
}