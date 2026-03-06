#pragma once
#include <string>

namespace config {

enum TokenType {
    WORD,
    LBRACE,
    RBRACE,
    SEMICOLON
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, const std::string& v) : type(t), value(v) {}
};

}