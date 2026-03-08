#include "ConfigParser.hpp"
#include <stdexcept>
#include <cstdlib>
#include <iostream>

namespace config {

bool ConfigParser::match(TokenType type)
{
    if (pos < tokens.size() && tokens[pos].type == type)
    {
        pos++;
        return true;
    }
    return false;
}

Token ConfigParser::expect(TokenType type)
{
    if (pos >= tokens.size() || tokens[pos].type != type)
        throw std::runtime_error("Unexpected token");

    return tokens[pos++];
}

Config ConfigParser::parse(const char* path)
{
    Lexer lexer;
    lexer.tokenize(path);

    tokens = lexer.getTokens();
    pos = 0;

    Config conf;

    while (pos < tokens.size())
    {
        // std::cout << "type : " << tokens[pos].type << " -- value : " << tokens[pos].value << std::endl;
        if (tokens[pos].value == "server")
        {
            pos++;
            conf.servers.push_back(parseServer());
        }
        else
            throw std::runtime_error("Expected server block");
        pos++;
    }

    return conf;
}

ServerConfig ConfigParser::parseServer()
{
    ServerConfig server;

    expect(LBRACE);

    while (!match(RBRACE))
        parseDirective(server);

    return server;
}

LocationConfig ConfigParser::parseLocation()
{
    LocationConfig loc;

    loc.path = expect(WORD).value;

    expect(LBRACE);

    while (!match(RBRACE))
        parseLocationDirective(loc);

    return loc;
}

void ConfigParser::parseDirective(ServerConfig& server)
{
    std::string key = expect(WORD).value;

    if (key == "listen")
    {
        ListenEndPoint ep;
        ep.host = 0;
        ep.port = atoi(expect(WORD).value.c_str());
        server.listens.push_back(ep);
    }

    else if (key == "server_name")
    {
        while (!match(SEMICOLON))
            server.server_names.push_back(expect(WORD).value);
        return;
    }

    else if (key == "root")
    {
        server.root = expect(WORD).value;
    }

    else if (key == "index")
    {
        while (!match(SEMICOLON))
            server.index_files.push_back(expect(WORD).value);
        return;
    }

    else if (key == "client_max_body_size")
    {
        server.client_max_body_size = atoi(expect(WORD).value.c_str());
    }

    else if (key == "error_page")
    {
        int code = atoi(expect(WORD).value.c_str());
        std::string path = expect(WORD).value;
        server.error_pages[code] = path;
    }

    else if (key == "location")
    {
        server.locations.push_back(parseLocation());
        return;
    }

    else
        throw std::runtime_error("Unknown directive in server");

    expect(SEMICOLON);
}

void ConfigParser::parseLocationDirective(LocationConfig& loc)
{
    std::string key = expect(WORD).value;

    if (key == "root")
    {
        loc.root = expect(WORD).value;
    }

    else if (key == "index")
    {
        while (!match(SEMICOLON))
            loc.index.push_back(expect(WORD).value);
        return;
    }

    else if (key == "allowed_methods")
    {
        while (!match(SEMICOLON))
            loc.allowed_methods.insert(expect(WORD).value);
        return;
    }

    else if (key == "return")
    {
        loc.redirect.return_code = atoi(expect(WORD).value.c_str());
        loc.redirect.return_target = expect(WORD).value;
    }

    else
        throw std::runtime_error("Unknown location directive");

    expect(SEMICOLON);
}

}