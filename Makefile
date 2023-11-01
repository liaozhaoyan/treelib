LUA ?= lua5.1
LUA_PC ?= lua5.1
LUA_CFLAGS = $(shell pkg-config $(LUA_PC) --cflags)

CFLAGS ?= -O3 -g -fPIC

all: treelib.so

%.o: %.c tree.h
	$(CC) -c $(CFLAGS) $(LUA_CFLAGS) -o $@ $<

treelib.so: treelib.o tree.o
	$(CC) -shared treelib.o tree.o -o $@

clean:
	rm -f treelib.so *.o *.rock