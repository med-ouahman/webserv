CXX := c++

DEBUG := -g3 -O0
CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG)

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

SRCDIR = srcs

BODY_DIR = ./srcs/http/parser/.body_dir

OBJDIR := obj

NAME = webserv

INCLUDES = -Iincludes \
	-Isrcs/core \
	-Isrcs/io \
	-Isrcs/http \
	-Isrcs/http/parser \
	-Isrcs/http/application \
	-Isrcs/http/application/body \
	-Isrcs/http/application/cgi \
	-Isrcs/http/application/dispatcher \
	-Isrcs/http/application/response_builder \
	-Isrcs/config \
	-Isrcs/utils \

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
	srcs/http/application/dispatcher/HTTPDispatcher.cpp \
	srcs/http/application/dispatcher/serialize_response.cpp \
	srcs/http/application/dispatcher/handle_request.cpp \
	srcs/http/application/dispatcher/find_location.cpp \
	srcs/http/application/dispatcher/resolve.cpp \
	srcs/http/application/handler_produce.cpp \
	srcs/http/application/cgi/CGIHandler.cpp \
	srcs/http/application/generate_directory_listing.cpp \
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
	
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)

$(BODY_DIR):
	@mkdir $(BODY_DIR)

clean:
	rm -fr $(OBJDIR)

fclean: clean
	rm -f $(NAME)
	rm -fr $(BODY_DIR)

re: fclean all

.PHONY: all clean fclean re
