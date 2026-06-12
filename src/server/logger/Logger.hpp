#pragma once

#include <string>
#include <ostream>

namespace logger {

class Logger {
public:
    enum LogLevel {
        Info,
        Debug,
        Warning,
        Error,
    };

    Logger(std::ostream& out);
    ~Logger();
    void log(LogLevel level, const std::string& message);

private:
    std::ostream& out_;
    std::string level_string(LogLevel level);
    std::string format(LogLevel level, const std::string& message);

};

}