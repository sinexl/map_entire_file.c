##
# Map-entire-file.cpp
#
# @file
# @version 0.1
CC = g++
WINCC = x86_64-w64-mingw32-g++
CFLAGS = -g -Wall -Wextra -Wswitch-enum -Wno-unused-function
INCLUDE = -I../ -I./exceptions/
HEADERS := $(shell find . -name "*.hpp")
SRC := $(shell find . -name "*.cpp")
all: example

example: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE) -o example.out $(SRC)

# end
