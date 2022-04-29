.PHONY: clean

# Declaring the compiler
CC=clang++

# Declaring the compiler flags for debug and errors
CFLAGS=-g -std=c++14 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code

# Declering the directories
SRC=src
OBJ=obj
BINDIR=bin
BIN=$(BINDIR)/xDama

SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

all: clean $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/*

fly:
	./$(BIN) 1
