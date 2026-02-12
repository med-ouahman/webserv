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

SRCS = $(SRCDIR)/main.cpp \
	srcs/core/Connection.cpp \
	srcs/core/ConnectionStateMachine.cpp \
	srcs/io/EventLoop.cpp \
	srcs/io/EventLoopFds.cpp \
	srcs/io/EventLoopRun.cpp \

OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

all: $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $(NAME)

clean:
	rm -fr $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
