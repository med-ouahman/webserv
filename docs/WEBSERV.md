# Webserv

# Overview
- webserv is a project at the 42 curriculum, it is about building a minimal yet functional
http server from scratch using C++98.

# What you will learn

- This project teaches you the basics of network programming (sockets, TCP), http (the protocol the powers the web), 
security basics (Dos attacks and malicious clients), event driven architectures and non-blocking IO through IO multiplexers (select, poll, epoll)
The project uses C++98 with limited features to understand every feature and build it from the ground up.

# How to get started

- Start with understanding http by reading the RFCs and working with http by hand using various http libraries provided by different languages
- Understand TCP and sockets, read books or manuals about the socket api
- Learn non-blocking IO and how it's different from blocking IO
- Learn about IO multiplexers and how to kernel notifies when a file descriptors is ready for IO
- Learn about event driven architectures where everything reacts to events

- Don't start with http, try creating a simple message protocol that supports both writing and reading, test with more than one client to see if you server
can handler multiple connections at the same time.
