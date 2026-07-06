#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <string>

class Logger {
public:
    static std::string get_log_directory();
    /**
     * @brief Clears application log files while keeping the log directory in place.
     * @param error Optional output for a user-facing failure reason.
     * @return True when the log directory was cleared or was already empty.
     */
    static bool clear_logs(std::string* error = nullptr);
    static void setup_loggers();
    static std::shared_ptr<spdlog::logger> get_logger(const std::string &name);
    static std::string get_log_file_path(const std::string &log_dir, const std::string &log_name);

private:
    static std::string get_xdg_cache_home();
    static std::string get_windows_log_directory();
    Logger() = delete;
};

#endif
