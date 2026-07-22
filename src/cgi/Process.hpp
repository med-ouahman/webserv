#pragma once

#include <unistd.h>
#include <vector>
#include "Pipe.hpp"
#include "Timestamp.hpp"
#include "CGIContext.hpp"

namespace cgi {

struct ProcessContext;

enum ProcessExitReason {
    Exited,
    Signaled,
    Stopped,
    Continued,
    Unknown,
};

struct ProcessResult {
	int status; // raw waitpid value
	ProcessExitReason reason;
};

class Process {
private:

enum ProcessState {
    Setup,
    Spawn,
    Running,
    Terminated,
    Error
} state_;

    pid_t     pid_;
    int       status_;
    
    Pipe     stdin_pipe_;
    Pipe     stdout_pipe_;
    Pipe     stderr_pipe_;

    Process(const Process&);
    Process& operator=(const Process&);

public:
    Process();
    ~Process();

    Pipe&   stdin_pipe();
    Pipe&   stdout_pipe();
    Pipe&   stderr_pipe();
    bool    want_stdin();

    bool    running() const;
    pid_t   pid() const;
    int     status() const;
    bool    reaped() const;
    bool    error() const;
    void    kill();
    void    terminate();
    void    reap();

    ProcessResult   result() const;
    static int      status_code(const ProcessResult& result);
    bool            start(const ProcessContext& ctx);
};

}