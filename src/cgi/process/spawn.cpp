    #include "CGIRequestHandler.hpp"
    #include "EventLoop.hpp"
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <cstdlib>
    #include "Request.hpp"
    #include "Timestamp.hpp"
    #include <sstream>

    namespace http {

    bool CGIRequestHandler::forbidden_header( const std::string& header_name ) {

    for ( size_t i(0); stripped_headers[i] != NULL; ++i ) {
        if (stripped_headers[i] == header_name) {
            return true;
        }
    }

    return false;
    }

    char* CGIRequestHandler::transform( bool has_http_prefix, Headers::const_iterator& it ) {

    std::string key = const_cast<std::string&>(it->name);
    const std::string& value = it->value;

    for (size_t i = 0; i < key.size(); ++i) {

        char c = key[i];

        if (c == '-')
            key[i] = '_';
        else
            key[i] = std::toupper(static_cast<unsigned char>(c));
    }

    std::string env_name = has_http_prefix ? "HTTP_" + key : key;

    size_t alloc_size = env_name.size() + 1 + value.size() + 1;

    char* var = new char[alloc_size];

    size_t pos = 0;

    for (size_t i = 0; i < env_name.size(); ++i)
        var[pos++] = env_name[i];

    var[pos++] = '=';
    for (size_t i = 0; i < value.size(); ++i)
        var[pos++] = value[i];

    var[pos] = '\0';

    return var;
    }

    Headers CGIRequestHandler::build_cgi_metadata( const CGIRequestContext& ctx ) {

    Headers headers;
    Request& req = const_cast<Request&>(result.request);

    for ( size_t i(0); cgi_metadata[i] != NULL; ++i ) {
        
        std::string s = std::string(cgi_metadata[i]);
        
        if (s == "REQUEST_METHOD") {
            headers.add(s, req.get_method_name());
        } else if (s == "SERVER_PROTOCOL") {
            headers.add(s, "HTTP/1.1");
        } else if (s == "QUERY_STRING") {
            headers.add(s, req.data().query_string);
        } else if (s == "CONTENT_TYPE") {
            headers.add(s, result.mime_type);
        } else if (s == "CONTENT_LENGTH") {
            std::stringstream ss;
            ss << std::dec << ctx.body_content_length;
            headers.add(s, ss.str()); // calc
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

    char** CGIRequestHandler::build_cgi_env( const CGIRequestContext& ctx ){

    size_t size = 0;

    // for ( ; __environ[size] not_eq NULL; ++size );


    const Headers& headers = result.request.data().headers;

    Headers::const_iterator it = headers.begin();

    for ( ; it != headers.end(); ++it ) ++size;

    Headers cgi_metadata = build_cgi_metadata(ctx);
    it = cgi_metadata.begin();

    for ( ; it != cgi_metadata.end(); ++it ) ++size;

    char** cgi_variables = new char*[size];

    cgi_variables[size - 1] = NULL;

    size_t i = 0;
    // for ( ; __environ[i] != NULL; ++i ) cgi_variables[i] = __environ[i];

    for (  it = headers.begin(); it != headers.end(); ) {

        if (forbidden_header(it->name)) {
            continue;
        }

        cgi_variables[i] = transform(true, it);
        ++it;
        ++i;
    }

    for (  it = cgi_metadata.begin(); it != cgi_metadata.end(); ++it ) {
        cgi_variables[i] = transform(false, it);
        ++i;
    }
    cgi_variables[i] = NULL;
    std::cout << "size: " << i << "\n";
    return cgi_variables;
    }


    }
