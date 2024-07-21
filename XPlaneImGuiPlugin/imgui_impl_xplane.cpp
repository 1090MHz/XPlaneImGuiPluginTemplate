#include "imgui_impl_xplane.h"

// ImGui library core
#include <imgui.h>

// OpenGL3 backend for ImGui
#include <backends/imgui_impl_opengl3.h>

// ImGui Demo Window Renderer
#include "ImGuiDemoRenderer.h"

namespace ImGui
{
    namespace XP
    {
        // An opaque handle to the window we will create
        XPLMWindowID g_window;

        // Global variable to store the window's current geometry
        WindowGeometry g_WindowGeometry;

        // Caution: The menu bar height is not included in the screen height and it may vary across platforms
        // int g_menuBarHeight = 30; // Height of the menu bar

        // Update window geometry
        static void UpdateWindowGeometry()
        {
            // Update g_WindowGeometry with the new geometry of the "Minimal Window"
            XPLMGetWindowGeometry(g_window, &g_WindowGeometry.left, &g_WindowGeometry.top, &g_WindowGeometry.right, &g_WindowGeometry.bottom);
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
            io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(g_WindowGeometry.top - y));

            // Determine if the mouse is over any ImGui content
            if (io.WantCaptureMouse)
            {
                // ImGui wants to capture the mouse, so don't pass the event to the underlying application
                return xplm_CursorArrow; // Change the cursor to indicate ImGui interaction
            }

            return xplm_CursorDefault; // Return the default cursor for other cases
        }

        void DrawWindowCallback(XPLMWindowID inWindowID, void *inRefcon)
        {
            // Do not draw anything for transparency
        }

        // Callbacks we will register when we create our window
        static int imgui_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void *in_refcon) { return 0; }
        static int imgui_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void *in_refcon) { return 0; }
        static void imgui_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void *in_refcon, int losing_focus) {}

        void InitializeTransparentImGuiOverlay()
        {
            XPLMCreateWindow_t params{};
            params.structSize = sizeof(params);
            params.visible = 1;
            params.drawWindowFunc = DrawWindowCallback;
            // Note on "dummy" handlers:
            // Even if we don't want to handle these events, we have to register a "do-nothing" callback for them
            params.handleMouseClickFunc = HandleMouseClickEvent;
            params.handleRightClickFunc = imgui_mouse_handler;
            params.handleMouseWheelFunc = imgui_wheel_handler;
            params.handleKeyFunc = imgui_key_handler;
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
            XPLMWindowID xplmWindowID = XPLMCreateWindowEx(&params);
            g_window = xplmWindowID;
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
        static void BeginFrame()
        {
            ImGui_ImplOpenGL3_NewFrame();
            NewFrame(); // Adapt as necessary.
            ImGui::NewFrame();
        }

        // Finalizes the ImGui frame and renders it to the screen. Call this at the end of your drawing callback.
        static void EndFrame()
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
            RenderDemoWindows();
            EndFrame();

            return 1;
        }

        // ImGui X-Plane integration initialization
        void Init()
        {

            InitializeTransparentImGuiOverlay();

            // Initialize ImGui for X-Plane OpenGL rendering
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui_ImplOpenGL3_Init("#version 330");

            // Additional ImGui setup can be done here

            // Setup Dear ImGui style - uncomment the style you want to use
            ImGui::StyleColorsDark(); // Use the dark style
            // ImGui::StyleColorsClassic(); // Use the classic style
            // ImGui::StyleColorsLight(); // Use the light style

            // Register the ImGui draw callback
            XPLMRegisterDrawCallback(RenderImGuiFrame, xplm_Phase_Window, 0, NULL);
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
