CXX := c++

FT := -pg  -finstrument-functions

DEBUG := -g3 -O0 -D DEBUG=1
DEVELOPMENT = -D DEV_MODE=1

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG) $(DEVELOPMENT) $(FT)

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

SRCDIR = src

OBJDIR := obj

NAME = webserv

INCLUDES = -Isrc \
	-Isrc/sys/ \
	-Isrc/net/ \
	-Isrc/net/connection/ \
	-Isrc/net/listener/ \
	-Isrc/cgi/ \
	-Isrc/cgi/process/ \
	-Isrc/io/ \
	-Isrc/io/stream/ \
	-Isrc/runtime/ \
	-Isrc/runtime/epoll/ \
	-Isrc/server/ \
	-Isrc/http/ \
	-Isrc/http/session/ \
	-Isrc/http/request/ \
	-Isrc/http/response/ \
	-Isrc/http/response/body/ \
	-Isrc/http/parser/ \
	-Isrc/http/execution/ \
	-Isrc/http/execution/handlers/ \
	-Isrc/http/common/ \
	-Isrc/http/routing/ \
	-Isrc/config/ \
	-Isrc/foundation/ \
	-Isrc/foundation/baselib \

SRCS = src/server/main.cpp \
	src/sys/signals.cpp \
	src/server/Server.cpp \
	src/net/listener/create_sockets.cpp \
	src/net/listener/listener_events.cpp \
	src/net/connection/Connection.cpp \
	src/net/listener/ListeningSocket.cpp \
	src/runtime/epoll/EventLoop.cpp \
	src/runtime/epoll/run.cpp \
	src/runtime/epoll/event_handlers.cpp \
	src/io/stream/Stream.cpp \
	src/config/ConfigParser.cpp \
	src/config/Lexer.cpp \
	src/foundation/DataView.cpp \
	src/foundation/BufferWriter.cpp \
	src/http/session/HttpSession.cpp \

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
