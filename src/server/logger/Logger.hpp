#pragma once

#include <string>
#include <ostream>
#include "Timestamp.hpp"
#include <fstream>

namespace logger {

enum LogLevel {
    Info,
    Debug,
    Warning,
    Error,
};

class Logger {

public:
    explicit Logger(const std::string& errorlog);
    ~Logger();

    Logger(const Logger&);
    Logger& operator=(const Logger&);
    
    void log(LogLevel level, const std::string& message, bool timestamp = true);

    void log_cstr(LogLevel level, const char* message, size_t size, bool timestamp = true);
    
    void setstream(std::ostream& stream) const;

    static std::string make_errno_error(std::string const& ctx, const char* file, int line);
    static std::string make_error(std::string const& context, std::string const& message, const char* file, int line);
    
private:
    mutable std::ostream* out;
    std::ofstream error_log_;

    std::string level_string(LogLevel level);
    static std::string format_date(time_t raw);

};

}