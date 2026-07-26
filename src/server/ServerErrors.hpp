#pragma once

enum ServerErrors {
    None,
    AllocFailed,
    SockFailed,
    ConfError,
    IOError,
    ConnectionError,
};

inline const char* server_error_message(ServerErrors error) {

    switch (error) {
        case AllocFailed: return "Allocation failed";
        case SockFailed: return "Socket creation faild";
        case ConfError: return "Configuration error";
        case IOError: return "IO error";
        case ConnectionError: return "Client connection error";
        default: return "";
    }

    return "";
}
