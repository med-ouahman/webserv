#include "Connection.hpp"
#include "Context.hpp"

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
    /* cp */
}

void Connection::consume(BufferReader& view) {
    // ctx.consume(view.data(), view.size());
    std::cout << view.data(), view.size();std::cout << "\n";
    state_ = WRITING;
}

void Connection::produce(BufferWriter& writer) {
    if (close_after_write) {
        state_ = CLOSING;
        return;
    }
    std::string s = "writer.write(\"Hello World\n\", );\n";
    writer.write(s.c_str(), s.size());
    close_after_write = true;
}

ConnectionState Connection::state() const {
    return state_;
}

}
