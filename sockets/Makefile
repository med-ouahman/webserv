
TARGET = internet_socket.c

OB = $(TARGET:.c=.o)

NAME = internet

all: $(NAME)

$(NAME): $(OB)
	gcc $(OB) -o $(NAME)

$(OB): $(TARGET)
	gcc -c $< -o $@
