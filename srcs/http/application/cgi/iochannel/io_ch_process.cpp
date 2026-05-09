#include "IOChannel.hpp"

namespace http {
    
    void IOChannel::process() {

        switch (stream) {
            case STDStream::STDERR: case STDStream::STDOUT:
                on_readable();
                break;
            case STDStream::STDIN:
                on_writeable();
                break;
            default:
                break;
        }
    }
}

