#include "ResponseParser.hpp"
#include <iostream>
#include "http/Parser/parser.hpp"
#include <cstdlib>

namespace http {

ResponseParser::ResponseParser()
    : code(OK), headers_(), parse_ctx() {}

ResponseParser::~ResponseParser() {}

ParseResult ResponseParser::parse_headers(BufferReader& reader) {
    std::cout << "Start CGI header parsing...\n";

    while (parse_ctx.state_ != CGIParseContext::Done) {

        size_t max_scan_size = CGIParseContext::MaxHeaderBlockLen - parse_ctx.header_bytes;

        parse_ctx.scanner.reset();
        
        ScanResult r = parse_ctx.scanner.scan(reader, max_scan_size);
        
        if (r == LIMIT_EXCEEDED) return ParseError;
        
        if (r == NEED_MORE) {
            return Continue;
        } 
            
        
        if (parse_ctx.scanner.line().empty()) {
            parse_ctx.state_ = CGIParseContext::Done;
            break;
        }

        parse_ctx.header_bytes += parse_ctx.scanner.line().size();
        
        ParseResult res = parse_header(parse_ctx.scanner.line());
        if (res != Success) return res;

    }

    std::cout << "finish CGI Header parsing\n";
    return Success;
}

ParseResult ResponseParser::sanitize_status_header(std::string const& value) {

    size_t space_pos = value.find(' ');

    if (space_pos == std::string::npos) {
        code = INTERNAL_SERVER_ERROR;
        reason = "Internal server error";
        return ParseError;
    }

    std::string code_str = value.substr(0, space_pos);
    std::string reason   = value.substr(space_pos + 1);

    for (size_t i = 0; i < code_str.size(); ++i) {
        if (!std::isdigit(code_str[i])) {
            code = INTERNAL_SERVER_ERROR;
            reason = "Invalid status code";
            return ParseError;
        }
    }

    int code = std::atoi(code_str.c_str());

    if (code < 200 or code > 599) {
        code = INTERNAL_SERVER_ERROR;
        reason = "Invalid status code";
        return ParseError;
    }

    return Success;
}

const Headers& ResponseParser::headers() const {
    return headers_;
}

http::StatusCode ResponseParser::status_code() const {
    return code;
}

bool ResponseParser::finished() const {
    return parse_ctx.state_ == CGIParseContext::Done;
}

ParseResult ResponseParser::parse_header(std::string const& line) {

    std::cout <<  line << "|\n";

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

}