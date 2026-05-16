#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include "EventLoop.hpp"
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include "HTTPRequest.hpp"
#include "Timestamp.hpp"

namespace http {

    char* CGIHandler::transform(std::map<std::string, std::string>::iterator& it) {

        std::string& key = const_cast<std::string&>(it->first);
        const std::string& value = it->second;

        std::string env_name;

        if (key == "content-type") env_name = "CONTENT_TYPE";
        else if (key == "content-length") env_name = "CONTENT_LENGTH";
        else {

            env_name = "HTTP_";

            for (size_t i = 0; i < key.size(); ++i) {

                char c = key[i];

                if (c == '-')
                    env_name += '_';
                else
                    key[i] = std::toupper(static_cast<unsigned char>(c));
            }

            env_name.append(key);
        }

        // KEY=value + '\0'
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

    char** CGIHandler::build_cgi_env(){
        size_t size = 0;

        for (; __environ[size]; ++size );
        
        HTTPRequest& req = const_cast<HTTPRequest&>(result.request);
        std::map<std::string, std::string>::iterator it = req.headers.begin();

        for ( ; it != result.request.headers.end(); ++it) ++size;

        char** s = build_non_header_vars();

        char** cgi_variables = new char*[size];

        size_t i = 0;
        for ( ; __environ[i] != NULL; ++i) cgi_variables[i] = __environ[i];

        it = req.headers.begin();

        for ( ; it != req.headers.end(); ++it ) cgi_variables[i] = transform(const_cast<std::map<std::string, std::string>::iterator&>(it));

        return cgi_variables;
    }

    void CGIHandler::spawn( const io::EventLoop& loop ) {
        
        if (cgi_state != CGIState::SPAWN) {
            std::cout << "Already spawned\n";
            return ;
        }

        CGIContext context = http::HTTPDispatcher::resolve_cgi_context(result);
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
               
            ::execve(context.interpreter_path.c_str(), argv, build_cgi_env());
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
