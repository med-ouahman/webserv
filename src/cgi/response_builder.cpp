#include "ResponseParser.hpp"
#include <iostream>
#include "http/parser/Parser.hpp"
#include <cstdlib>

namespace cgi {
namespace parser {

ParseResult ResponseParser::parse_headers(DataView& view) {
    while (parse_ctx.state_ != HEADERS_DONE) {
        size_t max_scan_size = CGIParseContext::MAX_CGI_HEADER_BLOCK_LEN - parse_ctx.header_bytes_;
        parse_ctx.sc_.reset();
        http::ScanResult r = parse_ctx.sc_.scan(view, max_scan_size);
        if (r == http::LIMIT_EXCEEDED) return PARSE_ERROR;
        if (r == http::NEED_MORE) return PARSE_CONTINUE;
        if (parse_ctx.sc_.line().empty() && parse_ctx.state_ == HEADERS) {
            parse_ctx.state_ = HEADERS_DONE;
            break;
        }
        std::cout << parse_ctx.sc_.line() << "\n";
        Base::Expected<std::pair<std::string, std::string>, int> header_result = http::parser::parse_header(parse_ctx.sc_.line());
        if (not header_result.has_value()) {
            std::cout  << "Error\n";
            return PARSE_ERROR;
        }
        std::cout << header_result.value().first << " | " << header_result.value().second << "\n";
        switch (parse_ctx.state_) {
            case HEADERS:
                std::cout << "XXD Headers\n";
                sanitize_header(header_result.value());
                break;
            default:
                return PARSE_SUCCESS;
        }
    }
    return PARSE_SUCCESS;
}

void ResponseParser::sanitize_status_line(const std::pair<std::string, std::string>& header) {

    const std::string& value = header.second;

    size_t space_pos = value.find(' ');

    if (space_pos == std::string::npos) {
        parse_ctx.state_ = CGI_ERROR;
        return ;
    }

    std::string code_str = value.substr(0, space_pos);
    std::string reason   = value.substr(space_pos + 1);

    for (size_t i = 0; i < code_str.size(); ++i) {
        if (!std::isdigit(code_str[i])) {
            parse_ctx.state_ = CGI_ERROR;
            return;
        }
    }

    int code = std::atoi(code_str.c_str());

    if (code < 200 or code > 599) {
        return ;
    }

    parse_ctx.state_ = HEADERS;
    std::cout << "Done\n";
}

void ResponseParser::sanitize_header(std::pair<std::string, std::string>& header) {
    if (header.first == "status") {
        sanitize_status_line(header);
        return;
    }

    http::parser::capitalize_header_name(header.first);
    headers_.add(header.first, header.second);
}

const http::Headers& ResponseParser::headers() const {
    return headers_;
}

bool ResponseParser::finished() const {
    return parse_ctx.state_ == HEADERS_DONE;
}

}
}