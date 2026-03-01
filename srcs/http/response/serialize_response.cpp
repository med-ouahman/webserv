
#include "HTTPResponseHandler.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
namespace http {
    std::string HTTPResponseHandler::serialize( void ) const {
        std::string response_buff = "HTTP/1.0 200 OK\n\rContent-Type: text/html\r\n";
        
        int fd = open("www/index.html", O_RDONLY);
        char buff[8192];
        ssize_t b;
        std::string body;
        while ((b=read(fd, buff,8191)) > 0) {
            buff[b]=0;
            body.append(buff);
        }
        std::stringstream ss;
        ss << body.size();
        response_buff.append("Content-length: "+ss.str()+"\r\n\r\n");
        response_buff.append(body);
        ::close(fd);
        return response_buff;
    }
}
