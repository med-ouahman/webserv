NAME1 = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

FILES = ConfigParser.cpp Lexer.cpp main.cpp

OBJCT = ConfigParser.o Lexer.o main.o

all: $(NAME1)

$(NAME1): $(OBJCT)
	$(CC) $(CFLAGS) $(OBJCT) -o $(NAME1)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJCT)

fclean: clean
	rm -f $(NAME1)

re: fclean all

.PHONY: all clean fclean re