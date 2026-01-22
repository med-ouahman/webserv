COMPILER := c++
COMPILER_FLAGS := -Wall -Wextra -Werror -std=c++98 -g
LIBS := NOTHING
SRCDIR = srcs
OBJDIR := obj
NAME = webserv

SRCS = $(SRCDIR)/main.cpp

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $(OBJS) -o $(NAME)

clean:
	rm -fr $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
