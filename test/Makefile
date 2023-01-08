# Copyright 2021 Rodrigo Dias Correa. See LICENSE.

CC       ?= $(CROSS_COMPILE)gcc
CPPFLAGS += -I..
CFLAGS   += -std=gnu99 -Wall -Wextra -W -Werror -pedantic -O1 -g -D_FORTIFY_SOURCE=2

BIN_NAME ?= ./test_app

SRC  = ../testprefix.c $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))

all: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(BIN_NAME)

clean:
	-rm $(OBJS) $(BIN_NAME)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
