#ifndef IMGUI_IMPL_XPLANE_H
#define IMGUI_IMPL_XPLANE_H

#include <XPLMDisplay.h>

namespace ImGui
{
    namespace XP
    {
        // WindowGeometry structure
        struct WindowGeometry
        {
            int left, top, right, bottom;
            int width, height;
        };

        // struct WindowGeometry
        // {
        //     int left, top, right, bottom;
        //     int width, height; // Added width and height members

        //     // Default constructor
        //     WindowGeometry() : left(0), top(0), right(0), bottom(0), width(0), height(0) {}

        //     // Constructor with parameters
        //     WindowGeometry(int l, int t, int r, int b)
        //         : left(l), top(t), right(r), bottom(b), width(r - l), height(b - t) {}
        // };

        // Global variable declarations
        extern XPLMWindowID g_window;
        extern WindowGeometry g_WindowGeometry;

        // Initialization and Shutdown
        void Init();     // Initialize ImGui for X-Plane. Add to XPluginStart.
        void Shutdown(); // Shutdown ImGui for X-Plane. Add to XPluginStop.

        // Frame Handling
        void NewFrame(); // Begins a new ImGui frame. Used at the beginning of drawing callback.
        void EndFrame(); // Ends the current ImGui frame and renders it. Used at the end of drawing callback.

        // Window Management
        void UpdateWindowGeometry();              // Updates the window geometry based on X-Plane's window. Required for ImGui window positioning.
        void InitializeTransparentImGuiOverlay(); // Full-screen transparent window for ImGui rendering.

        // Event Handling
        // Mouse Events
        int HandleMouseClickEvent(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus isDown, void *inRefcon); // Handle mouse click events. This should be called from your mouse click callback.
        int imgui_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void *in_refcon);           // A dummy mouse handler function for events we don't want to process.

        // Cursor Events
        XPLMCursorStatus HandleCursorEvent(XPLMWindowID inWindowID, int x, int y, void *inRefcon); // Handle cursor events. This should be called from your cursor status callback.

        // Wheel Events
        int imgui_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void *in_refcon); // A dummy wheel handler function for mouse wheel events we don't want to process.

        // Key Events
        void imgui_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void *in_refcon, int losing_focus); // A dummy key handler function for key events we don't want to process.

    } // namespace XP

} // namespace ImGui

#endif // IMGUI_IMPL_XPLANE_H