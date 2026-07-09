#include "Logger.hpp"
#include <iostream>
#include <iomanip>

namespace logger {

Logger::Logger(): out(&std::cout) {}

Logger::Logger(std::ostream& s): out(&s) {}

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

void Logger::log(LogLevel level, const std::string& message, bool timestamp) {
    const std::string ts =
        timestamp ? format_date(Timestamp::now().seconds()) : "";

    (*out)
        << std::left
        << std::setw(10) << level_string(level)
        << std::setw(30) << message
        << std::right
        << std::setw(50) << ts
        << '\n';
}

void Logger::setstream(std::ostream& stream) {
    out = &stream;
}

}