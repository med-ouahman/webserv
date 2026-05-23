#pragma once

<<<<<<< HEAD
#include "Pipe.hpp"
#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"
#include <vector>
#include "CStringArray.hpp"
#include "Fd.hpp"
=======
#include "PipeGuard.hpp"
#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"
>>>>>>> 2a4fb87 (s)

namespace cgi {

    struct CGIExecContext {

        std::string interpreter_path;
        std::string working_dir;
<<<<<<< HEAD
        Fd          stdin_fd;
        uint32_t    timeout_seconds;
        CStringArray argv;
        CStringArray envp;
        
        CGIExecContext() {

        }
    };

=======
        int         stdin_fd;

        uint32_t timeout_seconds;

        char* const* envp;
    };


>>>>>>> 2a4fb87 (s)
    class CGIProcess {

        private:
        	pid_t       pid;
			int         status;
<<<<<<< HEAD
            Pipe     stdout_pipe_;
            Pipe     stderr_pipe_;
=======
			PipeGuard   pipe_guard;
            io::Stream  stdout_;
            io::Stream  stderr_;
>>>>>>> 2a4fb87 (s)

            Timestamp spawn_time;
			Timestamp sigterm_sent_at;

			static time_t timeout_secs;

            CGIProcess( const CGIProcess& );
            CGIProcess& operator=( const CGIProcess& );

        public:
            CGIProcess( const CGIExecContext& ctx );
<<<<<<< HEAD

            bool timedout();
            Pipe& stdout_pipe() const;
            Pipe& stderr_pipe() const;
            
        private:
            enum ProcessState {
                SPAWN,
                RUNNING,
                ERROR
            } state;
=======
            void on_stdout_readable();
            void on_stdin_writeable();
            void on_stderr_readable();
            void on_error();
            bool timedout();
            
            io::Stream const& stdout() const;
            io::Stream const& stderr() const;
>>>>>>> 2a4fb87 (s)
    };
}