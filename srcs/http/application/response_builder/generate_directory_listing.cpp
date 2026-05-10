#include "HTTPDispatcher.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

namespace http {
    
    std::string HTTPDispatcher::generate_anchor( const char* name ) {
        std::string s = std::string(name);
        return std::string("<li><a href=\"") + s + std::string("\">") + s + std::string("</a></li>");
    }

    std::string HTTPDispatcher::generate_directory_list( const char* dirname ) {
        DIR* dir = ::opendir(dirname);
     
        if (!dir) {
            return "";
        }

        std::string index(dirname);
        if (index[index.size() - 1] != '/') {
            index.append("/index.html");
        }
        std::ofstream inFile(index.c_str(), std::ios::out | std::ios::trunc);
        if (!inFile.is_open()) {
            return "";
        }

        std::string div = "<div style=\"width: 100%; height: 2px; background: #000; margin: 10px 0;\"></div>";
        std::string buff = std::string("<!DOCTYPE html><html><head><title> Directory listing for ")
            + std::string(dirname)
            + std::string("</title></head><body><h1>Directory listing for ")
            + std::string(dirname) + std::string("</h1>") + div;

        ::size_t max_size = 1024 * 5;
        dirent* current;
        while ((current = readdir(dir))) {    
            std::string current_list = generate_anchor(current->d_name);
            std::cout << current_list << "\n";
            if (buff.size() > max_size) {
                inFile << buff;
                buff.clear();
            } else {
                buff.append(current_list);
            }
        }
        ::closedir(dir);
        buff.append("</body></html>" + div);
        inFile << buff;
        return index;
    }
}