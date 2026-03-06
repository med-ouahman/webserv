#pragma once

#include "Token.hpp"
#include <vector>
#include <string>

namespace config {

class Lexer {

private:
    std::vector<Token> tokens;

public:
    void tokenize(const std::string& path);
    const std::vector<Token>& getTokens() const;
};

}