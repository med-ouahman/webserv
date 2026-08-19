[1mdiff --git a/src/base/io/Reader.cpp b/src/base/io/Reader.cpp[m
[1mindex 2e26032..d8dd3c2 100644[m
[1m--- a/src/base/io/Reader.cpp[m
[1m+++ b/src/base/io/Reader.cpp[m
[36m@@ -3,7 +3,6 @@[m
 #include <cstring>[m
 #include <fcntl.h>[m
 #include <unistd.h>[m
[31m-#include"iostream"[m
 namespace base {[m
 namespace io {[m
 [m
[36m@@ -88,8 +87,7 @@[m [mbool Reader::reset(const char* buffer, usize size) {[m
 [m
 base::Expected<usize, Error> Reader::read(char* buff, usize max_size) {[m
 	[m
[31m-	if (type_ == NONE) { [m
[31m-		std::cout << "THERE IS SOMETHING IN THE SKY\n";[m
[32m+[m	[32mif (type_ == NONE) {[m
 		return 0;[m
 	}[m
 [m
[1mdiff --git a/src/cgi/Channel.cpp b/src/cgi/Channel.cpp[m
[1mindex 2b1237b..a809c46 100644[m
[1m--- a/src/cgi/Channel.cpp[m
[1m+++ b/src/cgi/Channel.cpp[m
[36m@@ -6,36 +6,20 @@[m [mnamespace cgi {[m
 [m
 Channel::~Channel() {}[m
 [m
[31m-std::string channel_type(Channel::Stream s) {[m
[31m-[m
[31m-    if (s == Channel::Stdin) {[m
[31m-        return "Stdin";[m
[31m-    }[m
[31m-[m
[31m-    if (s == Channel::Stdout) return "Stdout";[m
[31m-[m
[31m-    return "Stderr";[m
[31m-}[m
[31m-[m
 void Channel::on_event(io::Event event) {[m
 [m
     size_t w = 0;[m
[31m-    // std::string s = channel_type(stream_);[m
     switch (event) {[m
         case io::Readable: case io::Hup:[m
[31m-            // std::cout << "Channel Readable: " << s << "\n";[m
             w = handler_.on_readable(*this);[m
             buf.advance_read(w);[m
             break;[m
         case io::Writable:[m
[31m-            // std::cout << "Channel Writable" << s <<"\n";[m
             w = handler_.on_writable(buf, *this);[m
             break;[m
         case io::RHup:[m
[31m-            // std::cout << "Channel ReadEnd hangup\n";[m
             state_ = Closing; break;[m
         case io::Error:[m
[31m-            // std::cout << "Channel Error\n";[m
             state_ = Closing; break;[m
         default: break;[m
     }[m
[1mdiff --git a/src/cgi/Process.cpp b/src/cgi/Process.cpp[m
[1mindex cd06e53..c3a5b44 100644[m
[1m--- a/src/cgi/Process.cpp[m
[1m+++ b/src/cgi/Process.cpp[m
[36m@@ -2,7 +2,6 @@[m
 #include <csignal>[m
 #include <sys/wait.h>[m
 #include <cstdlib>[m
[31m-#include <iostream>[m
 #include "Result.hpp"[m
 #include "CGIContext.hpp"[m
 #include <cstdio>[m
[36m@@ -99,7 +98,8 @@[m [mbool Process::start(const ProcessContext& context) {[m
 [m
     if (state_ != Spawn) return true;[m
 [m
[31m-    if (context.stdin_fd.get() != STDIN_FILENO) stdin_pipe_.close_write_end();[m
[32m+[m[32m    if (context.stdin_fd.get() != STDIN_FILENO)[m
[32m+[m[32m        stdin_pipe_.close_write_end();[m
 [m
     pid_ = ::fork();[m
     if (pid_ == 0) {[m
[1mdiff --git a/src/cgi/ResponseParser.cpp b/src/cgi/ResponseParser.cpp[m
[1mindex 6b7a3f3..de4af70 100644[m
[1m--- a/src/cgi/ResponseParser.cpp[m
[1m+++ b/src/cgi/ResponseParser.cpp[m
[36m@@ -1,5 +1,4 @@[m
 #include "ResponseParser.hpp"[m
[31m-#include <iostream>[m
 #include "http/Parser/Parser.hpp"[m
 #include <cstdlib>[m
 #include <fcntl.h>[m
[36m@@ -30,8 +29,6 @@[m [mResponseParser::~ResponseParser() {[m
 }[m
 [m
 ResponseParser::ParseResult ResponseParser::parse(BufferView& reader) {[m
[31m-    // std::cout << "Start CGI header parsing...\n";[m
[31m-[m
     if (reader.empty() && state_ == Headers) {        [m
         return ParseError;[m
     }[m
[36m@@ -53,7 +50,6 @@[m [mResponseParser::ParseResult ResponseParser::parse(BufferView& reader) {[m
         [m
         if (parse_ctx.line_reader.line().empty()) {[m
             if (!validate_headers()) return ParseError;[m
[31m-            // std::cout << "Begin body reading\n";[m
             state_ = Body;[m
             continue;[m
         }[m
[36m@@ -62,25 +58,21 @@[m [mResponseParser::ParseResult ResponseParser::parse(BufferView& reader) {[m
         [m
         ParseResult res = parse_header(parse_ctx.line_reader.line());[m
         if (res != Success) {[m
[31m-            // if (res == ParseError) std::cout << "CGI parse error\n";[m
             return res;[m
         }[m
 [m
         parse_ctx.line_reader.reset();[m
     }[m
 [m
[31m-    // std::cout << "finish CGI\n";[m
     return Success;[m
 }[m
 [m
 ResponseParser::ParseResult ResponseParser::sanitize_status_header(std::string const& value) {[m
 [m
[31m-    // std::cout << "Sanitizing status header\n";[m
     size_t space_pos = value.find(' ');[m
 [m
     if (space_pos == std::string::npos) {[m
         code = INTERNAL_SERVER_ERROR;[m
[31m-        // std::cout << "Invalid status header\n";[m
         return ParseError;[m
     }[m
 [m
[36m@@ -89,7 +81,6 @@[m [mResponseParser::ParseResult ResponseParser::sanitize_status_header(std::string c[m
 [m
     for (size_t i = 0; i < code_str.size(); ++i) {[m
         if (!std::isdigit(code_str[i])) {[m
[31m-            // std::cout << "Invalid status code\n";[m
             code = INTERNAL_SERVER_ERROR;[m
             return ParseError;[m
         }[m
[36m@@ -101,7 +92,6 @@[m [mResponseParser::ParseResult ResponseParser::sanitize_status_header(std::string c[m
 [m
     if ((end && *end != '\0') || parsed_code < 200 or parsed_code > 599) {[m
         code = INTERNAL_SERVER_ERROR;[m
[31m-        // std::cout << "Status code out of range\n";[m
         return ParseError;[m
     }[m
 [m
[36m@@ -115,8 +105,6 @@[m [mbool ResponseParser::finished() const {[m
 }[m
 [m
 ResponseParser::ParseResult ResponseParser::parse_header(std::string const& line) {[m
[31m-    // std::cout << "|" << line << "|\n";[m
[31m-[m
     size_t colon = line.find(':');[m
     if (colon == std::string::npos)[m
         return ParseError;[m
[36m@@ -153,10 +141,7 @@[m [mResponseParser::ParseResult ResponseParser::parse_header(std::string const& line[m
 }[m
 ResponseParser::ParseResult ResponseParser::read_body(BufferView& reader) {[m
 [m
[31m-    // std::cout.write(reader.data(), reader.remaining());[m
[31m-[m
     if (reader.empty()) {[m
[31m-        // std::cout << "Body Done\n";[m
         state_ = Done;[m
         return Success;[m
     }[m
[36m@@ -183,13 +168,11 @@[m [mResponseParser::ParseResult ResponseParser::read_body(BufferView& reader) {[m
         body_fd = ::open(body_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);[m
         if (body_fd < 0) {[m
             state_ = Error;[m
[31m-            // std::cout << "File error\n";[m
             return ParseError;[m
         }[m
         if (!body_.empty()) {[m
             ssize_t w = ::write(body_fd, body_.c_str(), body_.size());[m
             if (w < 0) {[m
[31m-                // std::cout << "write error\n";[m
                 state_ = Error;[m
                 return ParseError;[m
             }[m
[36m@@ -200,7 +183,6 @@[m [mResponseParser::ParseResult ResponseParser::read_body(BufferView& reader) {[m
 [m
     ssize_t w = ::write(body_fd, reader.data(), reader.remaining());[m
     if (w < 0) {[m
[31m-        // std::cout << "Another write error\n";[m
         state_ = Error;[m
         return ParseError;[m
     }[m
[1mdiff --git a/src/http/pipeline/handlers/CgiHandler.cpp b/src/http/pipeline/handlers/CgiHandler.cpp[m
[1mindex d24ab86..f8b450c 100644[m
[1m--- a/src/http/pipeline/handlers/CgiHandler.cpp[m
[1m+++ b/src/http/pipeline/handlers/CgiHandler.cpp[m
[36m@@ -4,57 +4,6 @@[m
 [m
 #include "runtime/epoll/EventLoop.hpp"[m
 #include "Context.hpp"[m
[31m-[m
[31m-[m
[31m-#include <iostream>[m
[31m-[m
[31m-void log_cgi_request_context(const cgi::CGIRequestContext& ctx)[m
[31m-{[m
[31m-    (void)ctx;[m
[31m-    /*[m
[31m-    std::cout[m
[31m-        << "===== CGI Request Context =====\n"[m
[31m-        << "REQUEST_METHOD : " << ctx.request_method   << '\n'[m
[31m-        << "MIME_TYPE      : " << ctx.mime_type        << '\n'[m
[31m-        << "INTERPRETER    : " << ctx.interpreter      << '\n'[m
[31m-        << "SCRIPT_NAME    : " << ctx.script_name      << '\n'[m
[31m-        << "QUERY_STRING   : " << ctx.query_string     << '\n'[m
[31m-        << "CONTENT_LENGTH : " << ctx.content_length   << '\n'[m
[31m-        << "PATH_INFO      : " << ctx.path_info        << '\n'[m
[31m-        << "SERVER_NAME    : " << ctx.server_name      << '\n'[m
[31m-        << "SERVER_PROTOCOL: " << ctx.server_protocol  << '\n'[m
[31m-        << "SERVER_PORT    : " << ctx.server_port      << '\n'[m
[31m-        << "TIMEOUT        : " << ctx.timeout << " s\n"[m
[31m-        << "===============================\n";[m
[31m-    */[m
[31m-}[m
[31m-[m
[31m-void log_process_context(const cgi::ProcessContext& ctx)[m
[31m-{[m
[31m-    (void)ctx;[m
[31m-    /*[m
[31m-    std::cout[m
[31m-        << "======== Process Context ========\n"[m
[31m-        << "WORKING_DIR : " << ctx.working_dir << '\n'[m
[31m-        << "STDIN_FD    : " << ctx.stdin_fd.get() << '\n';[m
[31m-[m
[31m-    std::cout << "ARGV (" << ctx.argv.size() << ")\n";[m
[31m-    for (std::size_t i = 0; i < ctx.argv.size(); ++i)[m
[31m-    {[m
[31m-        std::cout << "  [" << i << "] " << ctx.argv.data()[i] << '\n';[m
[31m-    }[m
[31m-[m
[31m-    std::cout << "ENVP (" << ctx.envp.size() << ")\n";[m
[31m-    for (std::size_t i = 0; i < ctx.envp.size(); ++i)[m
[31m-    {[m
[31m-        std::cout << "  [" << i << "] " << ctx.envp.data()[i] << '\n';[m
[31m-    }[m
[31m-[m
[31m-    std::cout[m
[31m-        << "=================================\n";[m
[31m-    */[m
[31m-}[m
[31m-[m
 namespace cgi {[m
     [m
 template <size_t N>[m
[36m@@ -168,8 +117,6 @@[m [msize_t CgiHandler::on_readable(cgi::Channel& channel) {[m
     }[m
 [m
     if (channel.stream() == cgi::Channel::Stderr) {[m
[31m-        // std::cout << "Size: " <<  view.remaining() << "\n";[m
[31m-        // std::cout.write(view.data(), view.remaining());[m
         view.advance(view.remaining());[m
         close_channel(channel);[m
         return view.cursor();[m
[36m@@ -182,11 +129,8 @@[m [msize_t CgiHandler::on_readable(cgi::Channel& channel) {[m
     if (r == ResponseParser::ParseError) {[m
         reason_ = ParseError;[m
         response_state = Error;[m
[31m-        // std::cout << "hERE?? Error\n";[m
         return view.cursor();[m
     }[m
[31m-[m
[31m-    // std::cout << "CGI REQUEST DONE\n";[m
     [m
     close_channel(channel);[m
     response_state = Finished;[m
