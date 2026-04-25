#include "MimeType.hpp"

namespace http {

	const std::string MimeType::default_mime_type = "application/octet-stream";
	
	const std::string MimeType::get( const std::string& ext ) {
		return mime_types[ext].size() == 0 ? default_mime_type: mime_types[ext];
	}

}
