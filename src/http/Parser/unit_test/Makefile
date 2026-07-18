CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -I../../..
NAME := parser_test

SOURCES := \
	main.cpp \
	../Parser.cpp \
	../request_line.cpp \
	../headers/header_rules.cpp \
	../headers/header_utils.cpp \
	../headers/headers.cpp \
	../body/body.cpp \
	../body/body_store.cpp \
	../body/body_utils.cpp \
	../body/chunked.cpp \
	../body/fixed.cpp \
	../../../base/io/Reader.cpp \
	../../../base/io/Writer.cpp

all: $(NAME)

$(NAME): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(NAME)

run: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME)

re: clean all

.PHONY: all run clean re
