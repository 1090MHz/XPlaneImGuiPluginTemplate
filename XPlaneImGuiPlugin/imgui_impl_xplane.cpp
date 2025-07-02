// Windows SDK headers
#include <windows.h>

#include "imgui_impl_xplane.h"

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
#include <backends/imgui_impl_opengl3_loader.h> // required for OpenGL3 loader: glTexImage2D, glTexParameteri, etc.

// Project-specific headers
#include "XPlaneLog.h"

#include "XPLMGraphics.h"

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

                    // Get ImGui IO object
                    ImGuiIO& io = ImGui::GetIO();
                    
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
                    case XPLM_VK_TAB:            imguiKey = ImGuiKey_Tab; break;
                    case XPLM_VK_LEFT:           imguiKey = ImGuiKey_LeftArrow; break;
                    case XPLM_VK_RIGHT:          imguiKey = ImGuiKey_RightArrow; break;
                    case XPLM_VK_UP:             imguiKey = ImGuiKey_UpArrow; break;
                    case XPLM_VK_DOWN:           imguiKey = ImGuiKey_DownArrow; break;
                    case XPLM_VK_RETURN:         imguiKey = ImGuiKey_Enter; break;
                    case XPLM_VK_BACK:           imguiKey = ImGuiKey_Backspace; break;
                    case XPLM_VK_ESCAPE:         imguiKey = ImGuiKey_Escape; break;
                    case XPLM_VK_INSERT:         imguiKey = ImGuiKey_Insert; break;
                    case XPLM_VK_DELETE:         imguiKey = ImGuiKey_Delete; break;
                    case XPLM_VK_SPACE:          imguiKey = ImGuiKey_Space; break;
                    case XPLM_VK_PRIOR:          imguiKey = ImGuiKey_PageUp; break;
                    case XPLM_VK_NEXT:           imguiKey = ImGuiKey_PageDown; break;
                    case XPLM_VK_HOME:           imguiKey = ImGuiKey_Home; break;
                    case XPLM_VK_END:            imguiKey = ImGuiKey_End; break;
                    case XPLM_VK_A:              imguiKey = ImGuiKey_A; break;
                    case XPLM_VK_B:              imguiKey = ImGuiKey_B; break;
                    // Add additional key mappings here...
                    default: 
                    {
                        // Log the unrecognized key using the provided logger
                        XPlaneLog::info(("Unrecognized virtual key: " + std::to_string(virtual_key)).c_str());
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
            ImGui::GetIO().Fonts->Build();
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

            XPlaneLog::info("ImGui shutdown for X-Plane.");

            // Destroy the ImGui context
            ImGui::DestroyContext();
        }

    }
}
