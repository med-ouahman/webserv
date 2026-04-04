CXX := c++

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 -g

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
	-Isrcs/http/response \
	-Isrcs/config \
	-Isrcs/utils \

SRCS = $(SRCDIR)/main.cpp \
	srcs/core/Connection.cpp \
	srcs/core/ConnectionStateMachine.cpp \
	srcs/core/connection_on_event.cpp \
	srcs/core/conn_on_bytes.cpp \
	srcs/core/conn_buff.cpp \
	srcs/core/conn_advance.cpp \
	srcs/http/parser/HTTPParser.cpp \
	srcs/http/parser/parse_headers.cpp \
	srcs/http/parser/parse_request_line.cpp \
	srcs/http/parser/parse_body.cpp \
	srcs/http/parser/parser_utils.cpp \
	srcs/http/parser/HTTPRequest.cpp \
	srcs/http/parser/parser_detect_body.cpp \
	srcs/http/response/HTTPResponseHandler.cpp \
	srcs/http/response/serialize_response.cpp \
	srcs/http/response/handle_request.cpp \
	srcs/http/response/find_location.cpp \
	srcs/http/response/resolve.cpp \
	srcs/http/response/handler_produce.cpp \
	srcs/io/handle_connections.cpp \
	srcs/io/create_sockets.cpp \
	srcs/io/EventLoop.cpp \
	srcs/io/handle_fds.cpp \
	srcs/io/run.cpp \
	srcs/io/ListeningSocket.cpp \
	srcs/io/socket_events.cpp \
	srcs/io/handle_connection_event.cpp \
	srcs/config/ConfigParser.cpp \

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
