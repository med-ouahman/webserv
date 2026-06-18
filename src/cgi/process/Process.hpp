#pragma once

#include <unistd.h>
#include <vector>
#include "Pipe.hpp"
#include "Timestamp.hpp"
#include "CStringArray.hpp"

namespace cgi {

struct CGIExecContext;

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
    bool running() const;
    Process(const CGIExecContext& ctx);
    ~Process();

    Pipe& stdin_pipe();
    Pipe& stdout_pipe();
    Pipe& stderr_pipe();
    bool want_stdin();

    pid_t pid() const;
    int   status() const;
    bool reaped() const;

    void kill();
    void terminate();
    void poll();

    ProcessResult result() const;
    static int status_code(const ProcessResult& result); 
};

}