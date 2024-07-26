#include "XPLMProcessing.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "imgui_impl_xplane.h"
#include <imgui.h>
#include "ImGuiDemoRenderer.h"

#include "ImGuiExamples\ImGuiCustomWidgets.h"
#include "ImGuiExamples\ImGuiDrawing.h"
#include "ImGuiExamples\ImGuiInputs.h"
#include "ImGuiExamples\ImGuiLayouts.h"
#include "ImGuiExamples\ImGuiSimpleUI.h"

#include "XPLMMenus.h"
#include <string.h>

// Forward declarations
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginDisable(void);

// Global variables
static XPLMMenuID g_menu_id = nullptr;
// Define a structure to hold menu item indices
struct MenuItems
{
    int toggleImGuiIntro;
    int toggleImGuiDrawing;
    int toggleImGuiInputs;
    int toggleImGuiLayouts;
    int toggleImGuiSimpleUI;
    int toggleImGuiCustomWidgets;
    int reloadPlugin;
};

// Global instance of the structure for menu items
static MenuItems g_menuItems;

// Define a structure to hold window visibility states
struct WindowStates
{
    bool showImGuiIntro = true;
    bool showImGuiDrawing = false;
    bool showImGuiInputs = false;
    bool showImGuiLayouts = false;
    bool showImGuiSimpleUI = false;
    bool showImGuiCustomWidgets = false;
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
        case 2: g_windowStates.showImGuiDrawing = !g_windowStates.showImGuiDrawing; break;
        case 3: g_windowStates.showImGuiInputs = !g_windowStates.showImGuiInputs; break;
        case 4: g_windowStates.showImGuiLayouts = !g_windowStates.showImGuiLayouts; break;
        case 5: g_windowStates.showImGuiSimpleUI = !g_windowStates.showImGuiSimpleUI; break;
        case 6: g_windowStates.showImGuiCustomWidgets = !g_windowStates.showImGuiCustomWidgets; break;
        case 7: // Reload plugin
            XPluginDisable();
            XPluginEnable();
            break;
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
    g_menuItems.toggleImGuiDrawing = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Drawing", (void *)2, 1);
    g_menuItems.toggleImGuiInputs = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Inputs", (void *)3, 1);
    g_menuItems.toggleImGuiLayouts = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Layouts", (void *)4, 1);
    g_menuItems.toggleImGuiSimpleUI = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Simple UI", (void *)5, 1);
    g_menuItems.toggleImGuiCustomWidgets = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Custom Widgets", (void *)6, 1);
    g_menuItems.reloadPlugin = XPLMAppendMenuItem(g_menu_id, "Reload Plugin", (void *)7, 1);
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

    // No need to manually check for g_show_imgui_window here, ImGui handles it through the Begin() function.
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
auto RenderDemoWindowsCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderDemoWindows);
auto RenderCustomWidgetsCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderCustomWidgets, &g_windowStates.showImGuiCustomWidgets);
auto RenderDrawingCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderDrawing, &g_windowStates.showImGuiDrawing);
auto RenderInputsCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderInputs, &g_windowStates.showImGuiInputs);
auto RenderLayoutsCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderLayouts, &g_windowStates.showImGuiLayouts);
auto RenderSimpleUICallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderSimpleUI, &g_windowStates.showImGuiSimpleUI);

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
    ImGui::XP::UnregisterImGuiRenderCallback(RenderDemoWindowsCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(RenderCustomWidgetsCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(RenderDrawingCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(RenderInputsCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(RenderLayoutsCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(RenderSimpleUICallback);

    XPLMDebugString("XPlaneImGuiPluginTemplate: Callbacks unregistered\n");
}

PLUGIN_API int XPluginEnable(void)
{
    // Register the ImGui draw callback(s)
    ImGui::XP::RegisterImGuiRenderCallback(RenderDemoLambda);
    ImGui::XP::RegisterImGuiRenderCallback(XPlaneImGuiIntroRenderCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderDemoWindowsCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderCustomWidgetsCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderDrawingCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderInputsCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderLayoutsCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderSimpleUICallback);

    XPLMDebugString("XPlaneImGuiPluginTemplate: Callbacks registered\n");

    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID in_from, int in_msg, void *in_param)
{
}
