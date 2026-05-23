#pragma once

#include "Pipe.hpp"
#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"
#include <vector>
#include "CStringArray.hpp"
#include "Fd.hpp"

namespace cgi {

    struct CGIExecContext {

        std::string interpreter_path;
        std::string working_dir;
        Fd          stdin_fd;
        uint32_t    timeout_seconds;
        CStringArray argv;
        CStringArray envp;
        
        CGIExecContext() {

        }
    };

    class CGIProcess {

        private:
        	pid_t       pid;
			int         status;
            Pipe     stdout_pipe_;
            Pipe     stderr_pipe_;

            Timestamp spawn_time;
			Timestamp sigterm_sent_at;

			static time_t timeout_secs;

            CGIProcess( const CGIProcess& );
            CGIProcess& operator=( const CGIProcess& );

        public:
            CGIProcess( const CGIExecContext& ctx );

            bool timedout();
            Pipe& stdout_pipe() const;
            Pipe& stderr_pipe() const;
            
        private:
            enum ProcessState {
                SPAWN,
                RUNNING,
                ERROR
            } state;
    };
}