#include "ResponseParser.hpp"
#include "http/Parser/Parser.hpp"
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

namespace http {

namespace {

static bool writeAll(int fd, const char* data, size_t size) {
	size_t total = 0;

	while (total < size) {
		ssize_t written = ::write(fd, data + total, size - total);
		if (written < 0 && errno == EINTR)
			continue;
		if (written <= 0)
			return false;
		total += static_cast<size_t>(written);
	}
	return true;
}

static int createTempFile(std::string& path) {
	char pattern[] = "/tmp/webserv-cgi-XXXXXX";
	int fd = ::mkstemp(pattern);

	if (fd < 0)
		return -1;
	int flags = ::fcntl(fd, F_GETFD);
	if (flags < 0 || ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
		::close(fd);
		::unlink(pattern);
		return -1;
	}
	path = pattern;
	return fd;
}

}

ResponseParser::ResponseParser()
    : state_(Headers),
    code(OK),
    headers_(),
    parse_ctx(),
    body_fd(-1),
    body_filename(),
    body_content_length(0),
    body_(),
    body_mode_(Mem) {}

ResponseParser::~ResponseParser() {
    if (body_fd >= 0) {
        ::close(body_fd);
        body_fd = -1;
    }

    if (!body_filename.empty()) ::unlink(body_filename.c_str());
    
    body_filename.clear();
}

ResponseParser::ParseResult ResponseParser::parse(BufferView& reader) {
    while (state_ != Done) {

        if (state_ == Body) return read_body(reader);

        size_t max_scan_size = CGIParseContext::MaxHeaderBlockLen - parse_ctx.header_bytes;
        
        ReadResult r = parse_ctx.line_reader.readline(reader, max_scan_size);
        reader.advance(parse_ctx.line_reader.bytes_read());
        
        switch (r) {
            case LIMIT_EXCEEDED: return ParseError;
            case NEED_MORE: return Continue;
            case SUCCESS: break;
        }
        
        if (parse_ctx.line_reader.line().empty()) {
            if (!validate_headers()) return ParseError;
            state_ = Body;
            continue;
        }

        parse_ctx.header_bytes += parse_ctx.line_reader.line().size();
        
        ParseResult res = parse_header(parse_ctx.line_reader.line());
        if (res != Success) {
            return res;
        }

        parse_ctx.line_reader.reset();
    }

    return Success;
}

ResponseParser::ParseResult ResponseParser::sanitize_status_header(std::string const& value) {

    size_t space_pos = value.find(' ');

    if (space_pos == std::string::npos) {
        code = INTERNAL_SERVER_ERROR;
        return ParseError;
    }

    std::string code_str = value.substr(0, space_pos);
    std::string reason   = value.substr(space_pos + 1);

    for (size_t i = 0; i < code_str.size(); ++i) {
        if (!std::isdigit(code_str[i])) {
            code = INTERNAL_SERVER_ERROR;
            return ParseError;
        }
    }

    char* end = NULL;

    int parsed_code = std::strtol(code_str.c_str(), &end, 10);

    if ((end && *end != '\0') || parsed_code < 200 or parsed_code > 599) {
        code = INTERNAL_SERVER_ERROR;
        return ParseError;
    }


    code = static_cast<http::StatusCode>(parsed_code);
    return Success;
}

bool ResponseParser::finished() const {
    return state_ == Done;
}

ResponseParser::ParseResult ResponseParser::parse_header(std::string const& line) {
    size_t colon = line.find(':');
    if (colon == std::string::npos)
        return ParseError;

    std::string name = base::toLowerCase(line.substr(0, colon));

    for (size_t i = 0; i < name.size(); ++i) {
        if (::isspace(static_cast<unsigned char>(name[i])))
            return ParseError;
    }

    size_t start = colon + 1;

    while (start < line.size() &&
           ::isspace(static_cast<unsigned char>(line[start]))) {
        ++start;
    }

    size_t end = line.size();

    while (end > start &&
           ::isspace(static_cast<unsigned char>(line[end - 1]))) {
        --end;
    }

    std::string value = line.substr(start, end - start);

    if ("status" == name) return sanitize_status_header(value);

    headers_.add(name, value);

    return Success;
}
ResponseParser::ParseResult ResponseParser::read_body(BufferView& reader) {

	if (reader.empty())
		return Continue;

    switch (body_mode_) {
        case Mem: {
            size_t estimate = body_.size() + reader.remaining();
            if (estimate > MaxBodyMemSize) {
                body_mode_ = Disk;
            } else {
                size_t prev = body_.size();
                body_.append(reader.data(), reader.remaining());
                reader.advance(body_.size() - prev);
                return Continue;
            }
        }

        case Disk:
            break;
    }

    if (body_fd < 0) {
		body_fd = createTempFile(body_filename);
        if (body_fd < 0) {
            state_ = Error;
            return ParseError;
        }
        if (!body_.empty()) {
            if (!writeAll(body_fd, body_.data(), body_.size())) {
                state_ = Error;
                return ParseError;
            }
			body_content_length += body_.size();
            body_.clear();
        }
    }

	size_t size = reader.remaining();
	if (!writeAll(body_fd, reader.data(), size)) {
        state_ = Error;
        return ParseError;
    }

	body_content_length += size;
	reader.advance(size);

    return Continue;
}

ResponseParser::ParseResult ResponseParser::finish() {
	if (state_ == Done)
		return Success;
	if (state_ != Body)
		return ParseError;
	state_ = Done;
	return Success;
}


CGIResult ResponseParser::result() const {

    if (body_mode_ == Mem) return CGIResult(body_, code, headers_);
    
    if (body_fd >= 0) {
        ::close(body_fd);
        body_fd = -1;
    }

    std::string temp = body_filename;
    body_filename.clear();
    
    return CGIResult(temp,
        body_content_length,
        code,
        headers_);
}

bool ResponseParser::validate_headers() const {

    if (headers_.get("content-type").empty()
        && headers_.get("location").empty()) return false;

    return true;
}

}
