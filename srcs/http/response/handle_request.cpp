#include "HTTPResponseHandler.hpp"
#include "HTTPRequest.hpp"
#include <fcntl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <sys/stat.h>

namespace http {
    
    std::string send_favicon( void ) {
        std::string body = "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\n";

        int fd = open("www/favicon.ico", O_RDONLY);
        if (fd < 0) {
            return "";
        }
        char buff[8192];
        ssize_t s;
        struct stat buf;
        stat("www/favicon.ico", &buf);
        size_t size = buf.st_size;
         std::stringstream ss;
        ss << size;
        body.append("content-length: "+ss.str()+"\r\n\r\n");
        while ((s = read(fd, buff, 8191) > 0)) {
            buff[s ] = 0x0;
            body.append(buff);
        }
        return body;
    }  

    void HTTPResponseHandler::handle_request( const HTTPRequest& req ) {
        
        if (req.url == "/favicon.ico ") {
            response.body = send_favicon();
            return ;
        }
        write(1, (req.url+"\n").c_str(), req.url.size()+1);
        if (!req.version_supported()) {
            build_error_response(http::VERSION_NOT_SUPPORTED, "Version not supported");
            allow_keep_alive = false;
            return ;
        }

        response.body = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
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

        return ;
    }
}
