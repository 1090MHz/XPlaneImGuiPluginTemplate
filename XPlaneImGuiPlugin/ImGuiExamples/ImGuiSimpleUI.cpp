#include "imgui.h"

void RenderSimpleUI()
{

    // Placeholder window size, adjust based on expected content size
    ImVec2 windowSize = ImVec2(150, 100); // Example size, adjust as needed

    // Calculate center position for top-right quadrant assuming 1920x1080 resolution
    ImVec2 centerPos = ImVec2((3 * 1920 / 4) - (windowSize.x / 2), (1080 / 4) - (windowSize.y / 2));

    // Set initial position to center window in the top-right quadrant
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_FirstUseEver);
    // Set initial window size, adjust this based on your content
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

    ImGui::Begin("Simple UI Example");
    ImGui::Text("Hello, ImGui!");
    if (ImGui::Button("Click Me"))
    {
        // Button action here
    }
    ImGui::End();
}