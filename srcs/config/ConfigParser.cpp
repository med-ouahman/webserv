#include "ConfigParser.hpp"
#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <arpa/inet.h>

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

ConfigParser::~ConfigParser() {

}


#ifdef DEV_MODE
Config ConfigParser::build_default_config() {
    Config conf;
    ServerConfig server;
    LocationConfig root, images;
    server.client_max_body_size = 1000000;
    server.listens.push_back( (ListenEndPoint ) {
        0, 3000
    } );
    

    server.server_names.push_back("localhost");
    server.error_pages[404] = "./error_pages/404.html";
    server.error_pages[501] = "./error_pages/501.html";
    server.index_files.push_back("index.html");
    server.index_files.push_back("index.htm");
    root.root = server.root;
    root.allowed_methods.insert("GET");
    root.allowed_methods.insert("POST");
    server.locations.push_back(root);

    images.root = "./images";
    images.allowed_methods = root.allowed_methods;
    images.index = server.index_files;
    server.locations.push_back(images);
    conf.server = server;
    return conf;
}

#endif


Token ConfigParser::expect(TokenType type)
{
    if (pos >= tokens.size() || tokens[pos].type != type)
        throw std::runtime_error("Unexpected token");

    return tokens[pos++];
}

static uint16_t parsePort(const std::string& value)
{
    char* end = 0;
    long port = std::strtol(value.c_str(), &end, 10);
    if (*end != '\0' || port <= 0 || port > 65535)
        throw std::runtime_error("Invalid port value");
    return static_cast<uint16_t>(port);
}

static size_t parseSize(const std::string& value)
{
    char* end = 0;
    unsigned long number = std::strtoul(value.c_str(), &end, 10);
    if (end == value.c_str())
        throw std::runtime_error("Invalid size value");

    unsigned long multiplier = 1;
    if (*end != '\0')
    {
        if (end[1] != '\0')
            throw std::runtime_error("Invalid size suffix");

        switch (*end)
        {
            case 'K': case 'k': multiplier = 1024UL; break;
            case 'M': case 'm': multiplier = 1024UL * 1024UL; break;
            case 'G': case 'g': multiplier = 1024UL * 1024UL * 1024UL; break;
            default:
                throw std::runtime_error("Invalid size suffix");
        }
    }

    unsigned long result = number * multiplier;
    if (multiplier > 1 && result / multiplier != number)
        throw std::runtime_error("Size value overflow");

    return static_cast<size_t>(result);
}

static uint32_t parseHost(const std::string& value)
{
    struct in_addr addr;
    if (inet_aton(value.c_str(), &addr) == 0)
        throw std::runtime_error("Invalid listen address");
    return addr.s_addr;
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
        if (tokens[pos].type == WORD && tokens[pos].value == "server")
        {
            pos++;
            conf.servers.push_back(parseServer());
        }
        else
            throw std::runtime_error("Expected server block");
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
        std::string target = expect(WORD).value;
        ListenEndPoint ep;

        size_t colon = target.find(':');
        if (colon == std::string::npos)
        {
            ep.host = 0;
            ep.port = parsePort(target);
        }
        else
        {
            ep.host = parseHost(target.substr(0, colon));
            ep.port = parsePort(target.substr(colon + 1));
        }

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
        server.client_max_body_size = parseSize(expect(WORD).value);
    }

    else if (key == "session_enabled")
    {
        std::string value = expect(WORD).value;
        if (value == "on")
            server.session_enabled = true;
        else if (value == "off")
            server.session_enabled = false;
        else
            throw std::runtime_error("Invalid session_enabled value");
    }

    else if (key == "session_cookie_name")
    {
        server.session_cookie_name = expect(WORD).value;
    }

    else if (key == "session_timeout")
    {
        server.session_timeout = parseSize(expect(WORD).value);
    }

    else if (key == "session_store")
    {
        server.session_store = expect(WORD).value;
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

    else if (key == "autoindex")
    {
        std::string value = expect(WORD).value;
        if (value == "on")
            loc.autoindex = true;
        else if (value == "off")
            loc.autoindex = false;
        else
            throw std::runtime_error("Invalid autoindex value");
    }

    else if (key == "upload_enable")
    {
        std::string value = expect(WORD).value;
        if (value == "on")
            loc.upload_enabled = true;
        else if (value == "off")
            loc.upload_enabled = false;
        else
            throw std::runtime_error("Invalid upload_enable value");
    }

    else if (key == "upload_path" || key == "upload_store")
    {
        loc.upload_path = expect(WORD).value;
    }

    else if (key == "cgi_pass")
    {
        std::string extension = expect(WORD).value;
        std::string executable = expect(WORD).value;
        loc.cgi_pass[extension] = executable;
    }

    else if (key == "cgi_extension")
    {
        loc.cgi_extension = expect(WORD).value;
        if (!loc.cgi_path.empty())
            loc.cgi_pass[loc.cgi_extension] = loc.cgi_path;
    }

    else if (key == "cgi_path")
    {
        loc.cgi_path = expect(WORD).value;
        if (!loc.cgi_extension.empty())
            loc.cgi_pass[loc.cgi_extension] = loc.cgi_path;
    }

    else
        throw std::runtime_error("Unknown location directive");

    expect(SEMICOLON);
}

}