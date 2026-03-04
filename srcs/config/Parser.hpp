#ifndef PARSER_HPP
#define PARSER_HPP

#include "Config.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

public:
    Parser(const std::vector<Token>& tokens);

    Config parseConfig();

private:
    Server parseServer();
    Location parseLocation();

    void expect(TokenType type);
    bool check(TokenType type);
    void advance();
};

#endif