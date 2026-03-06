#include "Lexer.hpp"
#include <fstream>
#include <cctype>
#include <stdexcept>

namespace config {

void Lexer::tokenize(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file)
        throw std::runtime_error("Cannot open config file");

    char c;
    std::string word;

    while (file.get(c))
    {
        if (isspace(c))
        {
            if (!word.empty())
            {
                tokens.push_back(Token(WORD, word));
                word.clear();
            }
        }
        else if (c == '{')
        {
            if (!word.empty())
            {
                tokens.push_back(Token(WORD, word));
                word.clear();
            }
            tokens.push_back(Token(LBRACE,"{"));
        }
        else if (c == '}')
        {
            if (!word.empty())
            {
                tokens.push_back(Token(WORD, word));
                word.clear();
            }
            tokens.push_back(Token(RBRACE,"}"));
        }
        else if (c == ';')
        {
            if (!word.empty())
            {
                tokens.push_back(Token(WORD, word));
                word.clear();
            }
            tokens.push_back(Token(SEMICOLON,";"));
        }
        else
        {
            word += c;
        }
    }

    if (!word.empty())
        tokens.push_back(Token(WORD, word));
}

const std::vector<Token>& Lexer::getTokens() const
{
    return tokens;
}

}