#pragma once

#include <string>
#include <ostream>

namespace logger {

enum LogLevel {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
};

class Logger {

private:
    std::ostream& out_;
    void log_info(LogLevel level);
    void log_warn(LogLevel level);
    void log_debug(LogLevel level);
    void log_error(LogLevel level);
    std::string level_string(LogLevel level);
    std::string format(LogLevel level, const std::string& message);
    
public:
    Logger(std::ostream& out);
    ~Logger();
    void log(LogLevel level, const std::string& message);

};


}