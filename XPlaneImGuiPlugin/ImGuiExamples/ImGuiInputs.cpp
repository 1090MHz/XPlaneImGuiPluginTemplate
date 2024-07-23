#include "imgui.h"

void RenderInputs()
{
    static float sliderFloat = 0.0f;
    static char inputText[128] = "";

    // Placeholder window size, adjust based on expected content size
    ImVec2 windowSize = ImVec2(400, 200); // Example size, adjust as needed

    // Calculate center position for top-left quadrant assuming 1920x1080 resolution
    ImVec2 centerPos = ImVec2((1920 / 4) - (windowSize.x / 2), (1080 / 4) - (windowSize.y / 2));

    // Set initial position to center window in the top-left quadrant
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_FirstUseEver);
    // Set initial window size, adjust this based on your content
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

    ImGui::Begin("Input Example");
    ImGui::SliderFloat("Slider", &sliderFloat, 0.0f, 1.0f);
    ImGui::InputText("Text Input", inputText, IM_ARRAYSIZE(inputText));
    ImGui::End();
}