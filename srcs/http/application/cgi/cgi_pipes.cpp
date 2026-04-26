#include "CGIHandler.hpp"
#include "CGIContext.hpp"

namespace http {

    bool CGIHandler::create_pipes( void ) {

        return true;
    }

    void CGIHandler::close_pipes( int* pipefd ) {
        
        pipefd[0] > 0 ? ::close(pipefd[0]): 0;
        pipefd[1] > 0 ? ::close(pipefd[1]): 0;
    }

}