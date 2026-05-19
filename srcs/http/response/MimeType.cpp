#include "MimeType.hpp"

namespace http {

	const std::string MimeType::default_mime_type = "application/octet-stream";
	std::map<std::string, std::string> MimeType::mime_types;
	
	const std::string MimeType::get( std::string& ext ) {
		return mime_types[ext].size() == 0 ? default_mime_type: mime_types[ext];
	}

}
