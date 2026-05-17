#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include "HTTPRequestData.hpp"
#include "Timestamp.hpp"

namespace http {

    bool CGIHandler::forbidden_header( const std::string& header_name ) {

        for ( size_t i(0); stripped_headers[i] != NULL; ++i ) {
            if (stripped_headers[i] == header_name) {
                return true;
            }
        }

        return false;
    }

    char* CGIHandler::transform( bool http_prefix, std::map<std::string, std::string>::iterator& it ) {

        std::string key = const_cast<std::string&>(it->first);
        const std::string& value = it->second;

        for (size_t i = 0; i < key.size(); ++i) {

            char c = key[i];

            if (c == '-')
                key[i] = '_';
            else
                key[i] = std::toupper(static_cast<unsigned char>(c));
        }

        std::string env_name = http_prefix ? "HTTP_" + key : key;
        
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

    std::map<std::string, std::string> CGIHandler::build_cgi_metadata( const CGIContext& ctx ) {
        std::map<std::string, std::string> headers;
        HTTPRequestData& req = const_cast<HTTPRequestData&>(result.request);

        for ( size_t i(0); cgi_metadata[i] != NULL; ++i ) {
            
            std::string s = std::string(cgi_metadata[i]);
            
            if (s == "REQUEST_METHOD") {
                headers[s] = req.get_method_name(req.method);
            } else if (s == "SERVER_PROTOCOL") {
                headers[s] = "HTTP/1.1";
            } else if (s == "QUERY_STRING") {
                headers[s] = req.query;
            } else if (s == "CONTENT_TYPE") {
                headers[s] = result.mime_type;
            } else if (s == "CONTENT_LENGTH") {
                headers[s] = ""; // calc
            } else if (s == "GATEWAY_INTERFACE") {
                headers[s] = "CGI/1.1";
            } else if (s == "SCRIPT_NAME") {
                headers[s] = ctx.script_name;
            } else if (s == "PATH_INFO") {
                headers[s] = ctx.path_info;
            } else if (s == "SERVER_NAME") {
                headers[s] = ctx.server_name;
            } else if (s == "SERVER_PORT") {
                headers[s] = ctx.server_port;
            }
        }

        return headers;
    }

    char** CGIHandler::build_cgi_env( const CGIContext& ctx ){
        size_t size = 0;

        for (; __environ[size]; ++size );
        
        HTTPRequestData& req = const_cast<HTTPRequestData&>(result.request);
        std::map<std::string, std::string>::iterator it = req.headers.begin();
        for ( ; it != result.request.headers.end(); ++it ) ++size;
        
        std::map<std::string, std::string> cgi_metadata = build_cgi_metadata(ctx);
        it = cgi_metadata.begin();

        for ( ; it != cgi_metadata.end(); ++it ) ++size;

        char** cgi_variables = new char*[size];
        
        cgi_variables[size - 1] = NULL;

        size_t i = 0;
        for ( ; __environ[i] != NULL; ++i ) cgi_variables[i] = __environ[i];

        for (  it = req.headers.begin(); it != req.headers.end(); ) {

            if (forbidden_header(it->first)) {
                continue;
            }

            cgi_variables[i] = transform(true, const_cast<std::map<std::string, std::string>::iterator&>(it));
            ++it;
        }

        for (  it = cgi_metadata.begin(); it != cgi_metadata.end(); ++it )
            cgi_variables[i] = transform(false, const_cast<std::map<std::string, std::string>::iterator&>(it));
        cgi_variables[i] = NULL;
        return cgi_variables;
    }

    void CGIHandler::spawn( const io::EventLoop& loop ) {
        
        if (cgi_state != CGIState::SPAWN) {
            std::cout << "Already spawned\n";
            return ;
        }

        CGIContext context = http::HTTPDispatcher::resolve_cgi_context(result);

        cgi_timeout_secs = context.timeout_seconds;
        
        cgi_state = CGIState::ACTIVE;
        start_time.update();
        cgi_pid = ::fork();
        if (cgi_pid == 0) {

            ::dup2(pipe_guard.stdout_pipe[1], STDOUT_FILENO);
            ::dup2(pipe_guard.stderr_pipe[1], STDERR_FILENO);
                        
            pipe_guard.close_pipes();
            char* const argv[] = {
               const_cast<char*>( context.interpreter_path.c_str()), 
               const_cast<char*>(context.script_filename.c_str()),
               NULL};
               
            ::execve(context.interpreter_path.c_str(), argv, build_cgi_env(context));
            LOG_ERROR(MAKE_ERRNO_ERROR("execve()"));
            ::exit(EXIT_FAILURE);
        }

        CLOSE_FD(pipe_guard.stdin_pipe[0]);
        CLOSE_FD(pipe_guard.stdout_pipe[1]);
        CLOSE_FD(pipe_guard.stderr_pipe[1]);
        
        if (cgi_pid < 0) {
            throw std::runtime_error(strerror(errno));
        }

        loop.add_fd(stdout_ch.fd(), stdout_ch.get_event(), &stdout_ch);
        loop.add_fd(stderr_ch.fd(), stderr_ch.get_event(), &stderr_ch);
    }

}
