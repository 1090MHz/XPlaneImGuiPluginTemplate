# XPlaneImGuiPluginTemplate

A lightweight X-Plane plugin that integrates ImGui for enhanced in-sim user interfaces.

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

- **Replace `XPlaneImGuiIntroRender`** with your custom function. This function should contain ImGui calls to create your plugin's UI.

### Handle Plugin Messages

- **Implement logic in `XPluginReceiveMessage`** to handle any specific messages or actions your plugin needs to respond to.

### Include Necessary Headers

- **Ensure all necessary headers** for your custom functionality are included at the top of the file.

### Customize and Extend

- **Add any additional functions, global variables, or classes** needed for your plugin's functionality.

### Compile and Test

- **After making your changes**, compile your plugin and test it within X-Plane to ensure everything works as expected.