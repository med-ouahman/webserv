#include "IOChannel.hpp"

namespace http {
    
    bool IOChannel::process() {


        while (processing) {
            
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
        
        return false;
    }

}

