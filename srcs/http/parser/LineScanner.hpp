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
			core::DataView* 	data_view;
			bool				cr_found;

		public:
			void reset();
			std::string const& line();
			ScanResult scan( size_t max_block_len );
			void set_data_view( core::DataView* v ) { data_view = v; };
			bool empty() { return data_view->bytes_consumed == data_view->len_; };
			size_t get_bytes_consumed() { return data_view->bytes_consumed; };
			LineScanner();
			~LineScanner();
	};
}
