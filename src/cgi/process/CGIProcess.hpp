#pragma once

#include "Pipe.hpp"
#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"
#include <vector>
#include "CStringArray.hpp"

namespace cgi {

struct CGIExecContext;

class CGIProcess {
private:
    static time_t timeout_secs;
    pid_t     pid;
    int       status;
    
    Pipe     stdin_pipe_;
    Pipe     stdout_pipe_;
    Pipe     stderr_pipe_;
    
    Timestamp spawn_time;
    Timestamp sigterm_sent_at;

    CGIProcess( const CGIProcess& );
    CGIProcess& operator=( const CGIProcess& );

public:
    CGIProcess( const CGIResolver::Context& ctx );
    bool timedout();
    Pipe& stdin_pipe();
    Pipe& stdout_pipe();
    Pipe& stderr_pipe();
    
private:
    enum ProcessState {
        SPAWN,
        RUNNING,
        ERROR
    } state;
};

}