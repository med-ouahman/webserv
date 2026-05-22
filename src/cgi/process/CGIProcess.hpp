#pragma once

#include "PipeSet.hpp"
#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"

namespace cgi {

    struct CGIExecContext {

        std::string interpreter_path;
        std::string working_dir;
        int         stdin_fd;
        uint32_t timeout_seconds;
        char* const* envp;
    };

    class CGIProcess {

        private:
        	pid_t       pid;
			int         status;
            PipeSet     stdout_set;
            PipeSet     stderr_set;
            io::Stream  stdout_;
            io::Stream  stderr_;

            Timestamp spawn_time;
			Timestamp sigterm_sent_at;

			static time_t timeout_secs;

            CGIProcess( const CGIProcess& );
            CGIProcess& operator=( const CGIProcess& );

        public:
            CGIProcess( const CGIExecContext& ctx );
            void on_stdout_readable();
            void on_stdin_writeable();
            void on_stderr_readable();
            void on_error();
            bool timedout();
            
            io::Stream const& stdout() const;
            io::Stream const& stderr() const;
    };
}