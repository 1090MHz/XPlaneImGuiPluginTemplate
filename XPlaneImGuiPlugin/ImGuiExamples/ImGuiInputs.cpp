#include "imgui.h"

void RenderInputs()
{
    static float sliderFloat = 0.0f;
    static char inputText[128] = "";

    ImGui::Begin("Input Example");
    ImGui::SliderFloat("Slider", &sliderFloat, 0.0f, 1.0f);
    ImGui::InputText("Text Input", inputText, IM_ARRAYSIZE(inputText));
    ImGui::End();
}