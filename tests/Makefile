CXX := c++
SOCKET := Socket.cpp

INC  = Socket.hpp

TEST_DIR = load
TEST_SRC := $(shell find $(TEST_DIR)/*.cpp)

SRC = $(TEST_SRC)

OBJ = $(SRC:.cpp=.o)

NAME = test

all: $(NAME)

%.o: %.cpp
	$(CXX) -c $< -o $@

$(NAME): $(OBJ)
	$(cxx) $(OBJ) -o $(NAME)
