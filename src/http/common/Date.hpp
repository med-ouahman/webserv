#pragma once 

#include "Timestamp.hpp"
#include <iomanip>
#include <string>
#include <sstream>

namespace http {

class Date {
	
public:
	static std::string format(const Timestamp& ts) {
		struct tm f;
		char buf[100];
		const time_t s = ts.seconds();
		gmtime_r(&s, &f);
		std::string s;
		size_t w = strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &f);
		return std::string(buf, w);
	};

	static std::string now() {
		return format(Timestamp::now());
	}

	static std::string http_date_header_now() {
		return "Date: " + format(Timestamp::now()) + "\r\n";
	}

	static std::string http_date_header(const Timestamp& ts) {
		return "Date: " + format(ts) + "\r\n";
	}
};

}
