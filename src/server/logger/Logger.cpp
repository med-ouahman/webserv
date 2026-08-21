#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <sstream>

namespace logger {

Logger::Logger(): loggin_enabled(true), out(&std::cout) {}

Logger::Logger(std::ostream& s): loggin_enabled(true), out(&s) {}

Logger::~Logger() {}

std::string Logger::level_string(LogLevel level) {

    switch (level) {
        case Info: return "[INFO]";
        case Debug: return "[DEBUG]";
        case Warning: return "[WARNING]";
        case Error: return "[ERROR]";
    }

    return "DEBUG";
}

const char* level_color(LogLevel level) {
    switch (level) {
        case Debug:   return "\033[36m"; // Cyan
        case Info:    return "\033[32m"; // Green
        case Warning: return "\033[33m"; // Yellow
        case Error:   return "\033[31m"; // Red
        default:      return "\033[0m";  // Reset
    }
}

void Logger::log(LogLevel level, const std::string& message, bool timestamp)
{
    if (!loggin_enabled) return;

    const std::string ts =
        timestamp ? format_date(Timestamp::now().seconds()) : "";

    static const char* const reset = "\033[0m";

    (*out)
    << level_color(level)
    << std::left << std::setw(8) << level_string(level)
    << reset << "  "
    << std::left << std::setw(50) << message
    << "  " << ts
    << '\n';
}

std::string Logger::format_date(time_t raw) {
    char buffer[32];


    std::tm* tm = std::localtime(&raw);

    std::strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        tm
    );

    return "[ " + std::string(buffer) + " ]";
}

void Logger::setstream(std::ostream& stream) {
    out = &stream;
}

std::string Logger::make_errno_error(std::string const& ctx,  const char* file, int line) {
    std::stringstream ss;
    
    ss << ctx
    << " | code="
    << errno << " | "
    << strerror(errno) << " | "
    << file << ":" << line;

    return ss.str();
}

std::string Logger::make_error(std::string const& context, std::string const& message, const char* file, int line) {
    std::stringstream ss;
    
    ss << context
    << " | "
    << message << " | "
    << file << ":" << line;

    return ss.str();
}

void Logger::disable() {
    loggin_enabled = false;
}

void Logger::enable() {
    loggin_enabled = true;
}

}