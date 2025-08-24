# XPlaneImGuiPluginTemplate

[![Build Status](https://github.com/1090MHz/XPlaneImGuiPluginTemplate/workflows/Build/badge.svg)](https://github.com/1090MHz/XPlaneImGuiPluginTemplate/actions?workflow=Build)

A lightweight X-Plane plugin that integrates ImGui for enhanced in-sim user interfaces.

## Building the Plugin

This project uses CMake for cross-platform building and supports Windows, Linux, and macOS. A Visual Studio solution is also provided for convenience on Windows.

> **Note:** Examples below use Windows paths and commands. Adjust paths and shell syntax for your platform (e.g., use `/home/user/X-Plane` on Linux, `/Applications/X-Plane` on macOS).

### Prerequisites

- **CMake** (version 3.15 or higher)
- **C++ Compiler** with C++17 support:
  - Windows: Visual Studio 2019+ or MinGW
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+
- **Docker** (for dependency management)

### Setup Dependencies

First, set up the required libraries and fonts using the PreflightCheckLibs submodule:

**For Windows PowerShell:**
```powershell
$env:EXPORT_BASE_DIR="../third_party"; docker-compose -f ./PreflightCheckLibs/docker-compose.yml up
```

**For Windows Command Prompt:**
```cmd
set EXPORT_BASE_DIR=../third_party && docker-compose -f ./PreflightCheckLibs/docker-compose.yml up
```

**For Linux/macOS:**
```bash
EXPORT_BASE_DIR="../third_party" docker-compose -f ./PreflightCheckLibs/docker-compose.yml up
```

This will create the `third_party` directory with:
- X-Plane SDK (XPSDK410)
- libcurl
- Required fonts (Open Sans, Droid Sans Mono)
- Other dependencies

### Build Instructions

1. **Create a build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Configure the project:**
   
   **Simple configuration:**
   ```bash
   cmake ..
   ```
   
   **With X-Plane path (recommended):**
   ```bash
   cmake .. -DXPLANE_DIR="C:/X-Plane 12"
   ```

3. **Build the plugin:**
   ```bash
   cmake --build . --config Release
   ```

4. **Install the plugin:**
   
   **For X-Plane deployment (recommended):**
   ```bash
   cmake --build . --target install-xplane
   ```
   *(Requires XPLANE_DIR to be set during configuration)*
   
   **If XPLANE_DIR was not defined, use standard CMake install:**
   ```bash
   cmake --install . --prefix "C:/X-Plane 12/Resources/plugins"
   ```

## Customization Guide

To customize the `XPlaneImGui.cpp` file for your own use, follow these steps:

### Customize Plugin Information

- **Modify the `XPluginStart` function** to change the plugin's name, signature, and description to match your project.

### Initialize ImGui for X-Plane

- **Initialization and Shutdown**: Handled in `XPluginStart` and `XPluginStop`. No changes are needed unless you have additional initialization or cleanup requirements.

### Register and Unregister ImGui Draw Callbacks

- **In `XPluginEnable`**: Register your own render callback function instead of the examples.
- **In `XPluginDisable`**: Unregister your render callback function.

### Implement Custom Render Function

- **Replace `XPlanePluginIntroRender`** with your custom function. This function should contain ImGui calls to create your plugin's UI.

### Handle Plugin Messages

- **Implement logic in `XPluginReceiveMessage`** to handle any specific messages or actions your plugin needs to respond to.

### Include Necessary Headers

- **Ensure all necessary headers** for your custom functionality are included at the top of the file.

### Customize and Extend

- **Add any additional functions, global variables, or classes** needed for your plugin's functionality.

### Compile and Test

- **After making your changes**, compile your plugin and test it within X-Plane to ensure everything works as expected.