# HTTP changes

1. RequestHandler::handler()
location: src/http/pipeline/handlers/
Issue: While other handlers do the work inside the handle method, CgiHandler relies on events from the event loop to decide when to do work
so it might not finish now, need a method that tells if the handler has finished and can move to writing the response


2. Changed the Context to accept CGI related info

location: src/http/
Issue: Couldn't pass system info like EventLoop to CGI


3. CGI special case
location: src/http/pipeline/handlers/CgiHandler.hpp
Issue: CGI needs to monitor its resources and free them whenever they're no longer needed. a method CgiHandler::monitor runs every event loop cycle to check:
timeouts, Process exit, and pipes


