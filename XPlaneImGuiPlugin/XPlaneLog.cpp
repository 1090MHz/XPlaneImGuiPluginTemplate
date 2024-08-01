#include "XPlaneLog.h"

// Standard Library Headers
#include <filesystem> // For handling file system paths
#include <memory>     // For std::shared_ptr and std::make_shared
#include <mutex>      // For std::mutex used in custom sink
#include <string>     // For std::string

// Third-Party Library Headers
#include <spdlog/details/log_msg.h>          // For spdlog::details::log_msg
#include <spdlog/sinks/basic_file_sink.h>    // For spdlog::sinks::basic_file_sink_mt
#include <spdlog/sinks/stdout_color_sinks.h> // For spdlog::sinks::stdout_color_sink_mt
#include <spdlog/spdlog.h>                   // For spdlog logging functions

// X-Plane SDK Headers
#include "XPLMUtilities.h" // For XPLMDebugString and XPLMGetPluginInfo
#include "XPLMPlugin.h"    // For XPLMGetMyID

void XPlaneLog::init(const std::string &plugin_name)
{
    // Create an XPlaneLog::Sink instance
    auto xplane_sink = std::make_shared<Sink>();

    // Optionally, create other sinks (e.g., console and file sinks)
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // Determine the plugin's directory
    char pluginPath[512];
    XPLMGetPluginInfo(XPLMGetMyID(), nullptr, pluginPath, nullptr, nullptr);

    // Construct the path to Log.txt within the plugin's directory
    std::filesystem::path path(pluginPath);
    // std::filesystem::path logFileName = "Log.txt";

    // Sanitize plugin_name to be a valid filename
    std::string sanitized_plugin_name = plugin_name;
    std::replace_if(sanitized_plugin_name.begin(), sanitized_plugin_name.end(), [](char c)
                    { return !std::isalnum(c) && c != '_'; }, '_');

    std::filesystem::path logFileName = sanitized_plugin_name + ".log";
    std::filesystem::path logFilePath = path.parent_path() / logFileName;

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);

    spdlog::sinks_init_list sink_list = {console_sink, file_sink, xplane_sink};
    auto logger = std::make_shared<spdlog::logger>("XPlaneLogger", sink_list.begin(), sink_list.end());

    // Provide a custom format for the logger
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [" + plugin_name + "] [%^%l%$] %v");

    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::flush_on(spdlog::level::info);   // Flush logs on info level and higher
}

void XPlaneLog::trace(const std::string &message)
{
    spdlog::trace(message);
}

void XPlaneLog::debug(const std::string &message)
{
    spdlog::debug(message);
}

void XPlaneLog::info(const std::string &message)
{
    spdlog::info(message);
}

void XPlaneLog::warn(const std::string &message)
{
    spdlog::warn(message);
}

void XPlaneLog::error(const std::string &message)
{
    spdlog::error(message);
}

void XPlaneLog::critical(const std::string &message)
{
    spdlog::critical(message);
}

// Implementation of the custom sink for X-Plane
void XPlaneLog::Sink::sink_it_(const spdlog::details::log_msg &msg)
{
    // Format the message
    spdlog::memory_buf_t formatted;
    base_sink<std::mutex>::formatter_->format(msg, formatted);

    // Write the message to X-Plane Log.txt using XPLMDebugString
    XPLMDebugString(fmt::to_string(formatted).c_str());
}

void XPlaneLog::Sink::flush_()
{
    // Flush function can be empty as XPLMDebugString doesn't have a corresponding flush function
}