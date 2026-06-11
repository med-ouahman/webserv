
#include "http/Context.hpp"
#include "http/Parser/body/body.hpp"

namespace http {
namespace parser {

usize	body_min_size(usize a, usize b) { return a < b ? a : b; }

static bool	is_hex(char c) {
	return (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

static usize	hex_value(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return c - 'A' + 10;
}

static bool	add_hex_digit(usize& size, char c) {
	usize digit = hex_value(c);
	usize max = static_cast<usize>(-1);

	if (size > (max - digit) / 16)
		return false;
	size = size * 16 + digit;
	return true;
}

bool	body_parse_chunk_size(const std::string& line, usize& size) {
	usize i;

	if (line.empty() || !is_hex(line[0]))
		return false;
	size = 0;
	for (i = 0; i < line.size() && line[i] != ';'; ++i) {
		if (!is_hex(line[i]))
			return false;
		if (!add_hex_digit(size, line[i]))
			return false;
	}
	return true;
}

}

Error ParserState::body_write(usize size) {
	base::Expected<usize, base::io::Error> written =
		body_writer.write(raw_buffer.data(), size);

	if (!written || written.value() != size)
		return ERR_INTERNAL;
	raw_buffer.erase(0, size);
	body_received += size;
	return parser::check_body_size(body_received);
}

}
