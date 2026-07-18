#pragma once

#include <string>
#include <ostream>
#include "Timestamp.hpp"

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

    Logger(const Logger&);
    Logger& operator=(const Logger&);
    void log(LogLevel level, const std::string& message, bool timestamp);
    void setstream(std::ostream& stream);

    static std::string make_errno_error(std::string const& ctx);
    static std::string make_error(std::string const& context, std::string const& message);
private:
    std::ostream* out;
    std::string level_string(LogLevel level);
   
    static std::string format_date(time_t raw);

};

}