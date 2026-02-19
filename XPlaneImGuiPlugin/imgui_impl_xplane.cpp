// Windows SDK headers
#ifdef _WIN32
#include <windows.h>
#endif

#include "imgui_impl_xplane.h"

// Standard library headers
#include <filesystem>
#include <string>
#include <vector>

// X-Plane SDK headers
#include <XPLMDisplay.h>
#include <XPLMPlugin.h>
#include <XPLMUtilities.h>
#include <XPLMGraphics.h>

// ImGui core library
#include <imgui.h>

// OpenGL3 backend for ImGui
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h> // required for OpenGL3 loader: glTexImage2D, glTexParameteri, etc.

// Project-specific headers
#include "XPlaneLog.h"

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

        // Structure to store loaded fonts
        LoadedFonts loadedFonts;

        // Global font profiles
        std::vector<FontProfile> fontProfiles;

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

        // Forward declarations
        static void RenderImGuiFrame();

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
                    // Bring window to front so it receives input above other plugins
                    XPLMBringWindowToFront(inWindowID);
                    // Take keyboard focus when the mouse is clicked in an ImGui window
                    if (!XPLMHasKeyboardFocus(inWindowID))
                        XPLMTakeKeyboardFocus(inWindowID);
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
                // Mouse click is outside ImGui, disengage and pass the event to X-Plane
                // Ensure that the keyboard focus is removed from the XPLM window if it has it
                if (XPLMHasKeyboardFocus(inWindowID))
                {
                    XPLMTakeKeyboardFocus(nullptr);
                    // Clear input keys (in case any are pending)
                    io.ClearInputKeys();
                    
                    // Clear all incoming events
                    io.ClearEventsQueue();

                    // Clear mouse input (in case any is pending)
                    io.ClearInputMouse();
                }

                // Remove focus from the current ImGui window
                // This will provide a visual cue to the user that the window is not active
                ImGui::SetWindowFocus(nullptr);

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

#ifdef _WIN32
                // Map ImGui cursor to system cursor (Windows only)
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
#endif
                // TODO: Implement cursor handling for macOS and Linux

                return xplm_CursorCustom;
            }

            return xplm_CursorDefault; // Return the default cursor for other cases
        }

        static void DrawWindowCallback(XPLMWindowID inWindowID, void *inRefcon)
        {
            // Render ImGui content via window callback (not phase callback) so that
            // X-Plane respects z-order when XPLMBringWindowToFront() is called
            RenderImGuiFrame();
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

        static void HandleKeyEvent(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void *in_refcon, int losing_focus)
        {
            ImGuiIO &io = ImGui::GetIO();
            // Ensure ImGui is capturing keyboard input
            if (io.WantCaptureKeyboard)
            {
                // Determine if the key is pressed or released to Forward the event to ImGui
                bool keyDown = (flags & xplm_UpFlag) == 0; // Key down event (not key up)
        
                // Handle control, shift, and alt states
                io.KeyCtrl = (flags & xplm_ControlFlag) != 0;
                io.KeyShift = (flags & xplm_ShiftFlag) != 0;
                io.KeyAlt = (flags & xplm_OptionAltFlag) != 0;
        
                ImGuiKey imguiKey = ImGuiKey_None; // Default to None if not mapped
        
                // Map X-Plane virtual keys to ImGui keys using a switch statement
                switch (virtual_key)
                {
                    // Navigation keys
                    case XPLM_VK_TAB:            imguiKey = ImGuiKey_Tab; break;
                    case XPLM_VK_LEFT:           imguiKey = ImGuiKey_LeftArrow; break;
                    case XPLM_VK_RIGHT:          imguiKey = ImGuiKey_RightArrow; break;
                    case XPLM_VK_UP:             imguiKey = ImGuiKey_UpArrow; break;
                    case XPLM_VK_DOWN:           imguiKey = ImGuiKey_DownArrow; break;
                    case XPLM_VK_RETURN:         imguiKey = ImGuiKey_Enter; break;
                    case XPLM_VK_ENTER:          imguiKey = ImGuiKey_KeypadEnter; break;
                    case XPLM_VK_BACK:           imguiKey = ImGuiKey_Backspace; break;
                    case XPLM_VK_ESCAPE:         imguiKey = ImGuiKey_Escape; break;
                    case XPLM_VK_INSERT:         imguiKey = ImGuiKey_Insert; break;
                    case XPLM_VK_DELETE:         imguiKey = ImGuiKey_Delete; break;
                    case XPLM_VK_SPACE:          imguiKey = ImGuiKey_Space; break;
                    case XPLM_VK_PRIOR:          imguiKey = ImGuiKey_PageUp; break;
                    case XPLM_VK_NEXT:           imguiKey = ImGuiKey_PageDown; break;
                    case XPLM_VK_HOME:           imguiKey = ImGuiKey_Home; break;
                    case XPLM_VK_END:            imguiKey = ImGuiKey_End; break;
                    
                    // Alphabet keys (A-Z)
                    case XPLM_VK_A:              imguiKey = ImGuiKey_A; break;
                    case XPLM_VK_B:              imguiKey = ImGuiKey_B; break;
                    case XPLM_VK_C:              imguiKey = ImGuiKey_C; break;
                    case XPLM_VK_D:              imguiKey = ImGuiKey_D; break;
                    case XPLM_VK_E:              imguiKey = ImGuiKey_E; break;
                    case XPLM_VK_F:              imguiKey = ImGuiKey_F; break;
                    case XPLM_VK_G:              imguiKey = ImGuiKey_G; break;
                    case XPLM_VK_H:              imguiKey = ImGuiKey_H; break;
                    case XPLM_VK_I:              imguiKey = ImGuiKey_I; break;
                    case XPLM_VK_J:              imguiKey = ImGuiKey_J; break;
                    case XPLM_VK_K:              imguiKey = ImGuiKey_K; break;
                    case XPLM_VK_L:              imguiKey = ImGuiKey_L; break;
                    case XPLM_VK_M:              imguiKey = ImGuiKey_M; break;
                    case XPLM_VK_N:              imguiKey = ImGuiKey_N; break;
                    case XPLM_VK_O:              imguiKey = ImGuiKey_O; break;
                    case XPLM_VK_P:              imguiKey = ImGuiKey_P; break;
                    case XPLM_VK_Q:              imguiKey = ImGuiKey_Q; break;
                    case XPLM_VK_R:              imguiKey = ImGuiKey_R; break;
                    case XPLM_VK_S:              imguiKey = ImGuiKey_S; break;
                    case XPLM_VK_T:              imguiKey = ImGuiKey_T; break;
                    case XPLM_VK_U:              imguiKey = ImGuiKey_U; break;
                    case XPLM_VK_V:              imguiKey = ImGuiKey_V; break;
                    case XPLM_VK_W:              imguiKey = ImGuiKey_W; break;
                    case XPLM_VK_X:              imguiKey = ImGuiKey_X; break;
                    case XPLM_VK_Y:              imguiKey = ImGuiKey_Y; break;
                    case XPLM_VK_Z:              imguiKey = ImGuiKey_Z; break;
                    
                    // Number keys (0-9)
                    case XPLM_VK_0:              imguiKey = ImGuiKey_0; break;
                    case XPLM_VK_1:              imguiKey = ImGuiKey_1; break;
                    case XPLM_VK_2:              imguiKey = ImGuiKey_2; break;
                    case XPLM_VK_3:              imguiKey = ImGuiKey_3; break;
                    case XPLM_VK_4:              imguiKey = ImGuiKey_4; break;
                    case XPLM_VK_5:              imguiKey = ImGuiKey_5; break;
                    case XPLM_VK_6:              imguiKey = ImGuiKey_6; break;
                    case XPLM_VK_7:              imguiKey = ImGuiKey_7; break;
                    case XPLM_VK_8:              imguiKey = ImGuiKey_8; break;
                    case XPLM_VK_9:              imguiKey = ImGuiKey_9; break;
                    
                    // Numpad keys
                    case XPLM_VK_NUMPAD0:        imguiKey = ImGuiKey_Keypad0; break;
                    case XPLM_VK_NUMPAD1:        imguiKey = ImGuiKey_Keypad1; break;
                    case XPLM_VK_NUMPAD2:        imguiKey = ImGuiKey_Keypad2; break;
                    case XPLM_VK_NUMPAD3:        imguiKey = ImGuiKey_Keypad3; break;
                    case XPLM_VK_NUMPAD4:        imguiKey = ImGuiKey_Keypad4; break;
                    case XPLM_VK_NUMPAD5:        imguiKey = ImGuiKey_Keypad5; break;
                    case XPLM_VK_NUMPAD6:        imguiKey = ImGuiKey_Keypad6; break;
                    case XPLM_VK_NUMPAD7:        imguiKey = ImGuiKey_Keypad7; break;
                    case XPLM_VK_NUMPAD8:        imguiKey = ImGuiKey_Keypad8; break;
                    case XPLM_VK_NUMPAD9:        imguiKey = ImGuiKey_Keypad9; break;
                    case XPLM_VK_MULTIPLY:       imguiKey = ImGuiKey_KeypadMultiply; break;
                    case XPLM_VK_ADD:            imguiKey = ImGuiKey_KeypadAdd; break;
                    case XPLM_VK_SUBTRACT:       imguiKey = ImGuiKey_KeypadSubtract; break;
                    case XPLM_VK_DECIMAL:        imguiKey = ImGuiKey_KeypadDecimal; break;
                    case XPLM_VK_DIVIDE:         imguiKey = ImGuiKey_KeypadDivide; break;
                    case XPLM_VK_NUMPAD_ENT:     imguiKey = ImGuiKey_KeypadEnter; break;
                    case XPLM_VK_NUMPAD_EQ:      imguiKey = ImGuiKey_KeypadEqual; break;
                    
                    // Function keys (F1-F24)
                    case XPLM_VK_F1:             imguiKey = ImGuiKey_F1; break;
                    case XPLM_VK_F2:             imguiKey = ImGuiKey_F2; break;
                    case XPLM_VK_F3:             imguiKey = ImGuiKey_F3; break;
                    case XPLM_VK_F4:             imguiKey = ImGuiKey_F4; break;
                    case XPLM_VK_F5:             imguiKey = ImGuiKey_F5; break;
                    case XPLM_VK_F6:             imguiKey = ImGuiKey_F6; break;
                    case XPLM_VK_F7:             imguiKey = ImGuiKey_F7; break;
                    case XPLM_VK_F8:             imguiKey = ImGuiKey_F8; break;
                    case XPLM_VK_F9:             imguiKey = ImGuiKey_F9; break;
                    case XPLM_VK_F10:            imguiKey = ImGuiKey_F10; break;
                    case XPLM_VK_F11:            imguiKey = ImGuiKey_F11; break;
                    case XPLM_VK_F12:            imguiKey = ImGuiKey_F12; break;
                    case XPLM_VK_F13:            imguiKey = ImGuiKey_F13; break;
                    case XPLM_VK_F14:            imguiKey = ImGuiKey_F14; break;
                    case XPLM_VK_F15:            imguiKey = ImGuiKey_F15; break;
                    case XPLM_VK_F16:            imguiKey = ImGuiKey_F16; break;
                    case XPLM_VK_F17:            imguiKey = ImGuiKey_F17; break;
                    case XPLM_VK_F18:            imguiKey = ImGuiKey_F18; break;
                    case XPLM_VK_F19:            imguiKey = ImGuiKey_F19; break;
                    case XPLM_VK_F20:            imguiKey = ImGuiKey_F20; break;
                    case XPLM_VK_F21:            imguiKey = ImGuiKey_F21; break;
                    case XPLM_VK_F22:            imguiKey = ImGuiKey_F22; break;
                    case XPLM_VK_F23:            imguiKey = ImGuiKey_F23; break;
                    case XPLM_VK_F24:            imguiKey = ImGuiKey_F24; break;
                    
                    // Punctuation and symbol keys
                    case XPLM_VK_EQUAL:          imguiKey = ImGuiKey_Equal; break;
                    case XPLM_VK_MINUS:          imguiKey = ImGuiKey_Minus; break;
                    case XPLM_VK_RBRACE:         imguiKey = ImGuiKey_RightBracket; break;
                    case XPLM_VK_LBRACE:         imguiKey = ImGuiKey_LeftBracket; break;
                    case XPLM_VK_QUOTE:          imguiKey = ImGuiKey_Apostrophe; break;
                    case XPLM_VK_SEMICOLON:      imguiKey = ImGuiKey_Semicolon; break;
                    case XPLM_VK_BACKSLASH:      imguiKey = ImGuiKey_Backslash; break;
                    case XPLM_VK_COMMA:          imguiKey = ImGuiKey_Comma; break;
                    case XPLM_VK_SLASH:          imguiKey = ImGuiKey_Slash; break;
                    case XPLM_VK_PERIOD:         imguiKey = ImGuiKey_Period; break;
                    case XPLM_VK_BACKQUOTE:      imguiKey = ImGuiKey_GraveAccent; break;
                    default: 
                    {
                        // Ignore null virtual keys (normal when no key is pressed)
                        if (virtual_key != 0) {
                        // Log the unrecognized key using the provided logger
                            XPlaneLog::warn(("Unrecognized virtual key: " + std::to_string(virtual_key)).c_str());
                        }
                        imguiKey = ImGuiKey_None; 
                        break; 
                    }
                }
        
                // Forward the key event to ImGui
                if (imguiKey != ImGuiKey_None)
                {
                    io.AddKeyEvent(imguiKey, keyDown);
                }
        
                // Handle character input for text input fields
                if (keyDown)
                {
                    // Special handling for backspace key
                    if (virtual_key == XPLM_VK_BACK)
                    {
                        io.AddKeyEvent(ImGuiKey_Backspace, true); // Backspace key down
                        // io.AddInputCharacter should not be used for backspace (non-printable character)
                    }
                    // Special handling for the return key
                    // XPLM_VK_RETURN: Use this when you want to detect the Enter key on the main keyboard.
                    else if (virtual_key == XPLM_VK_RETURN)
                    {
                        io.AddKeyEvent(ImGuiKey_Enter, true); // Main keyboard Enter
                        // io.AddInputCharacter should not be used for return (non-printable character)
                    }
                    // Special handling for enter key
                    // XPLM_VK_ENTER: Use this when you want to detect the Enter key on the numeric keypad.
                    else if (virtual_key == XPLM_VK_ENTER)
                    {
                        io.AddKeyEvent(ImGuiKey_KeypadEnter, true); // Keypad Enter
                        // io.AddInputCharacter should not be used for enter (non-printable character)
                    }
                    else
                    {
                        // Forward normal character input to ImGui for text fields
                        io.AddInputCharacter((unsigned short)key); // Add the character to the input queue
                    }
                }
                else
                {
                    // Special handling for backspace key release
                    if (virtual_key == XPLM_VK_BACK)
                    {
                        io.AddKeyEvent(ImGuiKey_Backspace, false); // Backspace key up
                    }
                    // Special handling for return key release
                    else if (virtual_key == XPLM_VK_RETURN)
                    {
                        io.AddKeyEvent(ImGuiKey_Enter, false); // Main keyboard Enter key up
                    }
                    // Special handling for enter key release
                    else if (virtual_key == XPLM_VK_ENTER)
                    {
                        io.AddKeyEvent(ImGuiKey_KeypadEnter, false); // Keypad Enter key up
                    }
                }
            }
        }

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
        // Called by DrawWindowCallback so rendering respects window z-order
        static void RenderImGuiFrame()
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

        // Function to set up the keymap
        //static void SetupKeyMap()
        //{
        //    ImGuiIO &io = ImGui::GetIO(); // Get a reference to the ImGui I/O structure

        //    // Map ImGui keys to X-Plane virtual key codes
        //    io.KeyMap[ImGuiKey_Tab] = XPLM_VK_TAB;           // Tab key
        //    io.KeyMap[ImGuiKey_LeftArrow] = XPLM_VK_LEFT;    // Left arrow key
        //    io.KeyMap[ImGuiKey_RightArrow] = XPLM_VK_RIGHT;  // Right arrow key
        //    io.KeyMap[ImGuiKey_UpArrow] = XPLM_VK_UP;        // Up arrow key
        //    io.KeyMap[ImGuiKey_DownArrow] = XPLM_VK_DOWN;    // Down arrow key
        //    io.KeyMap[ImGuiKey_PageUp] = XPLM_VK_PRIOR;      // Page Up key
        //    io.KeyMap[ImGuiKey_PageDown] = XPLM_VK_NEXT;     // Page Down key
        //    io.KeyMap[ImGuiKey_Home] = XPLM_VK_HOME;         // Home key
        //    io.KeyMap[ImGuiKey_End] = XPLM_VK_END;           // End key
        //    io.KeyMap[ImGuiKey_Insert] = XPLM_VK_INSERT;     // Insert key
        //    io.KeyMap[ImGuiKey_Delete] = XPLM_VK_DELETE;     // Delete key
        //    io.KeyMap[ImGuiKey_Backspace] = XPLM_VK_BACK;    // Backspace key
        //    io.KeyMap[ImGuiKey_Space] = XPLM_VK_SPACE;       // Space key
        //    io.KeyMap[ImGuiKey_Enter] = XPLM_VK_RETURN;      // Enter key
        //    io.KeyMap[ImGuiKey_Escape] = XPLM_VK_ESCAPE;     // Escape key
        //    io.KeyMap[ImGuiKey_KeypadEnter] = XPLM_VK_ENTER; // Keypad Enter key
        //    io.KeyMap[ImGuiKey_A] = XPLM_VK_A;               // 'A' key, needed for "Select All" (Ctrl+A)
        //    io.KeyMap[ImGuiKey_C] = XPLM_VK_C;               // 'C' key, needed for "Copy" (Ctrl+C)
        //    io.KeyMap[ImGuiKey_V] = XPLM_VK_V;               // 'V' key, needed for "Paste" (Ctrl+V)
        //    io.KeyMap[ImGuiKey_X] = XPLM_VK_X;               // 'X' key, needed for "Cut" (Ctrl+X)
        //    io.KeyMap[ImGuiKey_Y] = XPLM_VK_Y;               // 'Y' key, needed for "Redo" (Ctrl+Y)
        //    io.KeyMap[ImGuiKey_Z] = XPLM_VK_Z;               // 'Z' key, needed for "Undo" (Ctrl+Z)
        //}

        static void InitLogger()
        {
            // Initialize the logger with a specific name
            XPlaneLog::init("ImGui::XP");
        }

        // ImGui X-Plane integration initialization
        void Init()
        {
            InitLogger();

            InitializeTransparentImGuiOverlay();

            // Initialize ImGui for X-Plane OpenGL rendering
            IMGUI_CHECKVERSION();
            g_ImGuiContext = ImGui::CreateContext();
            ImGui::SetCurrentContext(g_ImGuiContext);  // Critical: Set the context as current!
            ImGui_ImplOpenGL3_Init("#version 330");

            // Additional ImGui setup can be done here

            // Setup Dear ImGui style - uncomment the style you want to use
            ImGui::StyleColorsDark();
            // ImGui::StyleColorsClassic();
            // ImGui::StyleColorsLight();

            // SetupKeyMap(); // Needed to map X-Plane key codes to ImGui key codes

            // Determine the plugin's directory
            // Initialize pluginPath with a size of 512
            // Ensure that the path is null-terminated and has enough space for the full path
            pluginPath.resize(512, '\0');

            XPLMGetPluginInfo(XPLMGetMyID(), nullptr, &pluginPath[0], nullptr, nullptr);

            // Construct the path to imgui.ini within the plugin's directory
            std::filesystem::path path(pluginPath.c_str());
            std::filesystem::path iniFileName = "imgui.ini";
            std::filesystem::path iniPath = path.parent_path() / iniFileName;

            // Debug: Print or log the iniPath to verify its correctness
            XPlaneLog::info(("ImGui ini path: " + iniPath.string()).c_str());

            static std::string iniPath_string = iniPath.string();

            // Set ImGui to save its configuration to the constructed path
            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = iniPath_string.c_str();

            XPlaneLog::info("ImGui initialized for X-Plane.");
        }

        void AddGlyphToDefaultFont(const void *font_data, int font_size, float font_pixel_size, const ImWchar *glyphs_ranges, float glyphMinAdvanceXFactor)
        {
            ImGuiIO &io = ImGui::GetIO();
            io.Fonts->AddFontDefault();

            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            icons_config.GlyphMinAdvanceX = font_pixel_size * glyphMinAdvanceXFactor;

            io.Fonts->AddFontFromMemoryCompressedTTF(font_data, font_size, font_pixel_size, &icons_config, glyphs_ranges);

            // Build the font atlas
            unsigned char *tex_pixels = nullptr;
            int tex_width, tex_height;
            io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_width, &tex_height);

            // Generate texture ID using X-Plane's function
            int textureID;
            XPLMGenerateTextureNumbers(&textureID, 1);

            // Bind the texture using X-Plane's function
            XPLMBindTexture2d(textureID, 0);

            // Upload the texture to the generated texture ID
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Set the texture ID in ImGui
            io.Fonts->TexID = (ImTextureID)(intptr_t)textureID;
        }

        ImFont *LoadFontProfile(const std::string &name, const std::string &fontPath, float size)
        {
            XPlaneLog::info(("Loading: " + fontPath).c_str());

            fontProfiles.push_back({name, fontPath, size});

            // Construct the path to imgui.ini within the plugin's directory
            std::filesystem::path path(pluginPath.c_str());
            std::filesystem::path pluginFolder = path.parent_path();

            // Construct the full path to the font file
            std::filesystem::path fullPath(fontPath);
            if (fullPath.is_relative())
            {
                // Construct the full path using the plugin folder
                fullPath = pluginFolder / fontPath;
            }

            // Load the font
            ImGuiIO &io = ImGui::GetIO();
            ImFont *font = io.Fonts->AddFontFromFileTTF(fullPath.string().c_str(), size);
            IM_ASSERT(font != nullptr);
            if (font)
            {
                loadedFonts.fontMap[name] = font;
                return font;
            }
            else
            {
                XPlaneLog::error(("Failed to load font: " + fontPath).c_str());
                return nullptr;
            }
        }

        void BuildFontAtlas()
        {
            // After adding fonts, we must rebuild the font atlas and recreate device objects
            // so the OpenGL3 backend uploads the new combined font texture
            ImGui::GetIO().Fonts->Build();

            // Destroy old GPU objects (font texture) and recreate with new atlas
            ::ImGui_ImplOpenGL3_DestroyDeviceObjects();
            ::ImGui_ImplOpenGL3_CreateDeviceObjects();

            XPlaneLog::info("Font atlas built successfully.");
        }

        ImFont *GetFont(const std::string &name)
        {
            auto it = loadedFonts.fontMap.find(name);
            if (it != loadedFonts.fontMap.end())
            {
                return it->second;
            }
            return nullptr; // or handle the case where the font is not found
        }

        void RegisterImGuiRenderCallback(ImGuiRenderCallbackWrapper callback)
        {
            g_ImGuiRenderCallbacks.push_back(callback);
        }

        void UnregisterImGuiRenderCallback(ImGuiRenderCallback callback)
        {
            // Find the callback in the vector and remove it
            auto it = std::find(g_ImGuiRenderCallbacks.begin(), g_ImGuiRenderCallbacks.end(), callback);
            if (it != g_ImGuiRenderCallbacks.end())
            {
                g_ImGuiRenderCallbacks.erase(it);
            }
        }

        // ImGui X-Plane integration shutdown
        void Shutdown()
        {
            // Shutdown the ImGui OpenGL3 backend
            ImGui_ImplOpenGL3_Shutdown();

            XPlaneLog::info("ImGui shutdown for X-Plane.");

            // Destroy the ImGui context
            ImGui::DestroyContext();
        }

    }
}
