#pragma once

#include <string>
#include <ostream>
#include "Timestamp.hpp"

#define LOG(logger, level, msg) \
    (logger).log(level, __FILE__, __LINE__, msg)

namespace logger {

enum LogLevel {
    Info,
    Debug,
    Warning,
    Error,
};

class Logger {

public:
    explicit Logger(std::ostream& out);
    Logger();
    ~Logger();
    void log(LogLevel level, const std::string& message, bool timestamp);
    void setstream(std::ostream& stream);

private:
    std::ostream* out;
    std::string level_string(LogLevel level);
   
    static std::string format_date(time_t raw);

};

}