
#pragma once

#include <string>
#include <map>

/* this class maps extensions to their mime type
	e.g .html -> text/html
*/

namespace http {
	class MimeType {

		private:
			static std::map<std::string, std::string> mime_types;
			const static std::string default_mime_type;

		public:
			const static std::string get( const std::string& ext );
	};
}
