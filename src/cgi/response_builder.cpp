#include "cgi.hpp"
#include <iostream>
#include "http/parser/Parser.hpp"
#include <cstdlib>

namespace cgi {
namespace parser {

ParseResult ResponseBuilder::parse_headers( DataView& view ) {
    while (ctx_.state_ != HEADERS_DONE) {

        size_t max_scan_size = CGIParseContext::MAX_CGI_HEADER_BLOCK_LEN - ctx_.header_bytes_;

        ctx_.sc_.reset();
        http::ScanResult r = ctx_.sc_.scan(view, max_scan_size);
        
        if (r == http::LIMIT_EXCEEDED) return PARSE_ERROR;
        if (r == http::NEED_MORE) return PARSE_CONTINUE;

        if (ctx_.sc_.line().empty() && ctx_.state_ == HEADERS) {
            ctx_.state_ = HEADERS_DONE;
            break;
        }

        std::cout << ctx_.sc_.line() << "\n";
        Base::Expected<std::pair<std::string, std::string>, int> header_result = http::parser::parse_header(ctx_.sc_.line());

        if (not header_result.has_value()) {
            std::cout  << "Error\n";
            return PARSE_ERROR;
        }

        std::cout << header_result.value().first << " | " << header_result.value().second << "\n";

        switch (ctx_.state_) {
            case STATUS_LINE:
                sanitize_status_line(header_result.value());
                break;
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

void ResponseBuilder::sanitize_status_line( const std::pair<std::string, std::string>& header ) {
    if (header.first != "status") return;

    const std::string& value = header.second;

    size_t space_pos = value.find(' ');

    if (space_pos == std::string::npos) {
        ctx_.state_ = CGI_ERROR;
        return ;
    }

    std::string code_str = value.substr(0, space_pos);
    std::string reason   = value.substr(space_pos + 1);

    for ( size_t i = 0; i < code_str.size(); ++i ) {
        if (!std::isdigit(code_str[i])) {
            ctx_.state_ = CGI_ERROR;
            return;
        }
    }

    int code = std::atoi(code_str.c_str());

    if (code < 200 or code > 599) {
        return ;
    }

    ctx_.state_ = HEADERS;
    std::cout << "Done\n";
}

void ResponseBuilder::sanitize_header( std::pair<std::string, std::string>& header ) {
    http::parser::capitalize_header_name(header.first);
    headers_.add(header.first, header.second);
}

const http::Headers& ResponseBuilder::headers() const {
    return headers_;
}

}
}