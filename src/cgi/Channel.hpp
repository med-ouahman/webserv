#pragma once

#include "Buffer.hpp"
#include "BufferView.hpp"
#include "runtime/AEventHandler.hpp"

namespace http { class CgiHandler; }

namespace cgi {
class Channel: public io::AEventHandler {
public:
enum Stream {
	Stdin,
	Stdout,
	Stderr
};

enum State {
	Init,
	Open,
	Closing,
	Closed,
	Error
};

private:
	Stream stream_;
	State state_;
	http::CgiHandler& handler_;
	Buffer buf;

public:
template <size_t N>
Channel(Storage<N>& storage,
	Stream s,
	int fd,
	io::Event events,
	http::CgiHandler& handler);

~Channel();

void on_event(io::Event ev);
void read();
void write();
Stream stream() const;
State state() const;
bool closed() const;
void shutdown();
void mark_closing();
BufferView view() const;

};

}