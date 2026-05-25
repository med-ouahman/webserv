
#include "CGIEnvBuilder.hpp"
#include "CGIResolver.hpp"
#include <sstream>
#include <iomanip>

namespace cgi {

const char* CGIEnvBuilder::metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};
const char* CGIEnvBuilder::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

bool CGIEnvBuilder::forbidden_header( const std::string& header_name ) {

    for ( size_t i(0); stripped_headers[i] != NULL; ++i ) {
        if (stripped_headers[i] == header_name) return true;
    }

    return false;
}

std::string CGIEnvBuilder::transform( bool has_http_prefix, http::Headers::const_iterator& it ) {

    std::string key = const_cast<std::string&>(it->name);
    const std::string& value = it->value;
    for ( size_t i = 0; i < key.size(); ++i ) {
        char c = key[i];
        if (c == '-') key[i] = '_';
        else key[i] = std::toupper(static_cast<unsigned char>(c));
    }

    return has_http_prefix ? "HTTP_" + key : key;
}

http::Headers CGIEnvBuilder::build_metadata( const CGIResolver::CGIRequestContext& ctx ) {

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

CStringArray CGIEnvBuilder::build( const CGIResolver::CGIRequestContext& ctx,
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
