cmake_minimum_required(VERSION 3.15)
project(XPlaneImGuiPluginTemplate LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure all targets are compiled with position-independent code so static
# libraries can be linked into the shared plugin (.xpl) on Linux.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add subdirectories
add_subdirectory(XPlaneUtilities)
add_subdirectory(XPlaneImGuiPlugin)

# Optional: Add other subdirectories if needed in the future
# add_subdirectory(tests)
# add_subdirectory(examples)
