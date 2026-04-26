#pragma once

#include "IOHandler.hpp"
#include <unistd.h>
#include <stdlib.h>

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}

struct WriteFd {
	int fd;

	WriteFd( int fd_ ): fd(fd_) {}
};

struct ReadFd {
	int fd;
	
	ReadFd( int fd_ ): fd(fd_) {}
};

namespace http {
	
	struct CGIContext;

	class CGIHandler: public io::IOHandler {
		
		public:
			explicit CGIHandler( const io::EventLoop& loop, const core::Connection& con );
			~CGIHandler();
			void on_event( io::EventType event );
			void spawn( const CGIContext& context );

		private:
			enum CGIState {
				SPAWN,
				ACTIVE,
				ERROR,
				IDLE,
			} cgi_state;

		private:
			pid_t	cgi_pid;
			int		cgi_status;

			int		pipe_stdin;
        	int 	pipe_stdout;
        	int 	pipe_stderr;
			
			const 	io::EventLoop& loop;
			const 	core::Connection& conn;

		private:
			bool create_pipes( void );
		
		private:
			static void close_pipes( int* pipefd );
		
		public:
			WriteFd get_stdin_pipe( void ) const;
			ReadFd	get_stdout_pipe( void ) const;
			ReadFd	get_stderr_pipe( void ) const;
	};

}
