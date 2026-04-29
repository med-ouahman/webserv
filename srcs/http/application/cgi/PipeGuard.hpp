#pragma once 

#include <unistd.h>
#include <stdexcept>

#define CLOSE_FD(FD) \
    do { \
        if (FD >= 0) { ::close(FD); FD = -1; } \
    } while (0) \

namespace http {
     struct PipeGuard {

        int stdin_pipe[2];
        int stdout_pipe[2];
        int stderr_pipe[2];

        PipeGuard() {    
            stdin_pipe[0] = stdin_pipe[1] = -1;
            stdout_pipe[0] = stdout_pipe[1] = -1;
            stderr_pipe[0] = stderr_pipe[1] = -1;
        
            if (::pipe(stdin_pipe) == -1 || ::pipe(stdout_pipe) == -1 || ::pipe(stderr_pipe) == -1) {
                close_pipes();
                throw std::runtime_error("some shit went wrong!");
            }
        }

        ~PipeGuard() {
            close_pipes();
        }

        void close_pipes( void ) {

            CLOSE_FD(stdin_pipe[0]);
            CLOSE_FD(stdin_pipe[1]);
            
            CLOSE_FD(stdout_pipe[0]);
            CLOSE_FD(stdout_pipe[1]);
            
            CLOSE_FD(stderr_pipe[0]);
            CLOSE_FD(stderr_pipe[1]);
        }

        private:
            PipeGuard( const PipeGuard& ) {};
            PipeGuard& operator=( const PipeGuard& ) { return *this; };
    };
}