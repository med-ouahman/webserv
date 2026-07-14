
#include "http/Context.hpp"
#include "http/Parser/Parser.hpp"

namespace http {

usize Parser::minSize(usize a, usize b) { return a < b ? a : b; }

static bool	isHex(char c) {
	return (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

static usize	hexValue(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return c - 'A' + 10;
}

static bool	addHexDigit(usize& size, char c) {
	usize digit = hexValue(c);
	usize max = static_cast<usize>(-1);

	if (size > (max - digit) / 16)
		return false;
	size = size * 16 + digit;
	return true;
}

bool Parser::parseChunkSize(const std::string& line, usize& size) {
	usize i;

	if (line.empty() || !isHex(line[0]))
		return false;
	size = 0;
	for (i = 0; i < line.size() && line[i] != ';'; ++i) {
		if (!isHex(line[i]))
			return false;
		if (!addHexDigit(size, line[i]))
			return false;
	}
	return true;
}

Error Parser::bodyWrite(usize size) {
	if (body_received > max_body_size
		|| size > max_body_size - body_received)
		return ERR_BODY_TOO_LARGE;
	base::Expected<usize, base::io::Error> written =
		bodyWriter.write(raw_buffer.data(), size);

	if (!written || written.value() != size)
		return ERR_INTERNAL;
	raw_buffer.erase(0, size);
	body_received += size;
	timer.update();
	return ERR_NONE;
}

}
