
#pragma once

/*


*/


#include <iostream>
#include "AEventHandler.hpp"
#include <unistd.h>
#include "Result.hpp"
#include "BufferReader.hpp"
#include "BufferWriter.hpp"

namespace io {

struct StreamControl {
    io::Event   events;
    bool        paused_;

    StreamControl(): events(io::NONE), paused_(false) {}
};

class IStreamDelegate {
public:
	virtual void consume(BufferReader& view) = 0;
	virtual void produce(BufferWriter& w) = 0;
	virtual void on_stream_error() = 0;
	virtual void on_stream_closed() = 0;
	virtual ~IStreamDelegate() {};
};

class Stream: public AEventHandler {

public:
	const static std::size_t READ_BUFFER_SIZE = 1024 * 4;
	const static std::size_t WRITE_BUFFER_SIZE = 1024 * 4;
	Stream(int fd, Event events_, IStreamDelegate& de);
	~Stream();
	void on_event(io::Event events_);
	BufferReader& reader() { return reader_; }
	void pause();
	void resume();

private:
	StreamControl ctl_;
	IStreamDelegate& delegate;
	BufferReader reader_;
	BufferWriter writer;
	void on_readable();
	void on_writeable();
};

}
