CXX := c++

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 -g

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

SRCDIR = srcs

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
	srcs/core/ConnectionEventHandler.cpp \
	srcs/http/parser/HTTPParser.cpp \
	srcs/http/parser/parse_headers.cpp \
	srcs/http/parser/parse_request_line.cpp \
	srcs/http/parser/parse_body.cpp \
	srcs/http/parser/parser_utils.cpp \
	srcs/http/response/HTTPResponseHandler.cpp \
	srcs/io/HandleConnectionEvent.cpp \
	srcs/io/EventLoopCreateSocket.cpp \
	srcs/io/EventLoop.cpp \
	srcs/io/EventLoopFds.cpp \
	srcs/io/EventLoopRun.cpp \
	srcs/io/ListeningSocket.cpp \
	srcs/io/ListeningSocketEvents.cpp \
	srcs/io/EventLoopConnections.cpp \
	srcs/config/ConfigParser.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

a: $(NAME)
	@clear
	@./webserv
	
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)

clean:
	rm -fr $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
