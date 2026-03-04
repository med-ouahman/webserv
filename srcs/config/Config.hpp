#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>

class Location {
public:
    std::string path;
    std::string root;
    std::set<std::string> methods;
};

class Server {
public:
    int port;
    std::string host;
    std::vector<Location> locations;
};

class Config {
public:
    std::vector<Server> servers;
};

#endif