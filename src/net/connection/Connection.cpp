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
    register_ctx(regis_ctx)
  /*  ctx() */{}

Connection::~Connection() {
}

bool Connection::timedout() {

    /* in development */
    
    /*
    switch (ctx.state()) {
        case http::REQUEST_LINE:
        // timeout logic here 
        default:
        return false;
    }
    */

    return false;
}


void Connection::consume(DataView& view) {
    
}

void Connection::produce(BufferWriter& writer) {
    

    // Base::io::Writer w(writer.data(), writer.size());
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
