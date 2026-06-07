#include "Connection.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <sstream>
#include "Context.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"

namespace net {

Connection::Connection(int _fd, io::Event events, RegisterContext& regis_ctx)
    : stream_(_fd, events, *this),
    state_(READING),
    close_after_write(false),
    last_activity_(),
    lifetime_(),
    register_ctx(regis_ctx),
    ctx() {}

Connection::~Connection() {
}


void Connection::consume(DataView& view) {
    // ctx.consume(view.data(), view.size());
    std::cout << view.data(), view.size();std::cout << "\n";
}

void Connection::produce(BufferWriter& writer) {
    

    // base::io::Writer w(writer.data(), writer.size());
    /* in development */
    // ctx.produce(w);
    if (writer.remaining() == 0 && close_after_write) {
        state_ = CLOSING;
        stream_.update_events(io::NONE);
        return;
    }
}

ConnectionState Connection::state() const {
    return state_;
}


}
