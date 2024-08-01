#ifndef XPLANELOG_H
#define XPLANELOG_H

#include <string>
#include <memory>
#include <mutex>

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/log_msg.h>

class XPlaneLog
{
public:
    // Initialize the logger
    static void init(const std::string &plugin_name);

    // Log a trace message
    static void trace(const std::string &message);

    // Log a debug message
    static void debug(const std::string &message);

    // Log an info message
    static void info(const std::string &message);

    // Log a warning message
    static void warn(const std::string &message);

    // Log an error message
    static void error(const std::string &message);

    // Log a critical message
    static void critical(const std::string &message);

private:
    // Custom sink for X-Plane
    class Sink : public spdlog::sinks::base_sink<std::mutex>
    {
    protected:
        void sink_it_(const spdlog::details::log_msg &msg) override;
        void flush_() override;
    };

    static std::shared_ptr<spdlog::logger> logger;
};

#endif // XPLANELOG_H