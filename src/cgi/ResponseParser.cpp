#include "ResponseParser.hpp"
#include <iostream>
#include "http/Parser/Parser.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

namespace http {

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
    std::cout << "Start CGI header parsing...\n";

    if (reader.empty() && state_ == Headers) {        
        return ParseError;
    }

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
            std::cout << "Begin body reading\n";
            state_ = Body;
            continue;
        }

        parse_ctx.header_bytes += parse_ctx.line_reader.line().size();
        
        ParseResult res = parse_header(parse_ctx.line_reader.line());
        if (res != Success) {
            if (res == ParseError) std::cout << "stupid nigga\n";
            return res;
        }

        parse_ctx.line_reader.reset();
    }

    std::cout << "finish CGI\n";
    return Success;
}

ResponseParser::ParseResult ResponseParser::sanitize_status_header(std::string const& value) {

    std::cout << "Saniting status header\n";
    size_t space_pos = value.find(' ');

    if (space_pos == std::string::npos) {
        code = INTERNAL_SERVER_ERROR;
        std::cout << "Here\n";
        return ParseError;
    }

    std::string code_str = value.substr(0, space_pos);
    std::string reason   = value.substr(space_pos + 1);

    for (size_t i = 0; i < code_str.size(); ++i) {
        if (!std::isdigit(code_str[i])) {
            std::cout << "may be here??\n";
            code = INTERNAL_SERVER_ERROR;
            return ParseError;
        }
    }

    char* end = NULL;

    int parsed_code = std::strtol(code_str.c_str(), &end, 10);

    if ((end && *end != '\0') || parsed_code < 200 or parsed_code > 599) {
        code = INTERNAL_SERVER_ERROR;
        std::cout << "Or here?\n";
        return ParseError;
    }


    code = static_cast<http::StatusCode>(parsed_code);
    return Success;
}

bool ResponseParser::finished() const {
    return state_ == Done;
}

ResponseParser::ParseResult ResponseParser::parse_header(std::string const& line) {
    std::cout << "|" << line << "|\n";

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

    if ("status" == name)
        return sanitize_status_header(value);

    headers_.add(name, value);

    return Success;
}
ResponseParser::ParseResult ResponseParser::read_body(BufferView& reader) {

    std::cout.write(reader.data(), reader.remaining());

    if (reader.empty()) {
        std::cout << "Body Done\n";
        state_ = Done;
        return Success;
    }

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
        body_filename = "/tmp/" + base::random_string(10);
        body_fd = ::open(body_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (body_fd < 0) {
            state_ = Error;
            std::cout << "File error\n";
            return ParseError;
        }
        if (!body_.empty()) {
            ssize_t w = ::write(body_fd, body_.c_str(), body_.size());
            if (w < 0) {
                std::cout << "write error\n";
                state_ = Error;
                return ParseError;
            }
            body_content_length += w;
            body_.clear();
        }
    }

    ssize_t w = ::write(body_fd, reader.data(), reader.remaining());
    if (w < 0) {
        std::cout << "antoher write error\n";
        state_ = Error;
        return ParseError;
    }

    body_content_length += w;
    reader.advance(w);

    return Continue;
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

    if (headers_.get("content-type").empty() && headers_.get("location").empty()) return false;

    return true;
}

}