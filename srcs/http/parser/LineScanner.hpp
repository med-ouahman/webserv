#pragma once

#include <string>

struct DataView {
	char* data;
	size_t len_;
};

namespace http {
	
	enum ScanResult {
		SUCCESS,
		NEED_MORE,
		ERROR,
	};

	class LineScanner {
		private:
			std::string linebuff;
			size_t bytes_consumed;
			DataView view;
			bool cr_found;

		public:
			void reset();
			std::string const& line();
			ScanResult scan( size_t max_block_len );
			DataView& get_view() { return view; };
			bool empty() { return bytes_consumed == view.len_;};
			size_t get_bytes_consumed() { return bytes_consumed; };
			LineScanner();
			~LineScanner();
	};
}
