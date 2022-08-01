# Copyright 2021 Rodrigo Dias Correa. See LICENSE.

CC       ?= $(CROSS_COMPILE)gcc
AR       ?= $(CROSS_COMPILE)ar
CPPFLAGS += -I..
CFLAGS   += -std=gnu99 -Wall -Wextra -W -Werror -O1 -g -D_FORTIFY_SOURCE=2
LDFLAGS  +=

VALGRIND ?= valgrind

TEST_APP = ./test_app

SRC  = ../testprefix.c $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))

test: $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(TEST_APP) $(LDFLAGS)
	$(VALGRIND) -s --leak-check=full --track-origins=yes $(TEST_APP)

clean:
	-rm $(OBJS) $(TEST_APP)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
