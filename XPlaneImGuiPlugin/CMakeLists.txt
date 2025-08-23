cmake_minimum_required(VERSION 3.15)
project(XPlaneImGuiPlugin LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    imgui_impl_xplane.cpp
    MenuHandler.cpp
    XPlaneImGui.cpp
    XPlaneLog.cpp
    ../imgui/backends/imgui_impl_opengl3.cpp
    ../imgui/imgui.cpp
    ../imgui/imgui_demo.cpp
    ../imgui/imgui_draw.cpp
    ../imgui/imgui_tables.cpp
    ../imgui/imgui_widgets.cpp
)

# Header files
set(HEADERS
    imgui_impl_xplane.h
    MenuHandler.h
    XPlaneLog.h
    ../imgui/imgui.h
    ../imgui/backends/imgui_impl_opengl3.h
)

# Include directories
include_directories(
    ../imgui
    ../spdlog/include
    ../IconFontCppHeaders
)

# Add X-Plane SDK if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libs/XPSDK410/CHeaders/XPLM")
    include_directories(../third_party/libs/XPSDK410/CHeaders/XPLM)
endif()

# X-Plane SDK version definitions (needed for all platforms)
add_definitions(-DXPLM400 -DXPLM302 -DXPLM301 -DXPLM300 -DXPLM210 -DXPLM200)

# Platform-specific settings
if(WIN32)
    # Add Windows-specific settings here
    add_definitions(-DIBM=1)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libs/XPSDK410/Libraries/Win")
        link_directories(../third_party/libs/XPSDK410/Libraries/Win)
        set(EXTRA_LIBS OpenGL32 XPLM_64 XPWidgets_64)
    else()
        set(EXTRA_LIBS OpenGL32)
    endif()
elseif(APPLE)
    # Add macOS-specific settings here
    add_definitions(-DAPL=1)
    find_library(COCOA_LIBRARY Cocoa)
    set(EXTRA_LIBS ${COCOA_LIBRARY})
elseif(UNIX)
    # Add Linux-specific settings here
    add_definitions(-DLIN=1)
    find_package(OpenGL REQUIRED)
    set(EXTRA_LIBS ${OPENGL_LIBRARIES})
endif()

# Create shared library (X-Plane plugin)
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})

# Set output name to .xpl for X-Plane plugin
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    SUFFIX ".xpl"
)

# Install rules (following CMake standards)
# Set platform-specific destination directory
if(WIN32)
    set(PLUGIN_ARCH_DIR "win_x64")
    set(LIBCURL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libs/curl/bin/libcurl-x64.dll")
elseif(APPLE)
    set(PLUGIN_ARCH_DIR "mac_x64")
    set(LIBCURL_FILE "")  # macOS typically uses system libcurl
elseif(UNIX)
    set(PLUGIN_ARCH_DIR "lin_x64")
    set(LIBCURL_FILE "")  # Linux typically uses system libcurl
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "${PLUGIN_ARCH_DIR}"
    LIBRARY DESTINATION "${PLUGIN_ARCH_DIR}"
)

# Install dependencies (if they exist)
if(LIBCURL_FILE AND EXISTS "${LIBCURL_FILE}")
    install(FILES "${LIBCURL_FILE}"
        DESTINATION "${PLUGIN_ARCH_DIR}"
    )
endif()

# Install font directories
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fonts/open-sans"
    DESTINATION "third_party/fonts"
    OPTIONAL
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/fonts/droid-sans-mono"
    DESTINATION "third_party/fonts"
    OPTIONAL
)

# Custom install target that uses XPLANE_DIR if provided
if(DEFINED XPLANE_DIR)
    add_custom_target(install-xplane
        COMMAND ${CMAKE_COMMAND} --install . --prefix "${XPLANE_DIR}/Resources/plugins/${PROJECT_NAME}" --config $<CONFIG>
        COMMENT "Installing plugin to X-Plane: ${XPLANE_DIR}/Resources/plugins/${PROJECT_NAME}"
        USES_TERMINAL
    )
    message(STATUS "XPLANE_DIR set to: ${XPLANE_DIR}")
    message(STATUS "Use 'cmake --build . --target install-xplane' to install to X-Plane")
endif()

