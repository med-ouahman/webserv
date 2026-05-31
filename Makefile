CXX := c++

DEBUG := -g3 -O0
CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG)

FT := -pg  -finstrument-functions

DEBUG := -g3 -O0 -D DEBUG=1
DEVELOPMENT = -D DEV_MODE=1

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG) $(DEVELOPMENT) $(FT)

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

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
	-Isrc/http/common/ \
	-Isrc/config/ \
	-Isrc/foundation/ \
	-Isrc/foundation/baselib \

SRCS = src/server/main.cpp \
	src/sys/signals.cpp \
	src/server/Server.cpp \
	src/net/connection/Connection.cpp \
	src/net/listener/Listener.cpp \
	src/runtime/epoll/EventLoop.cpp \
	src/runtime/epoll/run.cpp \
	src/runtime/epoll/event_handlers.cpp \
	src/cgi/process/Process.cpp \
	src/cgi/build_envp.cpp \
	src/cgi/response_builder.cpp \
	src/cgi/handler/CGIRequestHandler.cpp \
	src/io/stream/Stream.cpp \
	src/http/common/LineScanner.cpp \
	src/http/common/Headers.cpp \
	src/http/parser/parse_headers.cpp \
	src/http/parser/utils.cpp \
	src/http/Request.cpp \
	src/http/Response.cpp \
	src/config/ConfigParser.cpp \
	src/config/Lexer.cpp \
	src/foundation/DataView.cpp \
	src/foundation/BufferWriter.cpp \
	src/foundation/baselib/io/Writer.cpp \
	src/foundation/baselib/io/Reader.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME) 
	
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
	@printf "\t%-30s -> %s\n" $(notdir $<) $(notdir $@)

$(NAME): $(OBJS)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)
	@echo "Build complete: $(NAME)"
clean:
	@rm -fr $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
