#pragma once

#include <unistd.h>
#include <vector>
#include "Pipe.hpp"
#include "Stream.hpp"
#include "Timestamp.hpp"
#include "CStringArray.hpp"

namespace cgi {

struct CGIExecContext;

class Process {
private:
    enum ProcessState {
        Spawn,
        Running,
        Error
    } state_;

    static time_t timeout_secs;
    pid_t     pid;
    int       status;
    
    Pipe     stdin_pipe_;
    Pipe     stdout_pipe_;
    Pipe     stderr_pipe_;
    
    Timestamp spawn_time;
    Timestamp sigterm_sent_at;

    Process(const Process&);
    Process& operator=(const Process&);

public:
    bool running() const;
    Process(const CGIExecContext& ctx);
    ~Process();
    bool timedout();
    Pipe& stdin_pipe();
    Pipe& stdout_pipe();
    Pipe& stderr_pipe();
};

}