#include "CGIEnvBuilder.hpp"

namespace cgi {

const char* CGIEnvBuilder::cgi_metadata[] = {"REQUEST_METHOD", "SERVER_PROTOCOL", "QUERY_STRING", NULL};
const char* CGIEnvBuilder::stripped_headers[] = {"transfer-encoding", "content-length", "content-type", "connection", NULL};

}
