#include "ConnectionStateMachine.hpp"

#include "ConnectionStateMachine.hpp"

namespace core {

ConnectionState
ConnectionStateMachine::next_state( ConnectionState currentState,
    ConnectionEvent event ) {
    switch (currentState) {

    // ───────────────────────────────── ACCEPTED ─────────────────────────────
    case ConnectionState::ACCEPTED:
        if (event == ConnectionEvent::SOCKET_READABLE)
            return ConnectionState::READING;
        break;

    // ───────────────────────────────── READING ──────────────────────────────
    case ConnectionState::READING:
        if (event == ConnectionEvent::SOCKET_READABLE)
            return ConnectionState::READING; // allowed, attempt read
        if (event == ConnectionEvent::READ_SUCCESS)
            return ConnectionState::PARSING;
        if (event == ConnectionEvent::READ_EOF)
            return ConnectionState::CLOSING;
        if (event == ConnectionEvent::READ_ERROR)
            return ConnectionState::ERROR;
        break;

    // ───────────────────────────────── PARSING ─────────────────────────────
    case ConnectionState::PARSING:
        if (event == ConnectionEvent::PARSE_NEED_MORE)
            return ConnectionState::READING;
        if (event == ConnectionEvent::PARSE_COMPLETE)
            return ConnectionState::PROCESSING;
        if (event == ConnectionEvent::PARSE_ERROR)
            return ConnectionState::ERROR;
        break;

    // ───────────────────────────────── PROCESSING ───────────────────────────
    case ConnectionState::PROCESSING:
        if (event == ConnectionEvent::PROCESSING_DONE)
            return ConnectionState::READY_TO_WRITE;
        break;

    // ───────────────────────────────── READY_TO_WRITE ───────────────────────
    case ConnectionState::READY_TO_WRITE:
        if (event == ConnectionEvent::SOCKET_WRITABLE)
            return ConnectionState::WRITING;
        break;

    // ───────────────────────────────── WRITING ──────────────────────────────
    case ConnectionState::WRITING:
        if (event == ConnectionEvent::WRITE_SUCCESS)
            return ConnectionState::WRITE_COMPLETE;
        if (event == ConnectionEvent::WRITE_ERROR)
            return ConnectionState::ERROR;
        break;

    // ───────────────────────────────── WRITE_COMPLETE ───────────────────────
    case ConnectionState::WRITE_COMPLETE:
        if (event == ConnectionEvent::CLOSE_REQUESTED)
            return ConnectionState::CLOSING;
        if (event == ConnectionEvent::SOCKET_READABLE)
            return ConnectionState::READING; // keep-alive path
        break;

    // ───────────────────────────────── ERROR ────────────────────────────────
    case ConnectionState::ERROR:
        return ConnectionState::CLOSING;

    // ───────────────────────────────── CLOSING ──────────────────────────────
    case ConnectionState::CLOSING:
        return ConnectionState::CLOSING;
    }

    // Any other (state, event) pair is illegal
    return ConnectionState::ERROR;
}

} // namespace core
