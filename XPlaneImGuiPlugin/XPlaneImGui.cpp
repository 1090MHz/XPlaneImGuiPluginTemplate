// Standard Library Headers
#include <string.h>

// Third-Party Library Headers
#include <imgui.h>
#include <IconsFontAwesome5.h>

// Project-Specific Headers
#include "imgui_impl_xplane.h"
#include "XPlaneLog.h"
#include "../third_party/fonts/fontawesome/fa-solid-900.inc"

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
    int togglePluginIntro;
    int togglePluginFeatures;
    int toggleImGuiStandaloneExample;
};

// Global instance of the structure for menu items
static MenuItems g_menuItems;

// Define a structure to hold window visibility states
struct WindowStates
{
    bool showPluginIntro = false;
    bool showPluginFeatures = false;
    bool showImGuiStandaloneExample = false;
    // Add more window states as needed
};

// Global instance of the structure
static WindowStates g_windowStates;

// Define a global variable to toggle the ImGui demo window
static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Font variables
static ImFont* DroidSansMono = nullptr;
static ImFont* DroidSansMono_20 = nullptr;
static ImFont* OpenSans_Regular = nullptr;
static ImFont* OpenSans_Bold = nullptr;

// Updated menuHandler function
void menuHandler(void *inMenuRef, void *inItemRef)
{
    // Cast inItemRef to intptr_t for comparison
    intptr_t itemRef = reinterpret_cast<intptr_t>(inItemRef);
    // clang-format off
    switch (itemRef) {
        case 1: g_windowStates.showPluginIntro = !g_windowStates.showPluginIntro; break;
        case 2: g_windowStates.showPluginFeatures = !g_windowStates.showPluginFeatures; break;
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
    g_menuItems.togglePluginIntro = XPLMAppendMenuItem(g_menu_id, "Toggle Plugin Intro", (void *)(intptr_t)menuItemCounter++, 1);
    g_menuItems.togglePluginFeatures = XPLMAppendMenuItem(g_menu_id, "Toggle Plugin Features Info", (void *)(intptr_t)menuItemCounter++, 1);
    g_menuItems.toggleImGuiStandaloneExample = XPLMAppendMenuItem(g_menu_id, "Toggle ImGui Standalone Example", (void *)(intptr_t)menuItemCounter++, 1);
    // Add more menu items as needed
}

// XPlanePluginIntroRender to display a simple introduction window
void XPlanePluginIntroRender()
{
    if (!g_windowStates.showPluginIntro)
        return;

    ImGui::Begin("XPlaneImGuiPluginTemplate", &g_windowStates.showPluginIntro);

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
auto RenderPluginFeatures = []()
{
    static bool transparent = false;
    static float alpha = 1.0f;

    // Create a window with the specified flags and transparency
    ImGui::SetNextWindowBgAlpha(alpha);
    ImGui::Begin("Implemented Features");

    // Checkbox to toggle transparency
    ImGui::Checkbox("Transparent", &transparent);

    // Set window transparency
    if (transparent)
    {
        alpha = 0.5f;
    }
    else
    {
        alpha = 1.0f;
    }

    // Left-click to focus ImGui content
    ImGui::Text("1. Left-click handling: Use to focus ImGui content.");

    // Right-click context menu hint
    ImGui::Text("2. Right-click handling: Use to open context menus.");

    // Right-click context menu
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Option 1"))
        {
            // Handle Option 1
        }
        if (ImGui::MenuItem("Option 2"))
        {
            // Handle Option 2
        }
        ImGui::EndPopup();
    }

    // Draggable and resizable window
    ImGui::Text("3. Drag and resize handling: Use to move and resize ImGui windows.");

    // Handle mouse wheel
    ImGui::Text("4. Mouse wheel handling: Use to scroll through content.");

    // Help marker
    ImGui::Text("5. Hover event handling: Use to create tooltips and help info: ");
    ImGui::SameLine();
    if (ImGui::Button("?"))
    {
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("This is a demo window for ImGui.\nYou can interact with it by left-clicking,\nright-clicking, dragging, resizing, and scrolling with the mouse wheel.");
        ImGui::EndTooltip();
    }


    ImGui::Text("6. Keyboard caputure: Use to type text.");

    // Multiline text edit for keyboard capture
    static char text[1024 * 16] = "Type here...";
    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_AllowTabInput);

    ImGui::Text("7. Use lambda functions: Useful for capturing variables.");

    ImGui::End();
};
// clang-format on

// Use to add selected glyphs to the default font
void AddGlyphsToFontDefault()
{
    // Selectively add glyphs to the default font to save texture space
    static ImVector<ImWchar> customGlyphRanges;
    ImFontGlyphRangesBuilder fontGlyphRangesBuilder;
    // Add the glyphs that are used in the ImGui widgets
    // clang-format off
    fontGlyphRangesBuilder.AddText(
        ICON_FA_ADJUST
        ICON_FA_ANGLE_DOUBLE_DOWN
        ICON_FA_ANGLE_DOUBLE_UP
        ICON_FA_ARROWS_ALT
        ICON_FA_BARS
        ICON_FA_BROADCAST_TOWER
        ICON_FA_COG
        ICON_FA_DOWNLOAD
        ICON_FA_ELLIPSIS_H
        ICON_FA_ERASER
        ICON_FA_EXCLAMATION_TRIANGLE
        ICON_FA_EXTERNAL_LINK_ALT
        ICON_FA_EXTERNAL_LINK_SQUARE_ALT
        ICON_FA_FILE
        ICON_FA_FILE_ALT
        ICON_FA_FILE_IMPORT
        ICON_FA_GLOBE
        ICON_FA_HOME
        ICON_FA_ID_CARD
        ICON_FA_INFO_CIRCLE
        ICON_FA_LIST
        ICON_FA_PAPER_PLANE
        ICON_FA_PLANE
        ICON_FA_PLANE_ARRIVAL
        ICON_FA_PLANE_DEPARTURE
        ICON_FA_PLANE_SLASH
        ICON_FA_PROJECT_DIAGRAM
        ICON_FA_QUESTION
        ICON_FA_ROAD
        ICON_FA_SAVE
        ICON_FA_SEARCH
        ICON_FA_SUITCASE
        ICON_FA_TIMES
        ICON_FA_USER_CIRCLE
        ICON_FA_WINDOW_MAXIMIZE
        ICON_FA_WINDOW_RESTORE
    );
    // clang-format on
    fontGlyphRangesBuilder.BuildRanges(&customGlyphRanges);

    int charWidth, charHeight;
    XPLMGetFontDimensions(xplmFont_Proportional, &charWidth, &charHeight, NULL);

    float baseFontSize = static_cast<float>(charHeight); // Use the height of the default font

    ImGui::XP::AddGlyphToDefaultFont(fa_solid_900_compressed_data, fa_solid_900_compressed_size, baseFontSize, customGlyphRanges.Data, 2.0f / 3.0f);
}

// Use to add all FontAwesome icons to the default font
void AddFontAwesomeIcons()
{
    int charWidth, charHeight;
    XPLMGetFontDimensions(xplmFont_Proportional, &charWidth, &charHeight, NULL);

    float baseFontSize = static_cast<float>(charHeight); // Use the height of the default font
    static const ImWchar glyph_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImGui::XP::AddGlyphToDefaultFont(fa_solid_900_compressed_data, fa_solid_900_compressed_size, baseFontSize, glyph_ranges, 2.0f / 3.0f);
}

// Load fonts using font profiles: name, path, and size
void LoadFonts()
{
    DroidSansMono = ImGui::XP::LoadFontProfile("DroidSansMono", "../third_party/fonts/droid-sans-mono/DroidSansMono.ttf", 16.0f);
    DroidSansMono_20 = ImGui::XP::LoadFontProfile("DroidSansMono_20", "../third_party/fonts/droid-sans-mono/DroidSansMono.ttf", 20.0f);
    OpenSans_Regular = ImGui::XP::LoadFontProfile("OpenSans_Regular", "../third_party/fonts/open-sans/OpenSans-Regular.ttf", 16.0f);
    OpenSans_Bold = ImGui::XP::LoadFontProfile("OpenSans_Bold", "../third_party/fonts/open-sans/OpenSans-Bold.ttf", 18.0f);

    // Build the font atlas
    ImGui::XP::BuildFontAtlas();
}

// Use ImGuiRenderCallbackWrapper to wrap render callbacks, optionally passing a flag to enable/disable the callback
auto XPlanePluginIntroRenderCallback = ImGui::XP::ImGuiRenderCallbackWrapper(XPlanePluginIntroRender);
auto RenderPluginFeaturesCallback = ImGui::XP::ImGuiRenderCallbackWrapper(RenderPluginFeatures, &g_windowStates.showPluginFeatures);
auto ImGuiStandaloneExampleCallback = ImGui::XP::ImGuiRenderCallbackWrapper(ImGuiStandaloneExample, &g_windowStates.showImGuiStandaloneExample);

PLUGIN_API int XPluginStart(char *out_name, char *out_signature, char *out_description)
{
    strcpy(out_name, "XPlaneImGuiPlugin");
    strcpy(out_signature, "GitHub.1090MHz.XPlaneImGui");
    strcpy(out_description, "XPlane ImGui Plugin");

    // Initialize the logger
    XPlaneLog::init(out_name);

    ImGui::XP::Init();

    // Add FontAwesome icons to the default font
    // AddFontAwesomeIcons();
    AddGlyphsToFontDefault();

    // Load additional fonts
    LoadFonts();

    // Initialize the menu
    createMenu(); // Assuming create_menu() is the function you've defined to set up the menu

    XPlaneLog::info("Plugin started");

    return 1;
}

PLUGIN_API void XPluginStop(void)
{
    ImGui::XP::Shutdown();
    XPlaneLog::info("Plugin stopped");
}

PLUGIN_API void XPluginDisable(void)
{
    // Unregister the ImGui draw callback(s)
    ImGui::XP::UnregisterImGuiRenderCallback(XPlanePluginIntroRenderCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(RenderPluginFeaturesCallback);
    ImGui::XP::UnregisterImGuiRenderCallback(ImGuiStandaloneExampleCallback);

    XPlaneLog::info("Plugin disabled");
}

PLUGIN_API int XPluginEnable(void)
{
    // Register the ImGui draw callback(s)
    ImGui::XP::RegisterImGuiRenderCallback(XPlanePluginIntroRenderCallback);
    ImGui::XP::RegisterImGuiRenderCallback(RenderPluginFeaturesCallback);
    ImGui::XP::RegisterImGuiRenderCallback(ImGuiStandaloneExampleCallback);

    XPlaneLog::info("Plugin enabled");

    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID in_from, int in_msg, void *in_param)
{
}
