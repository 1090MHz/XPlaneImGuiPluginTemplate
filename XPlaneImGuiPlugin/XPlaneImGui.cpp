#include "XPLMProcessing.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "imgui_impl_xplane.h"

float flight_loop_callback(float elapsed_since_last_call, float elapsed_since_last_loop, int counter, void *refcon)
{
    ImGui::XP::NewFrame();

    // ImGui rendering goes here

    ImGui::XP::EndFrame();
    return -1;
}

PLUGIN_API int XPluginStart(char *out_name, char *out_signature, char *out_description)
{
    strcpy(out_name, "X-Plane ImGui Plugin");
    strcpy(out_signature, "GitHub.1090MHz.XPlaneImGui");
    strcpy(out_description, "A plugin that uses ImGui");

    ImGui::XP::Init();

    return 1;
}

PLUGIN_API void XPluginStop(void)
{
    ImGui::XP::Shutdown();
}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API int XPluginEnable(void)
{
    XPLMRegisterFlightLoopCallback(flight_loop_callback, -1, NULL);
    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID in_from, int in_msg, void *in_param)
{
}
