#pragma once

namespace core {
	struct ConnectionAction {
		bool want_write;
		bool want_read;
		bool want_close;
		bool want_process;
		bool want_cgi;
	};
}
