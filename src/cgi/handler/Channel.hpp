#pragma once

#include "AEventHandler.hpp"
#include "BufferReader.hpp"
#include "BufferWriter.hpp"

namespace http {

class CGIRequestHandler;

class Channel: public io::AEventHandler {
public:

enum Role {
	Stdin,
	Stdout,
	Stderr
};

private:
    const static std::size_t ReaderSize = 1024 * 4;
    const static std::size_t WriterSize = 1024 * 4;
	Role role_;
	CGIRequestHandler& handler_;
	BufferReader reader_;
	BufferWriter writer_;

public:
	Channel(int fd, io::Event events, CGIRequestHandler& handler);
	~Channel();
	void on_event(io::Event ev);
	Role role() const;
};


}
