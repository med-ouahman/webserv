CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -I../..
NAME := http_tests

SOURCES := \
	main.cpp \
	../Context.cpp \
	../response_writer.cpp \
	../Parser/Parser.cpp \
	../Parser/request_line.cpp \
	../Parser/headers/header_rules.cpp \
	../Parser/headers/header_utils.cpp \
	../Parser/headers/headers.cpp \
	../Parser/body/body.cpp \
	../Parser/body/body_store.cpp \
	../Parser/body/body_utils.cpp \
	../Parser/body/chunked.cpp \
	../Parser/body/fixed.cpp \
	../Parser/body/temp_storage.cpp \
	../routing/Routing.cpp \
	../routing/checks.cpp \
	../routing/handlers.cpp \
	../routing/normalization.cpp \
	../routing/path.cpp \
	../routing/server.cpp \
	../pipeline/pipeline.cpp \
	../pipeline/RequestHandler.cpp \
	../pipeline/handlers/DeleteHandler.cpp \
	../pipeline/handlers/DirectoryHandler.cpp \
	../pipeline/handlers/ErrorHandler.cpp \
	../pipeline/handlers/CGI/CGIContext.cpp \
	../pipeline/handlers/RedirectHandler.cpp \
	../pipeline/handlers/StaticFileHandler.cpp \
	../pipeline/handlers/UploadHandler.cpp \
	../../base/io/Reader.cpp \
	../../base/io/Writer.cpp

all: $(NAME)

$(NAME): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(NAME)

clean:
	rm -f $(NAME)

re: clean all

.PHONY: all clean re
