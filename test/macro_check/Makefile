# Copyright 2021-2024 Rodrigo Dias Correa. See LICENSE.

CC       ?= $(CROSS_COMPILE)gcc
CPPFLAGS += -I../../
CFLAGS   += -std=gnu99 -Wall -Wextra -W -Werror -O1 -g -D_FORTIFY_SOURCE=2

BIN_NAME ?= ./macro_check

SRC  = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, out/%.o, $(SRC))

all: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(BIN_NAME)

clean:
	-rm $(OBJS) $(BIN_NAME)

out/%.o: src/%.c | out
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

out:
	mkdir out

