
#include "CGIHandler.hpp"

namespace http {

	ReadFd CGIHandler::get_stdout_pipe( void ) const {
		return ReadFd(pipe_stdout);
	}

	ReadFd CGIHandler::get_stderr_pipe( void ) const {
		return ReadFd(pipe_stderr);
	}

	WriteFd CGIHandler::get_stdin_pipe( void ) const {
		return WriteFd(pipe_stdin);
	}

}
