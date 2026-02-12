#pragma once

namespace core {
	class Connection;
}

namespace io {
	struct FdContext {
		int fd;
		core::Connection* conn;
	};
}
