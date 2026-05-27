
#include "cgi.hpp"
#include <iomanip>
#include <sstream>

namespace cgi {
namespace envp {

const char* Builder::metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};
const char* Builder::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

bool Builder::forbidden_header( const std::string& header_name ) {

    for ( size_t i(0); stripped_headers[i] != NULL; ++i ) {
        if (stripped_headers[i] == header_name) return true;
    }

    return false;
}

std::string Builder::transform( bool has_http_prefix, http::Headers::const_iterator& it ) {

    std::string result;
    const std::string http_prefix = has_http_prefix ? "HTTP_": "";

    size_t size = it->name.size() + it->value.size() + http_prefix.size();
    result.reserve(size);
    result.append(http_prefix);
    for ( size_t i = 0; i < it->name.size(); ++i ) {
        char c = it->name[i];
        if (c == '-') result[i] = '_';
        else result[i] = std::toupper(static_cast<unsigned char>(c));
    }
    result.append(it->name);
    return result;
}

http::Headers Builder::build_metadata( const resolver::CGIRequestContext& ctx ) {

    http::Headers headers;

    for ( size_t i(0); metadata[i] != NULL; ++i ) {
        std::string s = std::string(metadata[i]);
        if (s == "REQUEST_METHOD") {
            headers.add(s, ctx.request_method);
        } else if (s == "SERVER_PROTOCOL") {
            headers.add(s, "HTTP/1.1");
        } else if (s == "QUERY_STRING") {
            headers.add(s, ctx.query_string);
        } else if (s == "CONTENT_TYPE") {
            headers.add(s, ctx.mime_type);
        } else if (s == "CONTENT_LENGTH") {
            std::stringstream ss;
            ss << std::dec << ctx.body_content_length;
            headers.add(s, ss.str());
        } else if (s == "GATEWAY_INTERFACE") {
            headers.add(s, "CGI/1.1");
        } else if (s == "SCRIPT_NAME") {
            headers.add(s, ctx.script_name);
        } else if (s == "PATH_INFO") {
            headers.add(s, ctx.path_info);
        } else if (s == "SERVER_NAME") {
            headers.add(s, ctx.server_name);
        } else if (s == "SERVER_PORT") {
            std::stringstream ss;
            ss << std::dec << ctx.server_port;
            headers.add(s, ss.str());
        }
    }
    return headers;
}

CStringArray Builder::build( const resolver::CGIRequestContext& ctx,
    http::Headers const& request_headers ) {

    http::Headers headers = build_metadata(ctx);
    CStringArray arr;
    for ( http::Headers::const_iterator it = headers.begin();
            it != headers.end();
            ++it ) {
        arr.push(transform(false, it).c_str());
    }
    
    for ( http::Headers::const_iterator it = request_headers.begin();
        it != request_headers.end(); ++it ) {
            if (forbidden_header(it->name)) continue;
            arr.push(transform(true, it));
    }

    return arr;
}


}
}