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
    std::string level_string(LogLevel level);
    std::string format(LogLevel level, const std::string& message);
    
public:
    Logger(std::ostream& out);
    ~Logger();
    void log(LogLevel level, const std::string& message);
};


}