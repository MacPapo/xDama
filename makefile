.PHONY: clean

UNAME:=$(shell uname)

# Declaring the compiler flags for debug and errors
CFLAGS=-std=c++14 -O3 -g -pedantic -Wall -Wextra -Werror -Wshadow -Wunreachable-code -fstack-protector-all
# -mcpu=apple-m1
# MacOS flags
ifeq ($(UNAME), Darwin)
	CC=c++
endif

# Linux flags
ifeq ($(UNAME), Linux)
	CC=g++
endif

# Declering the directories
SRC=src
OBJ=obj
BOARDS = boards
BINDIR=bin
BIN=$(BINDIR)/xDama

SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

all:  $(BIN) valgrind

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/* $(BOARDS)/*

 valgrind:
	 	valgrind --leak-check=full --show-leak-kinds=all -s  --track-origins=yes --log-file=valgrind-out.txt ./bin/xDama 1

fly:
	./$(BIN) 1
