cmake_minimum_required(VERSION 3.15)
project(XPlaneImGuiPluginTemplate LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(XPlaneImGuiPlugin)

# Optional: Add other subdirectories if needed in the future
# add_subdirectory(tests)
# add_subdirectory(examples)
