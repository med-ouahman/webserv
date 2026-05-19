#pragma once

#include "Stream.hpp"
#include <unistd.h>
#include "Timestamp.hpp"
#include "EventLoop.hpp"

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

			static size_t cgi_timeout_secs;

            CGIProcess( const CGIProcess& );
            CGIProcess& operator=( const CGIProcess& );

        public:
            void spawn( const CGIContext& ctx, const io::EventLoop& loop );

            void on_stdout_readable();
            void on_stdin_writeable();
            void on_stderr_readable();
            void on_error();
    };
}