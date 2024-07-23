#include "imgui.h"

void RenderSimpleUI()
{
    ImGui::Begin("Simple UI Example");
    ImGui::Text("Hello, ImGui!");
    if (ImGui::Button("Click Me"))
    {
        // Button action here
    }
    ImGui::End();
}