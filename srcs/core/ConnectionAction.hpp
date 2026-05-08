#pragma once

namespace core {
	
	struct ConnectionAction {
		bool want_read;
		bool want_write;
		bool want_close;
		bool want_you;

		ConnectionAction(): want_read(false), want_write(false), want_close(false), want_you(false) {

		}
	};
}
