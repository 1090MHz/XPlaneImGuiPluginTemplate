#include "imgui.h"

void RenderCustomWidgets()
{
    // Set an initial window size
    ImGui::SetNextWindowSize(ImVec2(400, 550), ImGuiCond_FirstUseEver);

    // Set an initial window position
    ImGui::SetNextWindowPos(ImVec2(760, 265), ImGuiCond_FirstUseEver); // For a 1920x1080 screen

    ImGui::Begin("Custom Widgets Example");

    // Text
    ImGui::Text("Hello, ImGui!");

    // Button
    if (ImGui::Button("Click Me"))
    {
        // Button action
    }

    // Input text
    static char text[128] = "";
    ImGui::InputText("Text Input", text, IM_ARRAYSIZE(text));

    // Slider
    static float sliderValue = 0.0f;
    ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 100.0f);

    // Checkbox
    static bool checkboxChecked = false;
    ImGui::Checkbox("Checkbox", &checkboxChecked);

    // Combo box
    static const char *items[] = {"Item 1", "Item 2", "Item 3"};
    static int currentItem = 0;
    ImGui::Combo("Combo box", &currentItem, items, IM_ARRAYSIZE(items));

    // Blue Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.44f, 1.0f, 1.0f));
    if (ImGui::Button("Blue Button"))
    {
        // Button action
    }
    ImGui::PopStyleColor();

    // Orange Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.55f, 0.0f, 1.0f));
    if (ImGui::Button("Orange Button"))
    {
        // Button action
    }
    ImGui::PopStyleColor();

    // Bluish Green Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.5f, 1.0f));
    if (ImGui::Button("Bluish Green Button"))
    {
        // Button action
    }
    ImGui::PopStyleColor();

    // Yellow Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.9f, 0.25f, 1.0f));
    if (ImGui::Button("Yellow Button"))
    {
        // Button action
    }
    ImGui::PopStyleColor();

    // Sky Blue Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.7f, 0.9f, 1.0f));
    if (ImGui::Button("Sky Blue Button"))
    {
        // Button action
    }
    ImGui::PopStyleColor();

    // Vermilion Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.0f, 1.0f));
    if (ImGui::Button("Vermilion Button"))
    {
        // Button action
    }
    ImGui::PopStyleColor();

    // End of widgets
    ImGui::End();
}