#pragma once

#include "Config.hpp"
#include "Lexer.hpp"

namespace config {

class ConfigParser {

private:

    std::vector<Token> tokens;
    size_t pos;

    bool match(TokenType type);
    Token expect(TokenType type);

    ServerConfig parseServer();
    LocationConfig parseLocation();

    void parseDirective(ServerConfig& server);
    void parseLocationDirective(LocationConfig& loc);

public:

    Config parse(const char* path);
};

}