// Standard Library Headers
#include <string.h>

// Third-Party Library Headers
#include <imgui.h>

// Project-Specific Headers
#include "imgui_impl_xplane.h"

// X-Plane SDK Headers
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"
#include "XPLMProcessing.h"

// Forward declarations
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);

// Global variables
static XPLMMenuID g_menu_id = nullptr;
// Define a structure to hold menu item indices
struct MenuItems
{
    int toggleImGuiIntro;
};

// Global instance of the structure for menu items
static MenuItems g_menuItems;

// Define a structure to hold window visibility states
struct WindowStates
{
    bool showImGuiIntro = true;
    // Add more window states as needed
};

// Global instance of the structure
static WindowStates g_windowStates;

// Updated menuHandler function
void menuHandler(void *inMenuRef, void *inItemRef)
{
    // Cast inItemRef to intptr_t for comparison
    intptr_t itemRef = reinterpret_cast<intptr_t>(inItemRef);
    // clang-format off
    switch (itemRef) {
        case 1: g_windowStates.showImGuiIntro = !g_windowStates.showImGuiIntro; break;
        default: break; // Handle other cases or do nothing
    }
    // clang-format on
}

// Updated createMenu function
void createMenu()
{
    int PluginMenuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "ImGui Window Toggle", 0, 0);
    g_menu_id = XPLMCreateMenu("XPlaneImGuiPlugin", XPLMFindPluginsMenu(), PluginMenuItem, menuHandler, nullptr);

    // Append menu items for each window and store their indices
    g_menuItems.toggleImGuiIntro = XPLMAppendMenuItem(g_menu_id, "Toggle Plugin Intro", (void *)1, 1);
    // Add more menu items as needed
}

// XPlaneImGuiIntroRender to display a simple introduction window
void XPlaneImGuiIntroRender()
{
    if (!g_windowStates.showImGuiIntro)
        return;

    ImGui::Begin("XPlaneImGuiPluginTemplate", &g_windowStates.showImGuiIntro);

    // Display text in the window
    ImGui::Text("Welcome to the X-Plane ImGui Plugin!");
    ImGui::Separator();
    ImGui::Text("This plugin demonstrates how to integrate ImGui with X-Plane.");
    ImGui::Text("You can use it as a template for creating your own ImGui-based plugins.");

    // Optionally, add more ImGui elements here as needed
    ImGui::BulletText("Easy to use ImGui interface.");
    ImGui::BulletText("Customizable for various use cases.");
    ImGui::BulletText("Lightweight and efficient.");

    ImGui::End(); // End the window
}

// ImGuiRenderCallbackWrapper to create a base ImGui window using a lambda function
// clang-format off
auto RenderDemoLambda = ImGui::XP::ImGuiRenderCallbackWrapper([]()
{
    ImGui::Begin("Demo Window"); // Add a title for the window
    ImGui::Text("Lambda Function Support!"); // Display text in the window
    ImGui::End();
});
// clang-format on

// Use ImGuiRenderCallbackWrapper to register render callbacks
auto XPlaneImGuiIntroRenderCallback = ImGui::XP::ImGuiRenderCallbackWrapper(XPlaneImGuiIntroRender);

PLUGIN_API int XPluginStart(char *out_name, char *out_signature, char *out_description)
{
    strcpy(out_name, "X-Plane ImGui Plugin");
    strcpy(out_signature, "GitHub.1090MHz.XPlaneImGui");
    strcpy(out_description, "A plugin that uses ImGui");

    ImGui::XP::Init();

    // Initialize the menu
    createMenu(); // Assuming create_menu() is the function you've defined to set up the menu

    XPLMDebugString("XPlaneImGuiPluginTemplate: Plugin started\n");

    return 1;
}

PLUGIN_API void XPluginStop(void)
{
    ImGui::XP::Shutdown();
}

PLUGIN_API void XPluginDisable(void)
{
    // Unregister the ImGui draw callback(s)
    ImGui::XP::UnregisterImGuiRenderCallback(RenderDemoLambda);
    ImGui::XP::UnregisterImGuiRenderCallback(XPlaneImGuiIntroRenderCallback);

    XPLMDebugString("XPlaneImGuiPluginTemplate: Callbacks unregistered\n");
}

PLUGIN_API int XPluginEnable(void)
{
    // Register the ImGui draw callback(s)
    ImGui::XP::RegisterImGuiRenderCallback(RenderDemoLambda);
    ImGui::XP::RegisterImGuiRenderCallback(XPlaneImGuiIntroRenderCallback);

    XPLMDebugString("XPlaneImGuiPluginTemplate: Callbacks registered\n");

    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID in_from, int in_msg, void *in_param)
{
}
