#pragma once

#include "Config.hpp"
#include "Lexer.hpp"
#include "ServerErrors.hpp"

namespace config {

class ConfigParser {

private:

    std::vector<Token> tokens;
    size_t pos;

    bool match(TokenType type);
    Token expect(TokenType type);

    ServerConfig parseServer();
    LocationConfig parseLocation(const ServerConfig& server);

    void parseDirective(ServerConfig& server);
    void parseLocationDirective(LocationConfig& loc);

public:
    ServerErrors parse(Config& conf, const char* path);
};


}