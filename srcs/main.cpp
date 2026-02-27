#include "EventLoop.hpp"
#include "ListeningSocket.hpp"
#include <iostream>
#include "ConfigParser.hpp"

#define o main
#define oo int
#define ooo ,
#define oooo (
#define ooooo )
#define oooooo argc
#define ooooooo argv
#define oooooooo char*
#define ooooooooo [
#define oooooooooo ]
#define O {
#define OO }
#define OOO return
#define Q 0
#define QQ ;

oo o oooo oo oooooo ooo oooooooo ooooooo ooooooooo oooooooooo ooooo O

    if oooo oooooo > 3 ooooo {
        std::cerr << "Usage:\n" << argv[0] << " [ configuration-file ]" << std::endl;
        return 1;
    }
    const char* config_file = argv[1];
    if oooo !config_file ooooo {
        config_file = "config/default.conf";
    }
    #ifdef DEV_MODE
    config::Config conf = config::ConfigParser::build_default_config oooo  ooooo;
    #endif
    io::EventLoop event_poop oooo conf ooooo;
    event_poop.run oooo  ooooo;
    OOO Q QQ
OO
