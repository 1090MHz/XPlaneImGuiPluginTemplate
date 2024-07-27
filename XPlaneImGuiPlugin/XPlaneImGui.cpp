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
    int toggleImGuiDemoLambda;
    int toggleImGuiStandaloneExample;
};

// Global instance of the structure for menu items
static MenuItems g_menuItems;

// Define a structure to hold window visibility states
struct WindowStates
{
    bool showImGuiIntro = true;
    bool show_demo_lambda = true;
    bool showImGuiStandaloneExample = true;
    // Add more window states as needed
};

// Global instance of the structure
static WindowStates g_windowStates;

// Define a global variable to toggle the ImGui demo window
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Updated menuHandler function
void menuHandler(void *inMenuRef, void *inItemRef)
{
    // Cast inItemRef to intptr_t for comparison
    intptr_t itemRef = reinterpret_cast<intptr_t>(inItemRef);
    // clang-format off
    switch (itemRef) {
        case 1: g_windowStates.showImGuiIntro = !g_windowStates.showImGuiIntro; break;
        case 2: g_windowStates.show_demo_lambda = !g_windowStates.show_demo_lambda; break;
        case 3: g_windowStates.showImGuiStandaloneExample = !g_windowStates.showImGuiStandaloneExample; break;
        default: break; // Handle other cases or do nothing
    }
    // clang-format on
}

// Updated createMenu function
void createMenu()
{
    int PluginMenuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "ImGui Window Toggle", 0, 0);
    g_menu_id = XPLMCreateMenu("XPlaneImGuiPlugin", XPLMFindPluginsMenu(), PluginMenuItem, menuHandler, nullptr);

    // Initialize the menu item counter
    int menuItemCounter = 1;

    // Append menu items for each window and store their indices
    g_menuItems.toggleImGuiIntro = XPLMAppendMenuItem(g_menu_id, "Toggle Plugin Intro", (void *)(intptr_t)menuItemCounter++, 1);
    g_menuItems.toggleImGuiDemoLambda = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Demo", (void *)(intptr_t)menuItemCounter++, 1);
    g_menuItems.toggleImGuiStandaloneExample = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Standalone Example", (void *)(intptr_t)menuItemCounter++, 1);
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

// This lambda function serves as an example of using captured variables defined in this scope.
// It captures three variables by reference: show_demo_window, show_another_window, and clear_color.
// These variables are assumed to be defined elsewhere in the code (as they are in this example).
//
// The function consists of three main parts:
// 1. It conditionally displays the ImGui demo window based on the state of 'show_demo_window'.
// 2. It creates a custom window where various ImGui widgets are demonstrated. The state of these widgets could be tied to captured variables if needed.
// 3. It conditionally displays another simple window based on the state of 'show_another_window'.
//
// This design allows for a flexible and interactive GUI where the visibility and behavior of windows can be controlled through the captured variables.

// clang-format off
auto ImGuiStandaloneExample = [&]()
{
    // This example is based on the Dear ImGui standalone example.
    // For more information, visit: https://github.com/ocornut/imgui

    // ImGui::ShowDemoWindow renders regardless of show_demo_window.
    // Its short-circuit optimization is for if the window is collapsed, not closed.
    // Use show_demo_window to control visibility explicitly.

    // Our state - Note: already defined as global variables and captured by reference
    // bool show_demo_window = true;
    // bool show_another_window = false;
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // ImGui io is required to display the framerate
     ImGuiIO &io = ImGui::GetIO();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

};
// clang-format on

// Illustrative lambda function to render a simple ImGui demo window
// clang-format off
auto RenderDemoLambda = []()
{
    ImGui::Begin("Demo Window"); // Add a title for the window
    ImGui::Text("Lambda Function Support!"); // Display text in the window
    ImGui::End();
};
// clang-format on

// Use ImGuiRenderCallbackWrapper to wrap render callbacks, optionally passing a flag to enable/disable the callback
auto XPlaneImGuiIntroRenderCallback = ImGui::XP::ImGuiRenderCallbackWrapper(XPlaneImGuiIntroRender);
auto ImGuiStandaloneExampleCallback = ImGui::XP::ImGuiRenderCallbackWrapper(ImGuiStandaloneExample, &g_windowStates.showImGuiStandaloneExample);
auto RenderDemoLambdaCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderDemoLambda, &g_windowStates.show_demo_lambda);

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
    ImGui::XP::UnregisterImGuiRenderCallback(RenderDemoLambdaCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(XPlaneImGuiIntroRenderCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(ImGuiStandaloneExampleCallback);

    XPLMDebugString("XPlaneImGuiPluginTemplate: Callbacks unregistered\n");
}

PLUGIN_API int XPluginEnable(void)
{
    // Register the ImGui draw callback(s)
    ImGui::XP::RegisterImGuiRenderCallback(RenderDemoLambdaCallback);
    ImGui::XP::RegisterImGuiRenderCallback(XPlaneImGuiIntroRenderCallback);
    ImGui::XP::RegisterImGuiRenderCallback(ImGuiStandaloneExampleCallback);

    XPLMDebugString("XPlaneImGuiPluginTemplate: Callbacks registered\n");

    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID in_from, int in_msg, void *in_param)
{
}
