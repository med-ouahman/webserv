
#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"
#include "Config.hpp"
#include "../../core/SessionManager.hpp"
#include <cctype>
#include <sstream>


namespace http {

    HTTPResponseHandler::HTTPResponseHandler() {}

    HTTPResponseHandler::~HTTPResponseHandler() {}

    std::string HTTPResponseHandler::serialize( void ) const {
        return response.body;
    }

    void HTTPResponseHandler::handle_request( const HTTPRequest& req, const config::ServerConfig& server ) {
        // simple default body
        response.body = "OK";
        std::ostringstream __len_ss;
        __len_ss << response.body.size();
        response.headers["content-length"] = __len_ss.str();
        response.headers["connection"] = "close";

        if (!server.session_enabled) {
            return;
        }

        std::string cookie_name = server.session_cookie_name.empty() ? std::string("SESSIONID") : server.session_cookie_name;
        std::string found_session;

        std::map<std::string,std::string>::const_iterator it = req.headers.find("cookie");
        if (it != req.headers.end()) {
            std::string cookie_str = it->second;
            size_t pos = 0;
            while (pos < cookie_str.size()) {
                size_t semi = cookie_str.find(';', pos);
                std::string token = (semi == std::string::npos) ? cookie_str.substr(pos) : cookie_str.substr(pos, semi - pos);
                // trim
                size_t s = 0;
                while (s < token.size() && isspace(static_cast<unsigned char>(token[s]))) s++;
                size_t e = token.size();
                while (e > s && isspace(static_cast<unsigned char>(token[e-1]))) e--;
                std::string kv = token.substr(s, e - s);
                size_t eq = kv.find('=');
                if (eq != std::string::npos) {
                    std::string name = kv.substr(0, eq);
                    std::string value = kv.substr(eq + 1);
                    if (name == cookie_name) {
                        found_session = value;
                        break;
                    }
                }
                if (semi == std::string::npos) break;
                pos = semi + 1;
            }
        }

        core::SessionManager& sm = core::SessionManager::instance();
        if (found_session.empty() || !sm.has_session(found_session)) {
            std::string new_id = sm.create_session();
            std::string sc = cookie_name + "=" + new_id + "; Path=/; HttpOnly";
            response.headers["set-cookie"] = sc;
        }
    }

    HTTPResponseHandler::ResolutionResult HTTPResponseHandler::resolve( const HTTPRequest& req, const config::ServerConfig& server ) const {
        ResolutionResult result;

        const config::LocationConfig* location = find_location(req.url, server.locations);
        
        if (!location) {
            result.status_code = NOT_FOUND;
            result.type = ERROR_RESPONSE;
            result.path = "";
        }
        
        return result;
    }

    const config::LocationConfig* HTTPResponseHandler::find_location( const std::string& url, const std::vector<config::LocationConfig>& locations ) {
    
        size_t location_index = locations.size();
        size_t best_prefix_length = 0;

        for ( size_t i = 0; i < locations.size(); ++i ) {
            if (url == locations[i].path) {
                return &locations[i];
            }
            if (!url.compare(0, locations[i].path.size(), locations[i].path) && url[locations[i].path.size()] == '/') {
                if (best_prefix_length < locations[i].path.size()) {
                    best_prefix_length = locations[i].path.size();
                    location_index = i;
                }
            }
        }

        if (location_index == locations.size()) {
            return NULL;
        }

        return &locations[location_index];
    }

    void HTTPResponseHandler::build_error_response( HTTPStatusCode code, std::string reason ) {
        response.status_code = code;
        response.reason = reason;
        response.headers["Content-Length"] = "0";
        response.headers["Connection"] = "close";
    }

}
