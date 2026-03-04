Server Parser::parseServer()
{
    expect("server");
    expect("{");

    Server srv;

    while (!check("}"))
    {
        if (check("listen"))
            srv.port = parseListen();
        else if (check("host"))
            srv.host = parseHost();
        else if (check("location"))
            srv.locations.push_back(parseLocation());
        else
            error("Unexpected directive");
    }

    expect("}");
    return srv;
}