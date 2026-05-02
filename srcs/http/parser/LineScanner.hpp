#pragma once

#include <string>
#include "DataView.hpp"

namespace http {
	
	enum ScanResult {
		SUCCESS,
		NEED_MORE,
		ERROR,
	};

	class LineScanner {
		private:
			std::string 		linebuff;
			core::DataView* 	view;
			bool				cr_found;

		public:
			void reset();
			std::string const& line();
			ScanResult scan( size_t max_block_len );
			void set_data_view( core::DataView* v ) { view = v; };
			bool empty() { return view->bytes_consumed == view->len_; };
			size_t get_bytes_consumed() { return view->bytes_consumed; };
			LineScanner();
			~LineScanner();
	};
}
