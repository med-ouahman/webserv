#pragma once

#include "IOChannel.hpp"
#include <unistd.h>

namespace cgi {

    class CGIProcess {

        private:
        	pid_t	cgi_pid;
			int		cgi_status;
			PipeGuard pipe_guard;
            IOChannel stdin_;
            IOChannel stdout_;
            IOChannel stderr_;

            Timestamp start_time;
			Timestamp sigterm_sent_at;

			static size_t cgi_timeout_secs;

            CGIProcess( const CGIProcess& );
            CGIProcess& operator=( const CGIProcess& );

        public:
            void spawn( const CGIContext& ctx );

    };
}