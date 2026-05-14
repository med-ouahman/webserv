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

INCLUDES = -Iincludes \
	-Isrcs/core/ \
	-Isrcs/io/ \
	-Isrcs/http/ \
	-Isrcs/http/parser \
	-Isrcs/http/application \
	-Isrcs/http/application/body \
	-Isrcs/http/application/cgi \
	-Isrcs/http/application/dispatcher \
	-Isrcs/http/application/response_builder \
	-Isrcs/http/application/cgi/iochannel \
	-Isrcs/http/application/body_handler \
	-Isrcs/io/stream/ \
	-Isrcs/config/ \
	-Isrcs/utils/ \

SRCS = $(SRCDIR)/main.cpp \
	srcs/core/Connection.cpp \
	srcs/core/process_incoming_data.cpp \
	srcs/core/process_outgoing_data.cpp \
	srcs/core/conn_cgi.cpp \
	srcs/core/conn_process.cpp \
	srcs/core/conn_handle_event.cpp \
	srcs/core/conn_request_phase.cpp \
	srcs/core/on_cgi_finished.cpp \
	srcs/core/DataView.cpp \
	srcs/core/BufferWriter.cpp \
	srcs/http/parser/HTTPParser.cpp \
	srcs/http/parser/parse_headers.cpp \
	srcs/http/parser/parse_request_line.cpp \
	srcs/http/application/body_handler/read_body.cpp \
	srcs/http/parser/parser_utils.cpp \
	srcs/http/parser/HTTPRequest.cpp \
	srcs/http/parser/http_parse.cpp \
	srcs/http/application/body_handler/detect_body.cpp \
	srcs/http/application/body_handler/parse_chunk_size.cpp \
	srcs/http/parser/LineScanner.cpp \
	srcs/http/application/body_handler/BodyHandler.cpp \
	srcs/http/application/body_handler/read_body_chunked.cpp \
	srcs/http/application/body_handler/read_body_content_length.cpp \
	srcs/http/application/dispatcher/HTTPDispatcher.cpp \
	srcs/http/application/dispatcher/serialize_response.cpp \
	srcs/http/application/dispatcher/handle_request.cpp \
	srcs/http/application/dispatcher/find_location.cpp \
	srcs/http/application/dispatcher/resolve.cpp \
	srcs/http/application/dispatcher/get_cgi_context.cpp \
	srcs/http/application/handler_produce.cpp \
	srcs/http/application/cgi/CGIHandler.cpp \
	srcs/http/application/cgi/spawn.cpp \
	srcs/http/application/cgi/cgi_input.cpp \
	srcs/http/application/cgi/cgi_on_error.cpp \
	srcs/http/application/cgi/cgi_output.cpp \
	srcs/http/application/cgi/cgi_pull.cpp \
	srcs/http/application/cgi/iochannel/IOChannel.cpp \
	srcs/http/application/cgi/iochannel/io_channel_handle_event.cpp \
	srcs/http/application/cgi/iochannel/io_ch_process.cpp \
	srcs/http/application/cgi/iochannel/io_ch_process_data.cpp \
	srcs/http/application/cgi/cgi_iochannel_closed.cpp \
	srcs/http/application/response_builder/generate_directory_listing.cpp \
	srcs/http/application/response_builder/MimeType.cpp \
	srcs/http/application/body/CGIBodyProvider.cpp \
	srcs/http/application/body/cgi_send_body.cpp \
	srcs/http/application/body/cgi_body_chunked.cpp \
	srcs/io/create_sockets.cpp \
	srcs/io/EventLoop.cpp \
	srcs/io/handle_fds.cpp \
	srcs/io/run.cpp \
	srcs/io/ListeningSocket.cpp \
	srcs/io/socket_events.cpp \
	srcs/io/handle_connections.cpp \
	srcs/io/register_handlers.cpp \
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
