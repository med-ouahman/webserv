#include "CGIHandler.hpp"
#include "CGIContext.hpp"
#include <sys/wait.h>

namespace http {

    void CGIHandler::spawn( const CGIContext& context ) {


        int stdin_p[2];
        int stdout_p[2];
        int stderr_p[2];
        
        if (::pipe(stdin_p) || ::pipe(stderr_p) || ::pipe(stdout_p)) {
            close_pipes(stdin_p);
            close_pipes(stdout_p);
            close_pipes(stderr_p);
        }

        cgi_pid = ::fork();
        if (cgi_pid < 0) {
            return ;
        }

        if (cgi_pid == 0) {
            dup2(stdin_p[0], STDIN_FILENO);
            dup2(stdout_p[1], STDOUT_FILENO);
            dup2(stderr_p[1], STDERR_FILENO);
            
            close_pipes(stdin_p);
            close_pipes(stdout_p);
            close_pipes(stderr_p);

            chdir(context.working_directory.c_str());
            execve(context.interpreter_path.c_str(), NULL, __environ);
            exit(0);
        }

        pipe_stdin = stdin_p[1];
        pipe_stdout = stdout_p[0];
        pipe_stderr = stderr_p[0];
        waitpid(cgi_pid, &cgi_status, WNOHANG);
    }
}
