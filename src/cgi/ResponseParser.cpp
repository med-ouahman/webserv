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
    parse_ctx() {}

ResponseParser::~ResponseParser() {}

ResponseParser::ParseResult ResponseParser::parse(BufferView& reader) {
    std::cout << "Start CGI header parsing...\n";

    if (reader.empty() && state_ == Headers) {
        return ParseError;
    }

    while (state_ != Done) {

        size_t max_scan_size = CGIParseContext::MaxHeaderBlockLen - parse_ctx.header_bytes;
        
        ReadResult r = parse_ctx.line_reader.readline(reader, max_scan_size);
        reader.advance(parse_ctx.line_reader.bytes_read());
        
        switch (r) {
            case LIMIT_EXCEEDED: return ParseError;
            case NEED_MORE: return Continue;
            case SUCCESS: break;
        }
        
        if (parse_ctx.line_reader.line().empty()) {
            state_ = Done;
            break;
        }

        parse_ctx.header_bytes += parse_ctx.line_reader.line().size();
        
        ParseResult res = parse_header(parse_ctx.line_reader.line());
        if (res != Success) return res;

        parse_ctx.line_reader.reset();
    }

    std::cout << "finish CGI\n";

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

    int code = std::strtol(code_str.c_str(), &end, 10);

    if ((end && *end != '\0') || code < 200 or code > 599) {
        code = INTERNAL_SERVER_ERROR;
        return ParseError;
    }

    return Success;
}

bool ResponseParser::finished() const {
    return state_ == Done;
}

ResponseParser::ParseResult ResponseParser::parse_header(std::string const& line) {

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


CGIResult ResponseParser::result() const {
    
    return CGIResult(
        code,
        headers_);
}

}