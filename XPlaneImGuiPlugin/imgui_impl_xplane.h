#ifndef IMGUI_IMPL_XPLANE_H
#define IMGUI_IMPL_XPLANE_H

#include <XPLMDisplay.h>
#include "imgui.h"
#include <vector>
#include <functional>

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

        // Global variable declarations
        // External variables are declared in imgui_impl_xplane.cpp

        // X-Plane window identifier
        extern XPLMWindowID xplmWindowID;

        // Geometry for the X-Plane window
        extern WindowGeometry g_WindowGeometry;

        // ImGui context pointer
        extern ImGuiContext *g_ImGuiContext;

        // Static variable for pluginPath
        static std::string pluginPath;

        class ImGuiRenderCallbackWrapper
        {
        public:
            ImGuiRenderCallbackWrapper(std::function<void()> callback, bool *callbackEnabledFlag = nullptr)
                : m_callback(callback), m_callbackEnabledFlag(callbackEnabledFlag), m_id(s_nextId++) {}

            void operator()() const
            {
                if (!m_callbackEnabledFlag || *m_callbackEnabledFlag)
                    m_callback();
            }
            bool operator==(const ImGuiRenderCallbackWrapper &other) const { return m_id == other.m_id; }

            // Getter methods
            bool getVisibilityFlag() const { return m_callbackEnabledFlag ? *m_callbackEnabledFlag : true; }
            std::function<void()> getCallback() const { return m_callback; }

        private:
            std::function<void()> m_callback;
            bool *m_callbackEnabledFlag;
            int m_id;
            static int s_nextId;
        };

        typedef ImGuiRenderCallbackWrapper ImGuiRenderCallback;

        extern std::vector<ImGuiRenderCallbackWrapper> g_ImGuiRenderCallbacks;

        // Initialization and Shutdown
        void Init();        // Initialize ImGui for X-Plane. Add to XPluginStart.
        ImFont *LoadFonts(const char *fontFilePath, float fontSize); // Load custom font for ImGui.
        void SetupKeyMap(); // Setup key map for ImGui. Used in Init.
        void Shutdown();    // Shutdown ImGui for X-Plane. Add to XPluginStop.

        // Frame Handling
        void BeginFrame(); // Begins a new ImGui frame. Used at the beginning of drawing callback.
        void EndFrame();   // Ends the current ImGui frame and renders it. Used at the end of drawing callback.
        int RenderImGuiFrame(XPLMDrawingPhase phase, int isBefore, void *refcon);

        // Registering and Unregistering ImGui Render Callbacks
        void RegisterImGuiRenderCallback(ImGuiRenderCallbackWrapper callback);
        void UnregisterImGuiRenderCallback(ImGuiRenderCallback callback);

        // Ensuring ImGui Draw Callback is Registered/Unregistered
        void EnsureImGuiDrawCallbackRegistered();
        void EnsureImGuiDrawCallbackUnregistered();

        // Window Management
        void UpdateWindowGeometry();
        void InitializeTransparentImGuiOverlay();

        // Event Handling
        int HandleMouseClickEvent(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus isDown, void *inRefcon);
        int HandleRightClickEvent(XPLMWindowID in_window_id, int x, int y, int is_down, void *in_refcon);
        XPLMCursorStatus HandleCursorEvent(XPLMWindowID inWindowID, int x, int y, void *inRefcon);
        int HandleMouseWheelEvent(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void *in_refcon);
        void HandleKeyEvent(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void *in_refcon, int losing_focus);

    } // namespace XP

} // namespace ImGui

#endif // IMGUI_IMPL_XPLANE_H