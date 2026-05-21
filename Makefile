CXX := c++

FT := -pg  -finstrument-functions

DEBUG := -g3 -O0 -D DEBUG=1
DEVELOPMENT = -D DEV_MODE=1

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG) $(DEVELOPMENT) $(FT)

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

SRCDIR = srcs

OBJDIR := obj

NAME = webserv

INCLUDES = -Isrc \
	-Isrc/net/ \
	-Isrc/net/connection/ \
	-Isrc/net/listener/ \
	-Isrc/cgi/ \
	-Isrc/cgi/process/ \
	-Isrc/io/ \
	-Isrcs/io/stream/ \
	-Isrc/runtime/ \
	-Isrc/runtime/epoll/ \
	-Isrc/server/ \
	-Isrc/http/ \
	-Isrcs/http/session \
	-Isrc/config/ \
	-Isrc/foundation/ \
	-Isrcs/foundation/baselib \

SRCS = $(SRCDIR)/main.cpp \
	srcs/core/Connection.cpp \
	srcs/core/process_incoming_data.cpp \
	srcs/core/process_outgoing_data.cpp \
	srcs/core/conn_cgi.cpp \
	srcs/core/conn_process.cpp \
	srcs/core/conn_handle_event.cpp \
	srcs/core/conn_request_phase.cpp \
	srcs/core/on_cgi_finished.cpp \
	srcs/foundation/DataView.cpp \
	srcs/foundation/BufferWriter.cpp \
	srcs/http/Headers.cpp \
	srcs/http/parser/Parser.cpp \
	srcs/http/parser/parse_headers.cpp \
	srcs/http/parser/parse_request_line.cpp \
	srcs/http/application/body_handler/read_body.cpp \
	srcs/http/parser/parser_utils.cpp \
	srcs/http/parser/Request.cpp \
	srcs/http/parser/parse_http_request.cpp \
	srcs/http/application/body_handler/detect_body.cpp \
	srcs/http/application/body_handler/parse_chunk_size.cpp \
	srcs/http/parser/LineScanner.cpp \
	srcs/http/application/body_handler/BodyHandler.cpp \
	srcs/http/application/body_handler/read_body_chunked.cpp \
	srcs/http/application/body_handler/read_body_content_length.cpp \
	srcs/http/application/dispatcher/Dispatcher.cpp \
	srcs/http/application/dispatcher/serialize_response.cpp \
	srcs/http/application/dispatcher/create_request_handler.cpp \
	srcs/http/application/dispatcher/find_location.cpp \
	srcs/http/application/dispatcher/resolve.cpp \
	srcs/http/application/cgi/CGIRequestHandler.cpp \
	srcs/http/application/cgi/spawn.cpp \
	srcs/http/application/cgi/parse_headers.cpp \
	srcs/http/application/cgi/on_input_ready.cpp \
	srcs/http/application/cgi/on_error.cpp \
	srcs/http/application/cgi/on_output_ready.cpp \
	srcs/http/application/cgi/iochannel/IOChannel.cpp \
	srcs/http/application/cgi/iochannel/io_channel_handle_event.cpp \
	srcs/http/application/cgi/iochannel/io_ch_process.cpp \
	srcs/http/application/cgi/iochannel/io_ch_process_data.cpp \
	srcs/http/application/cgi/iochannel_closed.cpp \
	srcs/http/application/response_builder/Response.cpp \
	srcs/http/application/response_builder/response_produce.cpp \
	srcs/http/application/response_builder/generate_directory_listing.cpp \
	srcs/http/application/response_builder/MimeType.cpp \
	srcs/http/application/body/CGIBodyProvider.cpp \
	srcs/http/application/body/send_body.cpp \
	srcs/http/application/body/body_chunked.cpp \
	srcs/io/create_sockets.cpp \
	srcs/io/EventLoop.cpp \
	srcs/io/handle_fds.cpp \
	srcs/io/run.cpp \
	srcs/io/event_loop_sweep.cpp \
	srcs/io/ListeningSocket.cpp \
	srcs/io/socket_events.cpp \
	srcs/io/handle_connections.cpp \
	srcs/io/stream/Stream.cpp \
	srcs/io/stream/stream_read.cpp \
	srcs/io/stream/stream_write.cpp \
	srcs/io/stream/stream_on_event.cpp \
	srcs/io/stream/stream_on_readable.cpp \
	srcs/io/stream/stream_on_writeable.cpp \
	srcs/config/ConfigParser.cpp \
	srcs/config/Lexer.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME) 
	
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
	@printf "\t%-30s -> %s\n" $(notdir $<) $(notdir $@)

$(NAME): $(OBJS)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)

clean:
	@rm -fr $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	@rm -fr 

re: fclean all

.PHONY: all clean fclean re
