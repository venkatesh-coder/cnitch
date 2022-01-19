CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic -pedantic-errors -std=c11 -ggdb
SRC= src/main.c src/recdir.c src/util.c src/list.c src/todo.c

build/cnitch: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@

.PHONY: clean
clean:
	rm -f build/cnitch
