# Session Integration:

# Affected: 

## http

Parser.cpp
 After parsing headers and in the processing phase, the cookies are then extracted, validated and stored in the request

Request.hpp:
 Added two attributes: currentSessionID -> string; holds the current session ID, currentSessionIDValid -> bool; tell wheter the session exists or not

handlers:
added Cookies Handlers that use Cookies

