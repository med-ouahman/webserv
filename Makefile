	CXX := c++

DEBUG := -g3 -O0
CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG)

<<<<<<< HEAD
DEBUG := -g3 -O0
CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG)
=======
FT := -pg  -finstrument-functions

DEBUG := -g3 -O0 -D DEBUG=1
DEVELOPMENT = -D DEV_MODE=1

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG) $(DEVELOPMENT) $(FT)
>>>>>>> fba701ffe56069d2759bc19b86233db7fe442d5c

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

<<<<<<< HEAD
=======
SRCDIR = src

BODY_DIR = ./srcs/http/parser/.body_dir

>>>>>>> shady
OBJDIR := obj

NAME = webserv

INCLUDES = -Isrc \
	-Isrc/sys/ \
	-Isrc/net/ \
	-Isrc/net/connection/ \
	-Isrc/net/listener/ \
	-Isrc/cgi/ \
	-Isrc/cgi/process/ \
	-Isrc/cgi/handler/ \
	-Isrc/io/ \
	-Isrc/io/stream/ \
	-Isrc/runtime/ \
	-Isrc/runtime/epoll/ \
	-Isrc/server/ \
	-Isrc/http/ \
	-Isrc/http/pipeline/ \
	-Isrc/http/pipeline/body/ \
	-Isrc/http/common/ \
	-Isrc/config/ \
	-Isrc/foundation/ \
	-Isrc/base \

<<<<<<< HEAD
SRCS = $(SRCDIR)/main.cpp \
	srcs/core/Connection.cpp \
	srcs/core/ConnectionStateMachine.cpp \
	srcs/core/connection_on_event.cpp \
	srcs/core/process_incoming_data.cpp \
	srcs/core/conn_on_write.cpp \
	srcs/core/conn_advance.cpp \
	srcs/http/parser/HTTPParser.cpp \
	srcs/http/parser/parse_headers.cpp \
	srcs/http/parser/parse_request_line.cpp \
	srcs/http/parser/parse_body.cpp \
	srcs/http/parser/parser_utils.cpp \
	srcs/http/parser/HTTPRequest.cpp \
	srcs/http/parser/parser_scan_line.cpp \
	srcs/http/parser/parser_consume.cpp \
	srcs/http/parser/parser_detect_body.cpp \
	srcs/http/response/HTTPResponseHandler.cpp \
	srcs/http/response/serialize_response.cpp \
	srcs/http/response/handle_request.cpp \
	srcs/http/response/find_location.cpp \
	srcs/http/response/resolve.cpp \
	srcs/http/response/handler_produce.cpp \
	srcs/http/response/CGIHandler.cpp \
	srcs/http/response/generate_directory_listing.cpp \
	srcs/io/create_sockets.cpp \
	srcs/io/EventLoop.cpp \
	srcs/io/handle_fds.cpp \
	srcs/io/run.cpp \
	srcs/io/ListeningSocket.cpp \
	srcs/io/socket_events.cpp \
	srcs/io/handle_connections.cpp \
	srcs/io/handle_connection_event.cpp \
	srcs/config/ConfigParser.cpp \
	srcs/config/Lexer.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME) $(BODY_DIR)

a: $(NAME)
	@clear
	@./webserv
=======
SRCS = src/server/main.cpp \
	src/sys/signals.cpp \
	src/server/Server.cpp \
	src/net/connection/Connection.cpp \
	src/net/connection/connection_stream.cpp \
	src/net/listener/Listener.cpp \
	src/runtime/epoll/EventPoller.cpp \
	src/runtime/epoll/poll.cpp \
	src/runtime/epoll/event_handlers.cpp \
	src/cgi/process/Process.cpp \
	src/cgi/resolve_cgi.cpp \
	src/cgi/response_builder.cpp \
	src/cgi/handler/CGIRequestHandler.cpp \
	src/io/stream/Stream.cpp \
	src/http/Context.cpp \
	src/http/pipeline/body/CGIBodyProvider.cpp \
	src/http/common/LineScanner.cpp \
	src/http/common/Headers.cpp \
	src/http/parser/parse_headers.cpp \
	src/http/parser/utils.cpp \
	src/http/Request.cpp \
	src/http/Response.cpp \
	src/config/ConfigParser.cpp \
	src/config/Lexer.cpp \
	src/foundation/BufferReader.cpp \
	src/foundation/BufferWriter.cpp \
	src/base/io/Writer.cpp \
	src/base/io/Reader.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME) 
>>>>>>> fba701ffe56069d2759bc19b86233db7fe442d5c
	
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
	@printf "\t%-30s -> %s\n" $(notdir $<) $(notdir $@)

$(NAME): $(OBJS)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)
<<<<<<< HEAD
	@echo "Build complete: $(NAME)"
=======

$(BODY_DIR):
	@mkdir $(BODY_DIR)

>>>>>>> shady
clean:
	@rm -fr $(OBJDIR)

fclean: clean
<<<<<<< HEAD
	rm -f $(NAME)
	rm -fr $(BODY_DIR)
=======
	@rm -f $(NAME)
<<<<<<< HEAD
=======
	@rm -fr 
>>>>>>> fba701ffe56069d2759bc19b86233db7fe442d5c
>>>>>>> shady

re: fclean all

.PHONY: all clean fclean re
