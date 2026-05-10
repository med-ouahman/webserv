
#include "Connection.hpp"
#include "LineScanner.hpp"

namespace core {
  
    void Connection::process_incoming_data() {

        http::ResolutionResult r;
        http::ScanResult result = http::SUCCESS;

        while (true) {

            switch (phase) {

                case RequestPhase::BUILDING:
                    result = p.parse();
                    switch (result) {
                        case http::NEED_MORE:
                            return ;
                        case http::ERROR:
                            on_client_error();
                            return ;
                        case http::SUCCESS:
                            phase = RequestPhase::RESOLVING;
                            break;
                    }
                    break;

                case RequestPhase::RESOLVING:
                    r = dispatcher.resolve(p.get_request(), config.server);
                    if (r.type == http::HTTPResponseType::CGI) {
                        invoke_cgi(dispatcher.get_cgi_context(p.get_request(), r));
                        return ;
                    }
                    dispatcher.handle_request(r, p.get_request());
                    phase = RequestPhase::PROCESSING;
                    break;

                case RequestPhase::PROCESSING:
                    return ;
            }
        }
    }

}
