CXX := c++

DEBUG := -g3 -O0

FT := -pg  -finstrument-functions

DEVELOPMENT = -D DEV_MODE=1 -D DEBUG=1

CXX_FLAGS := -Wall -Wextra -Werror -std=c++98 $(DEBUG) $(DEVELOPMENT)


# Libraries we might link against in the future, for now just a placeholder
LIBS := 

SRCDIR = src

BODY_DIR = ./srcs/http/parser/.body_dir

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

SRCS = src/server/main.cpp \
	src/sys/signals.cpp \
	src/server/Server.cpp \
	src/net/connection/Connection.cpp \
	src/net/listener/Listener.cpp \
	src/runtime/epoll/EventPoller.cpp \
	src/runtime/epoll/poll.cpp \
	src/runtime/epoll/event_handlers.cpp \
	src/cgi/process/Process.cpp \
	src/cgi/resolve_cgi.cpp \
	src/cgi/response_builder.cpp \
	src/cgi/handler/CGIRequestHandler.cpp \
	src/http/Context.cpp \
	src/http/pipeline/body/CGIBodyProvider.cpp \
	src/http/common/LineScanner.cpp \
	src/http/common/Headers.cpp \
	src/http/parser/parse_headers.cpp \
	src/http/parser/utils.cpp \
	src/http/parser/parse.cpp \
	src/http/parser/request_line.cpp \
	src/http/parser/headers/headers.cpp \
	src/http/parser/headers/header_rules.cpp \
	src/http/parser/headers/header_utils.cpp \
	src/http/parser/body.cpp \
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
	
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(LIBS) -c $< -o $@
	@printf "\t%-30s -> %s\n" $(notdir $<) $(notdir $@)

$(NAME): $(OBJS)
	@$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)
	@echo "Build complete: $(NAME)"


$(BODY_DIR):
	@mkdir $(BODY_DIR)

clean:
	@rm -fr $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	@rm -fr $(BODY_DIR)

re: fclean all

.PHONY: all clean fclean re
