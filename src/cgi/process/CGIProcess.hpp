#pragma once

#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"

namespace cgi {

    class CGIProcess {

        private:
        	pid_t       pid;
			int         status;
			PipeGuard   pipe_guard;
            io::Stream  stdin_;
            io::Stream  stdout_;
            io::Stream  stderr_;

            Timestamp start_time;
			Timestamp sigterm_sent_at;

			static time_t timeout_secs;

            CGIProcess( const CGIProcess& );
            CGIProcess& operator=( const CGIProcess& );

        public:
            void spawn( const CGIContext& ctx );

            void on_stdout_readable();
            void on_stdin_writeable();
            void on_stderr_readable();
            void on_error();

            io::Stream const& stdin() const;
            io::Stream const& stdout() const;
            io::Stream const& stderr() const;
    };
}