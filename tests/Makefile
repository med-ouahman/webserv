CXX := c++
SOCKET := Socket.cpp

INC  = Socket.hpp

TEST_DIR = .

TEST_SRC := $(shell find $(TEST_DIR)/*.cpp)

SRC = $(TEST_SRC)

OBJ = $(SRC:.cpp=.o)

NAME = test

all: $(NAME)

%.o: %.cpp
	$(CXX) -c $< -o $@

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)
