CXX := c++

DEBUG := -g3 -O0

FT := -pg  -finstrument-functions

DEVELOPMENT = -D DEV_MODE=1 -D DEBUG=1

STD = -std=c++98

CXX_FLAGS := -Wall -Wextra -Werror $(STD) $(DEBUG) $(DEVELOPMENT)

# Libraries we might link against in the future, for now just a placeholder
LIBS :=

SRCDIR := ./src/

OBJDIR := obj

NAME = webserv

INCLUDES = -Isrc/ \
	-Isrc/net/ \
	-Isrc/net/connection/ \
	-Isrc/net/socket/ \
	-Isrc/runtime/ \
	-Isrc/runtime/epoll/ \
	-Isrc/server/ \
	-Isrc/server/logger/ \
	-Isrc/http/ \
	-Isrc/http/session/ \
	-Isrc/http/Request/ \
	-Isrc/http/Response/ \
	-Isrc/http/Parser/ \
	-Isrc/http/Parser/body/ \
	-Isrc/http/Parser/headers/ \
	-Isrc/http/routing/ \
	-Isrc/http/pipeline/ \
	-Isrc/http/pipeline/handlers/ \
	-Isrc/http/pipeline/handlers/cgi/ \
	-Isrc/http/common/ \
	-Isrc/config/ \
	-Isrc/foundation/ \
	-Isrc/base/ \

SRCS = src/server/main.cpp \
	src/server/signals.cpp \
	src/server/Server.cpp \
	src/server/logger/Logger.cpp \
	src/net/connection/Connection.cpp \
	src/net/connection/connection_io.cpp \
	src/net/connection/connection_events.cpp \
	src/net/socket/Socket.cpp \
	src/runtime/epoll/EventLoop.cpp \
	src/runtime/epoll/poll.cpp \
	src/runtime/epoll/events.cpp \
	src/cgi/Process.cpp \
	src/cgi/ResponseParser.cpp \
	src/cgi/CGIContext.cpp \
	src/cgi/Channel.cpp \
	src/http/session/SessionManager.cpp \
	src/http/session/CookieUtils.cpp \
	src/http/Context.cpp \
	src/http/ContextInternal.cpp \
	src/http/Request/Request.cpp \
	src/http/Response/Response.cpp \
	src/http/common/LineReader.cpp \
	src/http/common/Headers.cpp \
	src/http/Parser/Parser.cpp \
	src/http/Parser/request_line.cpp \
	src/http/Parser/headers/headers.cpp \
	src/http/Parser/headers/header_rules.cpp \
	src/http/Parser/headers/header_utils.cpp \
	src/http/Parser/body/body.cpp \
	src/http/Parser/body/body_store.cpp \
	src/http/Parser/body/body_utils.cpp \
	src/http/Parser/body/chunked.cpp \
	src/http/Parser/body/fixed.cpp \
	src/http/Parser/body/temp_storage.cpp \
	src/http/routing/Routing.cpp \
	src/http/routing/server.cpp \
	src/http/routing/handlers.cpp \
	src/http/routing/checks.cpp \
	src/http/routing/normalization.cpp \
	src/http/routing/path.cpp \
	src/http/pipeline/pipeline.cpp \
	src/http/pipeline/ARequestHandler.cpp \
	src/http/pipeline/handlers/DeleteHandler.cpp \
	src/http/pipeline/handlers/DirectoryHandler.cpp \
	src/http/pipeline/handlers/ErrorHandler.cpp \
	src/http/pipeline/handlers/RedirectHandler.cpp \
	src/http/pipeline/handlers/StaticFileHandler.cpp \
	src/http/pipeline/handlers/UploadHandler.cpp \
	src/http/pipeline/handlers/CgiHandler.cpp \
	src/http/pipeline/handlers/CounterHandler.cpp \
	src/http/pipeline/handlers/LoginHandler.cpp \
	src/config/ConfigParser.cpp \
	src/config/Lexer.cpp \
	src/base/io/Writer.cpp \
	src/base/io/Reader.cpp \
	src/base/file.cpp \
	src/base/random.cpp \
	src/base/string.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

fart:
	@make fclean
	@make -j4

debug: DEBUG = -g
debug:
	@echo "Compiling in Debug mode"
	@$(MAKE) fast

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

.PHONY: all clean fclean re fast debug
