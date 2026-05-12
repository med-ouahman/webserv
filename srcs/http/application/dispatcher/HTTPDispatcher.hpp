
#pragma once

#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"
#include <vector>
#include <iostream>
#include "CGIContext.hpp"
#include <memory>
#include "BodyHandler.hpp"
#include "IRequestHandler.hpp"

namespace core {
    class Connection;
}


namespace config {
    struct ServerConfig;
    struct LocationConfig;
    struct Config;
}

namespace http {

    struct HTTPRequest;

    struct BodyPolicy {
        BodyType::Type      type;
        BodyStorage::Type   storage;
        size_t              parsed_body_size;
        std::string         body_path;
    };

    struct ResolutionResult {
        BodyPolicy body_policy;
        HTTPResponseType::Type type;
        HTTPStatusCode status_code;
        std::string reason;
        std::string mime_type;
        std::map<std::string, std::string> extra_headers;
        std::string path;
        CGIContext* cgi_context;

        ResolutionResult( HTTPStatusCode c, std::string const& r )
            : type(HTTPResponseType::ERROR_RESPONSE),
            status_code(c),
            reason(r) {}
            
        ResolutionResult() {}
    };


    class HTTPDispatcher {
        public:
            static const std::size_t MAX_BUFFERED_BODY_SIZE = 1024;

        private:
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
            static std::string extract_path( const std::string& url );
            static bool file_exists( const char* filename );
            static size_t parse_content_length( const std::string& s );

        public:
            static std::string generate_directory_list( const char* dir ); // TO BE MOVED
            static std::string generate_anchor( const char* name ); // TO BE MOVED
            static void build_error_response( HTTPStatusCode code, std::string reason ); // TO BE MOVED;
            static ResolutionResult resolve( const HTTPRequest& req );
            HTTPDispatcher();
            ~HTTPDispatcher();
            static bool allow_presistance() { return true; };
            static IRequestHandler* dispatch_request_handler( core::Connection& conn, const HTTPRequest& req );
            static CGIContext get_cgi_context( const HTTPRequest& req, const ResolutionResult& result );

			static BodyType::Type detect_body_type( std::map<std::string, std::string>& headers );

            static void configure_body_policy( const HTTPRequest& req, ResolutionResult& result );

    };
}
