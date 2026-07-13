CXX := c++

DEBUG := -g3 -O0

FT := -pg  -finstrument-functions

DEVELOPMENT = -D DEV_MODE=1 -D DEBUG=1

STD = -std=c++98

CXX_FLAGS := -Wall -Wextra -Werror $(STD) $(DEBUG) $(DEVELOPMENT)

# Libraries we might link against in the future, for now just a placeholder
LIBS := 

SRCDIR := src

OBJDIR := obj

NAME = webserv

INCLUDES = -Isrc \
	-Isrc/net/ \
	-Isrc/net/connection/ \
	-Isrc/net/listener/ \
	-Isrc/runtime/ \
	-Isrc/runtime/epoll/ \
	-Isrc/server/ \
	-Isrc/server/logger/ \
	-Isrc/cgi/ \
	-Isrc/http/ \
	-Isrc/http/Parser/ \
	-Isrc/http/Parser/body/ \
	-Isrc/http/Parser/headers/ \
	-Isrc/http/routing/ \
	-Isrc/http/pipeline/ \
	-Isrc/http/pipeline/handlers \
	-Isrc/http/common/ \
	-Isrc/config/ \
	-Isrc/foundation/ \
	-Isrc/base \

SRCS = src/server/main.cpp \
	src/server/signals.cpp \
	src/server/Server.cpp \
	src/server/logger/Logger.cpp \
	src/net/connection/Connection.cpp \
	src/net/connection/connection_io.cpp \
	src/net/connection/connection_events.cpp \
	src/http/pipeline/handlers/CgiHandler.cpp \
	src/net/listener/Listener.cpp \
	src/runtime/epoll/EventLoop.cpp \
	src/runtime/epoll/poll.cpp \
	src/runtime/epoll/events.cpp \
	src/cgi/Process.cpp \
	src/cgi/resolve_cgi.cpp \
	src/cgi/ResponseParser.cpp \
	src/http/Context.cpp \
	src/http/common/LineReader.cpp \
	src/http/common/Headers.cpp \
	src/http/Parser/parser.cpp \
	src/http/Parser/request_line.cpp \
	src/http/Parser/headers/headers.cpp \
	src/http/Parser/headers/header_rules.cpp \
	src/http/Parser/headers/header_utils.cpp \
	src/http/Parser/body/body.cpp \
	src/http/Parser/body/body_store.cpp \
	src/http/Parser/body/body_utils.cpp \
	src/http/Parser/body/chunked.cpp \
	src/http/Parser/body/fixed.cpp \
	src/http/routing/Routing.cpp \
	src/config/ConfigParser.cpp \
	src/config/Lexer.cpp \
	src/base/io/Writer.cpp \
	src/base/io/Reader.cpp \
	src/base/file.cpp \
	src/base/random.cpp \

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
	@rm -fr $(BODY_DIR)

re: fclean all

.PHONY: all clean fclean re
