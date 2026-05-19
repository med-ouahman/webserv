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
			std::string linebuff;
			DataView& 	data_view;
			bool		cr_found;

		public:
			void reset();
			std::string const& line();
			ScanResult scan( size_t max_block_len );
			bool empty() { return data_view.empty(); };
			
			LineScanner( DataView& v );
			~LineScanner();
	};
}
