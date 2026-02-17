#pragma once

namespace core {
    enum ConnectionEvent {
        SOCKET_READABLE,
        SOCKET_WRITABLE,
        FATAL_ERROR,
        READ_SUCCESS,
        READ_EOF,
        READ_ERROR,
        PARSE_NEED_MORE_BYTES,
        PARSE_COMPLETE,
        PARSE_ERROR,
        PROCESSING_DONE,
        WRITE_SUCCESS,
        WRITE_ERROR,
        CLOSE_REQUESTED,
    };
}
