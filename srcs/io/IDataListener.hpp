#pragma once

#include <stdio.h>
#include "LineScanner.hpp"


class DataView;

class BufferWriter;


namespace http {
	class IOChannel;
}

namespace io {
	class IDataListener {

		public:
			IDataListener() {};
			virtual ~IDataListener() {};
			virtual http::ScanResult on_input_ready() = 0;
			virtual ssize_t produce_output( BufferWriter* writer ) = 0;
			virtual void on_error() = 0;
			virtual void on_channel_closed() = 0;
			virtual void on_ch_error() = 0;
	};
}
