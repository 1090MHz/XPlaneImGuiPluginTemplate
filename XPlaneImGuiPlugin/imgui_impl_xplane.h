#ifndef IMGUI_IMPL_XPLANE_H
#define IMGUI_IMPL_XPLANE_H

// X-Plane SDK
#include <XPLMDisplay.h>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

// Standard Library
#include <functional>
#include <map>
#include <string>
#include <vector>

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

        // Initialization and Shutdown
        void Init();        // Initialize ImGui for X-Plane. Add to XPluginStart.
        // void SetupKeyMap(); // Setup key map for ImGui. Used in Init.
        void Shutdown();    // Shutdown ImGui for X-Plane. Add to XPluginStop.

        // Font Handling
        struct LoadedFonts
        {
            std::map<std::string, ImFont *> fontMap;
        };

        struct FontProfile
        {
            std::string name;
            std::string path;
            float size;
        };

        void AddGlyphToDefaultFont(const void *font_data, int font_size, float font_pixel_size, const ImWchar *icons_ranges, float glyphMinAdvanceXFactor = 1.0f);
        ImFont *LoadFontProfile(const std::string &name, const std::string &path, float size);
        void BuildFontAtlas();
        ImFont *GetFont(const std::string &name);

        // Frame Handling
        void BeginFrame(); // Begins a new ImGui frame. Used at the beginning of drawing callback.
        void EndFrame();   // Ends the current ImGui frame and renders it. Used at the end of drawing callback.

        // Registering and Unregistering ImGui Render Callbacks
        void RegisterImGuiRenderCallback(ImGuiRenderCallbackWrapper callback);
        void UnregisterImGuiRenderCallback(ImGuiRenderCallback callback);

        // Keyboard Event Callback Hook
        // Callback type for key event notifications
        // Called AFTER ImGui processes the key, allowing application to inspect ImGui's response
        // Parameters:
        //   - key: The ImGuiKey enum value that was sent to ImGui
        //   - keyDown: true if key pressed, false if released
        //   - character: The original character code from X-Plane (for printable chars)
        //   - io: Reference to ImGuiIO for inspecting WantTextInput, NavActive, etc.
        typedef void (*ImGuiKeyEventCallback)(ImGuiKey key, bool keyDown, char character, ImGuiIO& io);

        // Set optional callback to receive key events after ImGui processes them
        // Pass nullptr to unregister
        void SetKeyEventCallback(ImGuiKeyEventCallback callback);

        // Request keyboard focus release (can be called from within callback)
        void ReleaseKeyboardFocus();

    } // namespace XP

} // namespace ImGui

#endif // IMGUI_IMPL_XPLANE_H