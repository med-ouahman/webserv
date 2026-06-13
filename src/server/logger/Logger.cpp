#include "Logger.hpp"
#include <iostream>

namespace logger {

Logger::Logger(): out(&std::cout) {}

Logger::Logger(std::ostream& s): out(&s) {}

Logger::~Logger() {}

std::string Logger::level_string(LogLevel level) {

    switch (level) {
        case Info: return "INFO";
        case Debug: return "DEBUG";
        case Warning: return "WARNING";
        case Error: return "ERROR";
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

    return std::string(buffer);
}

void Logger::log(LogLevel level, std::string const& message, bool timestamp) {

    *(out)
    << "[" 
    << level_string(level) 
    << "]"
    << "\t"
    << message
    << "\t"
    << (timestamp ? format_date(Timestamp::now().seconds()) : "")
    << "\n";
}

void Logger::setstream(std::ostream& stream) {
    out = &stream;
}

}