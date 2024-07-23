#include "imgui.h"

void RenderLayouts()
{

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver); // Set window size
    ImGui::SetNextWindowPos(ImVec2(710, 340), ImGuiCond_FirstUseEver);  // Set initial position

    ImGui::Begin("Layout Example");

    // Calculate the width of the text and add a margin
    const char *text = "Horizontal Layout with Spacing";
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float spacing = textSize.x + 10; // Add 10 pixels for padding

    // Horizontal layout with dynamic spacing
    ImGui::Text("%s", text);
    ImGui::SameLine(spacing); // Use the calculated spacing
    ImGui::Button("Right of Text with Spacing");

    // Separator
    ImGui::Separator();

    // Vertical layout with custom spacing
    ImGui::Text("Vertical Layout");
    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Custom vertical spacing
    ImGui::Button("Below Text with Spacing");

    // Separator
    ImGui::Separator();

    // Columns
    ImGui::Text("Column Layout");
    ImGui::Columns(2, NULL, false); // 2 columns, without borders
    ImGui::Text("Left Column");
    ImGui::NextColumn();
    ImGui::Text("Right Column");
    ImGui::Columns(1); // Reset back to 1 column

    // Separator
    ImGui::Separator();

    // Custom spacing and alignment
    ImGui::Text("Aligned Text");
    ImGui::SameLine(ImGui::GetWindowWidth() - 100); // Align to the right
    ImGui::Text("Right");

    ImGui::End();
}