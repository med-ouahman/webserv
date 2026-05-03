#pragma once

#include <stdio.h>

namespace core {
	struct DataView;
}

namespace http {
	class IOChannel;
}

namespace io {
	class IDataListener {

		public:
			IDataListener() {};
			virtual ~IDataListener() {};
			virtual void on_input_ready( core::DataView* ) = 0;
			virtual ssize_t produce_output( char* buff, size_t size ) = 0;
			virtual size_t consumed_data() = 0;
			virtual void on_error() = 0;
			virtual void on_channel_closed() = 0;
	};
}
