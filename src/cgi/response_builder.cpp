#include "ResponseParser.hpp"
#include <iostream>
#include "http/parser/Parser.hpp"
#include <cstdlib>

namespace http {

ResponseParser::ResponseParser()
    : code(OK), headers_(), parse_ctx() {}

ResponseParser::~ResponseParser() {}

ParseResult ResponseParser::parse_headers(BufferReader& reader) {
    std::cout << "Start CGI header parsing...\n";
    while (parse_ctx.state_ != CGIParseContext::HEADERS_DONE) {
        
        size_t max_scan_size = CGIParseContext::MAX_CGI_HEADER_BLOCK_LEN - parse_ctx.header_bytes_;
        parse_ctx.sc_.reset();
        ScanResult r = parse_ctx.sc_.scan(reader, max_scan_size);
        
        if (r == LIMIT_EXCEEDED) return PARSE_ERROR;
        
        if (r == NEED_MORE) return PARSE_CONTINUE;
        
        if (parse_ctx.sc_.line().empty() && parse_ctx.state_ == CGIParseContext::HEADERS) {
            parse_ctx.state_ = CGIParseContext::HEADERS_DONE;
            break;
        }

        std::cout << parse_ctx.sc_.line() << "\n";
        
        base::Expected<std::pair<std::string, std::string>, int> header_result = parser::parse_header(parse_ctx.sc_.line());
        
        if (!header_result.has_value()) {
            std::cout  << "Error\n";
            return PARSE_ERROR;
        }
        
        std::cout << header_result.value().first << " | " << header_result.value().second << "\n";
        std::cout << "XXD Headers\n";
        sanitize_header(header_result.value());
    }

    std::cout << "finish CGI Header parsing\n";
    return PARSE_SUCCESS;
}

void ResponseParser::sanitize_status_line(const std::pair<std::string, std::string>& header) {

    std::cout << "Sanitizing the status line header\n";
    const std::string& value = header.second;

    size_t space_pos = value.find(' ');

    if (space_pos == std::string::npos) {
        parse_ctx.state_ = CGIParseContext::CGI_ERROR;
        return ;
    }

    std::string code_str = value.substr(0, space_pos);
    std::string reason   = value.substr(space_pos + 1);

    for (size_t i = 0; i < code_str.size(); ++i) {
        if (!std::isdigit(code_str[i])) {
            parse_ctx.state_ = CGIParseContext::CGI_ERROR;
            return;
        }
    }

    int code = std::atoi(code_str.c_str());

    if (code < 200 or code > 599) {
        return ;
    }

    parse_ctx.state_ = CGIParseContext::HEADERS;
    std::cout << "Done\n";
}

void ResponseParser::sanitize_header(std::pair<std::string, std::string>& header) {
    if (header.first == "status") {
        sanitize_status_line(header);
        return;
    }
    std::cout << "Sanitizing CGI headers\n";
    parser::capitalize_header_name(header.first);
    headers_.add(header.first, header.second);
}

const Headers& ResponseParser::headers() const {
    return headers_;
}

http::StatusCode ResponseParser::status_code() const {
    return code;
}

bool ResponseParser::finished() const {
    return parse_ctx.state_ == CGIParseContext::HEADERS_DONE;
}

}