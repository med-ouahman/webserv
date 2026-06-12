#pragma once

#include "AEventHandler.hpp"
#include "BufferReader.hpp"
#include "BufferWriter.hpp"

namespace http {

class CGIRequestHandler;


class Channel: public io::AEventHandler {
public:

enum Stream {
	Stdin,
	Stdout,
	Stderr
};

enum State {
	Open,
	Closed,
	Error
};

private:
    const static std::size_t ReaderSize = 1024 * 4;
    const static std::size_t WriterSize = 1024 * 4;
	Stream stream_;
	State state_;
	CGIRequestHandler& handler_;

	BufferReader reader_;
	BufferWriter writer_;

public:
	Channel(Stream s, int fd, io::Event events, CGIRequestHandler& handler);
	~Channel();
	
	void on_event(io::Event ev);

	Stream stream() const;
};


}
