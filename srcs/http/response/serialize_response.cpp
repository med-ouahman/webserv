
#include "HTTPResponseHandler.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>

namespace http {
    void HTTPResponseHandler::serialize( void ) {

        response.body = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n";
        int f = open("www/index.html", O_RDONLY);
        char buff[8192];
        ssize_t b;
        std::string body;
        while ((b=read(f, buff,8191)) > 0) {
            buff[b]=0;
            body.append(buff);
        }
        std::stringstream ss;
        ss << body.size();
        response.body.append("Content-length: "+ss.str()+"\r\n\r\n");
        response.body.append(body);
        ::close(f);
    }
}
