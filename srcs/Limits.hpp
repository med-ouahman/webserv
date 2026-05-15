#pragma once

struct Limits {

	enum {
        MAX_REQUESTS               = 100,
        MIN_BODY_PROGRESS_BYTES         = 4096,
        MIN_BODY_CHUNK             = 4096,
		MAX_CONNECTIONS		= 1000,
		MAX_EVENTS			= 128,
		MAX_CONCURRENT_CGIS	= 100,
		
		/*
			timeouts in seconds
			Used to prevent slowloris and DOS attacks
		*/

		MAX_CGI_WAIT_AFTER_SIGTERM = 2,
		MAX_INITIAL_TIMEOUT = 5,
		MAX_HEADER_TIMEOUT	= 10,
		MAX_BODY_PROGRESS_TIMEOUT = 30,
		MAX_IDLE_TIMEOUT = 60,
		EPOLL_WAIT_TIMEOUT_MS = 1000,
	};

};
