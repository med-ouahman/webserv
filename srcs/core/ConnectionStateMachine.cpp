#include "ConnectionStateMachine.hpp"

#include "ConnectionStateMachine.hpp"

namespace core {

ConnectionState
ConnectionStateMachine::next_state( ConnectionState currentState,
    ConnectionEvent event ) {
    switch (currentState) {

    // ───────────────────────────────── ACCEPTED ─────────────────────────────
    case ACCEPTED:
        if (event == SOCKET_READABLE)
            return READING;
        break;

    // ───────────────────────────────── READING ──────────────────────────────
    case READING:
        if (event == SOCKET_READABLE)
            return READING; // allowed, attempt read
        if (event == READ_SUCCESS)
            return PARSING;
        if (event == READ_EOF)
            return CLOSING;
        if (event == READ_ERROR)
            return ERROR;
        break;

    // ───────────────────────────────── PARSING ─────────────────────────────
    case PARSING:
        if (event == PARSE_NEED_MORE)
            return READING;
        if (event == PARSE_COMPLETE)
            return PROCESSING;
        if (event == PARSE_ERROR)
            return ERROR;
        break;

    // ───────────────────────────────── PROCESSING ───────────────────────────
    case PROCESSING:
        if (event == PROCESSING_DONE)
            return READY_TO_WRITE;
        break;

    // ───────────────────────────────── READY_TO_WRITE ───────────────────────
    case READY_TO_WRITE:
        if (event == SOCKET_WRITABLE)
            return WRITING;
        break;

    // ───────────────────────────────── WRITING ──────────────────────────────
    case WRITING:
        if (event == WRITE_SUCCESS)
            return WRITE_COMPLETE;
        if (event == WRITE_ERROR)
            return ERROR;
        break;

    // ───────────────────────────────── WRITE_COMPLETE ───────────────────────
    case WRITE_COMPLETE:
        if (event == CLOSE_REQUESTED)
            return CLOSING;
        if (event == SOCKET_READABLE)
            return READING; // keep-alive path
        break;

    // ───────────────────────────────── ERROR ────────────────────────────────
    case ERROR:
        return CLOSING;

    // ───────────────────────────────── CLOSING ──────────────────────────────
    case CLOSING:
        return CLOSING;
    }

    // Any other (state, event) pair is illegal
    return ERROR;
}

} // namespace core
