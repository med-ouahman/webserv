#pragma once

#include "AEventHandler.hpp"
#include "BufferReader.hpp"
#include "BufferWriter.hpp"

namespace http {

class CgiHandler;


class Channel: public io::AEventHandler {
public:

enum Stream {
	Stdin,
	Stdout,
	Stderr
};

enum State {
	Open,
	Closing,
	Closed,
	Error
};

private:
    const static std::size_t ReaderSize = 1024 * 4;
    const static std::size_t WriterSize = 1024 * 4;
	Stream stream_;
	State state_;
	CgiHandler& handler_;

	BufferReader reader_;
	BufferWriter writer_;

public:
	Channel(Stream s, int fd, io::Event events, CgiHandler& handler);
	~Channel();
	
	void on_event(io::Event ev);

	void read();
	void write();

	Stream stream() const;
	State state() const;
	void shutdown();
	void mark_closing();
};


}
