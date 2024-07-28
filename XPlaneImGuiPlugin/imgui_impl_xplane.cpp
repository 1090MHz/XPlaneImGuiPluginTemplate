#include "imgui_impl_xplane.h"

// Windows SDK headers
#include <windows.h>

// Standard library headers
#include <filesystem>
#include <string>
#include <vector>

// X-Plane SDK headers
#include <XPLMDisplay.h>
#include <XPLMPlugin.h>
#include <XPLMUtilities.h>

// ImGui core library
#include <imgui.h>

// OpenGL3 backend for ImGui
#include <backends/imgui_impl_opengl3.h>

// Logging macro for function calls with plugin name
#define LOG_CALL(func, ...)                                            \
    XPLMDebugString("XPlaneImGuiPluginTemplate: " #func " called.\n"); \
    func(__VA_ARGS__);                                                 \
    XPLMDebugString("XPlaneImGuiPluginTemplate: " #func " finished.\n");

namespace ImGui
{
    namespace XP
    {
        // An opaque handle to the window we will create
        XPLMWindowID xplmWindowID;

        // Global variable to store the window's current geometry
        WindowGeometry g_WindowGeometry;

        // Global variable to store the ImGui context
        ImGuiContext *g_ImGuiContext;

        // Unique identifier for ImGuiRenderCallbackWrapper callbacks
        int ImGuiRenderCallbackWrapper::s_nextId = 0;

        // Vector to store ImGui render callbacks
        static std::vector<ImGuiRenderCallbackWrapper> g_ImGuiRenderCallbacks;

        // Flag to check if the draw callback is registered
        static bool isDrawCallbackRegistered = false;

        // Caution: The menu bar height is not included in the screen height and it may vary across platforms
        // int g_menuBarHeight = 30; // Height of the menu bar

        // Update window geometry
        static void UpdateWindowGeometry()
        {
            // Update g_WindowGeometry with the new geometry of the "Minimal Window"
            XPLMGetWindowGeometry(xplmWindowID, &g_WindowGeometry.left, &g_WindowGeometry.top, &g_WindowGeometry.right, &g_WindowGeometry.bottom);
            // Update width and height based on the obtained geometry
            g_WindowGeometry.width = g_WindowGeometry.right - g_WindowGeometry.left;
            g_WindowGeometry.height = g_WindowGeometry.top - g_WindowGeometry.bottom;
        }

        // Callbacks
        static int HandleMouseClickEvent(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus isDown, void *inRefcon)
        {
            // Update ImGui mouse position
            ImGuiIO &io = ImGui::GetIO();
            // Invert the Y-axis to match ImGui's coordinate system
            io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(g_WindowGeometry.top - y));

            // Determine if the mouse is over any ImGui content
            if (io.WantCaptureMouse)
            {
                // Forward the mouse click to ImGui by updating the ImGuiIO structure
                if (isDown == xplm_MouseDown)
                {
                    io.MouseDown[0] = true; // Left mouse button down
                }
                else if (isDown == xplm_MouseDrag)
                {
                    io.MouseDown[0] = true; // Left mouse button still down during drag
                }
                else if (isDown == xplm_MouseUp)
                {
                    io.MouseDown[0] = false; // Left mouse button up
                }

                // Consume the event, do not pass it to X-Plane
                return 1;
            }
            else
            {
                // Mouse is not over ImGui, pass the event to X-Plane
                // Return 0 to let X-Plane handle the mouse click
                return 0;
            }
        }

        static XPLMCursorStatus HandleCursorEvent(XPLMWindowID inWindowID, int x, int y, void *inRefcon)
        {
            // Update ImGui mouse position
            ImGuiIO &io = ImGui::GetIO();
            // Invert the Y-axis to match ImGui's coordinate system
            io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(g_WindowGeometry.top - y));

            // Determine if the mouse is over any ImGui content
            if (io.WantCaptureMouse)
            {
                // ImGui wants to capture the mouse, so don't pass the event to the underlying application

                // Get the current ImGui mouse cursor
                ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();

                // Map ImGui cursor to system cursor
                switch (imgui_cursor)
                {
                case ImGuiMouseCursor_Arrow:
                    SetCursor(LoadCursor(NULL, IDC_ARROW));
                    break;
                case ImGuiMouseCursor_TextInput:
                    SetCursor(LoadCursor(NULL, IDC_IBEAM));
                    break;
                case ImGuiMouseCursor_ResizeAll:
                    SetCursor(LoadCursor(NULL, IDC_SIZEALL));
                    break;
                case ImGuiMouseCursor_ResizeNS:
                    SetCursor(LoadCursor(NULL, IDC_SIZENS));
                    break;
                case ImGuiMouseCursor_ResizeEW:
                    SetCursor(LoadCursor(NULL, IDC_SIZEWE));
                    break;
                case ImGuiMouseCursor_ResizeNESW:
                    SetCursor(LoadCursor(NULL, IDC_SIZENESW));
                    break;
                case ImGuiMouseCursor_ResizeNWSE:
                    SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
                    break;
                case ImGuiMouseCursor_Hand:
                    SetCursor(LoadCursor(NULL, IDC_HAND));
                    break;
                case ImGuiMouseCursor_NotAllowed:
                    SetCursor(LoadCursor(NULL, IDC_NO));
                    break;
                default:
                    SetCursor(LoadCursor(NULL, IDC_ARROW));
                    break;
                }

                return xplm_CursorCustom;
            }

            return xplm_CursorDefault; // Return the default cursor for other cases
        }

        static void DrawWindowCallback(XPLMWindowID inWindowID, void *inRefcon)
        {
            // Do not draw anything for transparency
        }

        static int HandleRightClickEvent(XPLMWindowID in_window_id, int x, int y, int is_down, void *in_refcon)
        {
            // Update ImGui mouse position
            ImGuiIO &io = ImGui::GetIO();
            // Invert the Y-axis to match ImGui's coordinate system
            io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(g_WindowGeometry.top - y));

            // Determine if the mouse is over any ImGui content
            if (io.WantCaptureMouse)
            {
                if (is_down == xplm_MouseDown)
                {
                    io.MouseDown[1] = true; // Right mouse button down
                }
                else if (is_down == xplm_MouseDrag)
                {
                    io.MouseDown[1] = true; // Right mouse button still down during drag
                }
                else if (is_down == xplm_MouseUp)
                {
                    io.MouseDown[1] = false; // Right mouse button up
                }

                return 1; // Consume the event
            }
            else
            {
                // Mouse is not over ImGui, pass the event to X-Plane
                // Return 0 to let X-Plane handle the mouse click
                return 0;
            }
        }

        static int HandleMouseWheelEvent(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void *in_refcon)
        {
            ImGuiIO &io = ImGui::GetIO();
            if (io.WantCaptureMouse)
            {
                io.MouseWheel += static_cast<float>(clicks);
                return 1; // Indicate that the event has been handled by ImGui
            }
            return 0; // Let the event pass through to the underlying application
        }
        static void HandleKeyEvent(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void *in_refcon, int losing_focus) {}

        static void InitializeTransparentImGuiOverlay()
        {
            XPLMCreateWindow_t params{};
            params.structSize = sizeof(params);
            params.visible = 1;
            params.drawWindowFunc = DrawWindowCallback;
            // Note on "dummy" handlers:
            // Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
            params.handleMouseClickFunc = HandleMouseClickEvent;
            params.handleRightClickFunc = HandleRightClickEvent;
            params.handleMouseWheelFunc = HandleMouseWheelEvent;
            params.handleKeyFunc = HandleKeyEvent;
            params.handleCursorFunc = HandleCursorEvent;
            // Set refcon to NULL or a pointer to your data structure if needed
            params.refcon = NULL;
            params.layer = xplm_WindowLayerFloatingWindows;
            params.decorateAsFloatingWindow = xplm_WindowDecorationNone; // No decoration for full transparency

            // params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle; // Rounded rectangle window
            // params.decorateAsFloatingWindow = xplm_WindowDecorationSelfDecorated; // Self-decorated window
            // params.decorateAsFloatingWindow = xplm_WindowDecorationSelfDecoratedResizable; // Self-decorated resizable window

            // Initialize the window boundaries.
            // Relying on the lower-left corner of the primary monitor being at (0, 0) is not advisable.
            // It is necessary to obtain the global desktop dimensions.
            int left, bottom, right, top;
            XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);

            // Retrieve the screen width and height
            int screenWidth, screenHeight;
            XPLMGetScreenSize(&screenWidth, &screenHeight);

            // Full screen window
            params.left = left + 0;
            params.bottom = bottom + 0;
            params.right = params.left + screenWidth;
            params.top = params.bottom + screenHeight;

            // Ensure geometry is updated before window creation so that it is available in callbacks
            g_WindowGeometry.width = screenWidth;
            g_WindowGeometry.height = screenHeight;
            g_WindowGeometry.left = params.left;
            g_WindowGeometry.top = params.top;
            g_WindowGeometry.right = params.right;
            g_WindowGeometry.bottom = params.bottom;

            // Create the window
            xplmWindowID = XPLMCreateWindowEx(&params);
        }

        // Prepare ImGui for a new frame in the X-Plane environment
        static void NewFrame()
        {
            // Adapt ImGui to the current display size and DPI settings of X-Plane
            // This might involve querying X-Plane for the current window size and passing it to ImGui
            int windowWidth, windowHeight;
            XPLMGetScreenSize(&windowWidth, &windowHeight);
            ImGui::GetIO().DisplaySize = ImVec2((float)windowWidth, (float)windowHeight);

            // Start a new ImGui frame after adapting to X-Plane's environment
            // ImGui::NewFrame();
        }

        // Initializes a new ImGui frame. Call this at the beginning of your drawing callback.
        void BeginFrame()
        {
            ImGui_ImplOpenGL3_NewFrame();
            NewFrame(); // Adapt as necessary.
            ImGui::NewFrame();
        }

        // Finalizes the ImGui frame and renders it to the screen. Call this at the end of your drawing callback.
        void EndFrame()
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // If using ImGui's docking features, this is necessary
            // if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            //    GLFWwindow* backup_current_context = glfwGetCurrentContext();
            //    ImGui::UpdatePlatformWindows();
            //    ImGui::RenderPlatformWindowsDefault();
            //    glfwMakeContextCurrent(backup_current_context);
            //}

            // Call EndFrame() if you're not immediately starting a new frame afterwards
            ImGui::EndFrame();
        }

        // Renders ImGui frame within OpenGL context
        static int RenderImGuiFrame(XPLMDrawingPhase phase, int isBefore, void *refcon)
        {
            BeginFrame();

            for (auto &callbackEntry : g_ImGuiRenderCallbacks)
            {
                // Check if the visibility flag is true before executing the callback
                if (callbackEntry.getVisibilityFlag())
                {
                    callbackEntry.getCallback()();
                }
            }

            EndFrame();

            return 1;
        }

        // ImGui X-Plane integration initialization
        void Init()
        {
            InitializeTransparentImGuiOverlay();

            // Initialize ImGui for X-Plane OpenGL rendering
            IMGUI_CHECKVERSION();
            g_ImGuiContext = ImGui::CreateContext();
            ImGui_ImplOpenGL3_Init("#version 330");

            // Additional ImGui setup can be done here

            // Setup Dear ImGui style - uncomment the style you want to use
            ImGui::StyleColorsDark();
            // ImGui::StyleColorsClassic();
            // ImGui::StyleColorsLight();

            // Determine the plugin's directory
            char pluginPath[512];
            XPLMGetPluginInfo(XPLMGetMyID(), nullptr, pluginPath, nullptr, nullptr);

            // Construct the path to imgui.ini within the plugin's directory
            std::filesystem::path path(pluginPath);
            std::filesystem::path iniFileName = "imgui.ini";
            std::filesystem::path iniPath = path.parent_path() / iniFileName;

            // Debug: Print or log the iniPath to verify its correctness
            XPLMDebugString(("ImGui ini path: " + iniPath.string() + "\n").c_str());

            static std::string iniPath_string = iniPath.string();

            // Set ImGui to save its configuration to the constructed path
            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = iniPath_string.c_str();
        }

        static void EnsureImGuiDrawCallbackRegistered()
        {
            if (!isDrawCallbackRegistered)
            {
                XPLMRegisterDrawCallback(RenderImGuiFrame, xplm_Phase_Window, 0, NULL);
                isDrawCallbackRegistered = true;
            }
        }

        static void EnsureImGuiDrawCallbackUnregistered()
        {
            if (isDrawCallbackRegistered)
            {
                XPLMUnregisterDrawCallback(RenderImGuiFrame, xplm_Phase_Window, 0, NULL);
                isDrawCallbackRegistered = false;
            }
        }

        void RegisterImGuiRenderCallback(ImGuiRenderCallbackWrapper callback)
        {
            if (g_ImGuiRenderCallbacks.empty())
            {
                EnsureImGuiDrawCallbackRegistered();
            }
            g_ImGuiRenderCallbacks.push_back(callback);
        }

        void UnregisterImGuiRenderCallback(ImGuiRenderCallback callback)
        {
            // Find the callback in the vector and remove it
            auto it = std::find(g_ImGuiRenderCallbacks.begin(), g_ImGuiRenderCallbacks.end(), callback);
            if (it != g_ImGuiRenderCallbacks.end())
            {
                g_ImGuiRenderCallbacks.erase(it);
                if (g_ImGuiRenderCallbacks.empty())
                {
                    EnsureImGuiDrawCallbackUnregistered();
                }
            }
        }

        // ImGui X-Plane integration shutdown
        void Shutdown()
        {
            // Shutdown the ImGui OpenGL3 backend
            ImGui_ImplOpenGL3_Shutdown();
            // Destroy the ImGui context
            ImGui::DestroyContext();
        }

    }
}
