
#include "http/Context.hpp"
#include "http/Parser/Parser.hpp"

namespace http {

usize Parser::minSize(usize a, usize b) { return a < b ? a : b; }

static bool	addHexDigit(usize& size, char c) {
	usize digit = base::hexValue(c);
	usize max = static_cast<usize>(-1);

	if (size > (max - digit) / 16)
		return false;
	size = size * 16 + digit;
	return true;
}

Error Parser::parseChunkSize(const std::string& line, usize& size) {
	usize i;

	if (line.empty() || !base::isHex(line[0]))
		return ERR_BAD_REQUEST;
	size = 0;
	for (i = 0; i < line.size() && line[i] != ';'; ++i) {
		if (!base::isHex(line[i]) or !addHexDigit(size, line[i]))
			return ERR_BAD_REQUEST;
	}
	return ERR_NONE;
}

Error Parser::bodyWrite(BufferView& buff, usize size) {
	if (body_received > max_body_size
			|| size > max_body_size - body_received)
			return ERR_BODY_TOO_LARGE;

	base::Expected<usize, base::io::Error> written =
			bodyWriter.write(buff.data(), size);

	if (!written || written.value() != size)
			return ERR_INTERNAL;

	body_received += size;
	timer.update();
	return ERR_NONE;
}

}
