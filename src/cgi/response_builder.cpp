#include "ResponseParser.hpp"
#include <iostream>
#include "http/Parser/parser.hpp"
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
    body_content_length(0) {}

ResponseParser::~ResponseParser() {
    if (body_fd >= 0) ::close(body_fd);
    body_fd = -1;

    if (body_filename.empty() == false) ::unlink(body_filename.c_str());
    body_filename.clear();
}

ParseResult ResponseParser::parse_headers(BufferReader& reader) {
    // std::cout << "Start CGI header parsing...\n";

    while (state_ != Done) {

        if (state_ == Body) {

            ParseResult r = read_body(reader);
            
            if (r != Success) return r;

            continue;
        }
        
        size_t max_scan_size = CGIParseContext::MaxHeaderBlockLen - parse_ctx.header_bytes;
        parse_ctx.scanner.reset();
        
        ScanResult r = parse_ctx.scanner.scan(reader, max_scan_size);
        
        if (r == LIMIT_EXCEEDED) return ParseError;
        
        if (r == NEED_MORE) {
            std::cout << "Need more\n";
            return Continue;
        } 
        
        if (parse_ctx.scanner.line().empty()) {
            state_ = Body;
            continue;
        }

        parse_ctx.header_bytes += parse_ctx.scanner.line().size();
        
        ParseResult res = parse_header(parse_ctx.scanner.line());
        if (res != Success) return res;

    }

    // std::cout << "finish CGI\n";
    return Success;
}

ParseResult ResponseParser::sanitize_status_header(std::string const& value) {

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

    int code = std::atoi(code_str.c_str());

    if (code < 200 or code > 599) {
        code = INTERNAL_SERVER_ERROR;
        return ParseError;
    }

    return Success;
}

bool ResponseParser::finished() const {
    return state_ == Done;
}

ParseResult ResponseParser::parse_header(std::string const& line) {

    std::cout << "|" <<  line << "|\n";

    size_t colon = line.find(":");
    if (std::string::npos == colon) return ParseError;

    std::string name = line.substr(0, colon);
    
    for ( size_t i(0); i < name.size(); i++) {
        if (::isspace(name[i])) return ParseError;
    }

    size_t start = colon + 1;
    
    while (::isspace(line[start]) && start < line.size()) ++start;
    
    size_t end = line.size() - 1;

    while (end > start && ::isspace(line[end])) --end;
    
    std::string value = line.substr(start, end - start);

    if (name == "status" || name == "Status") return sanitize_status_header(value);

    headers_.add(name, value);
    
    return Success;
}

ParseResult ResponseParser::read_body(BufferReader& reader) {
    
    if (reader.size() == 0) {
        state_ = Done;
        return Success;
    }

    if (body_fd < 0) {
        
        body_filename = "/tmp/" + base::random_string(10);
        body_fd = ::open(body_filename.c_str(), O_WRONLY | O_CREAT, 0600);
        
        if (body_fd < 0) {
            state_ = Error;
            return ParseError;
        }
    }

    ssize_t w = ::write(body_fd, reader.data() + reader.cursor(), reader.remaining());
    
    if (w < 0) {
        state_ = Error;
        return ParseError;
    }

    body_content_length += w;
    reader.advance(w);
    return Continue;
}


CGIResult ResponseParser::result() const {

    ::close(body_fd);
    body_fd = -1;

    return CGIResult(body_filename,
        body_content_length,
        code,
        headers_);
}

}