#pragma once

struct limits {

	enum {
        SEND_CHUNK_SIZE            = 16384,
        MAX_REQUESTS               = 100,
        MIN_BODY_PROGRESS_BYTES         = 4096,
        MIN_BODY_CHUNK             = 4096,
		MAX_CONNECTIONS		= 1000,
		MAX_EVENTS			= 128,
		MAX_CONCURRENT_CGI	= 100,
		MAX_TIMEOUT_MS		= 60,
		MAX_HEADER_TIMEOUT	= 10,
		MAX_BODY_PROGRESS_TIMEOUT = 30,
		MAX_IDLE_TIMEOUT = 60,
	};

};
