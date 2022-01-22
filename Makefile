CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic -pedantic-errors -std=c11 -ggdb3
# CFLAGS+=-fsanitize-address-use-after-scope -fsanitize=address -fstack-protector-strong
# ASAN_OPTIONS=strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1
SRC= src/main.c src/recdir.c src/util.c src/list.c src/todo.c
# LDFLAGS=-fsanitize-address-use-after-scope
# LIBS=-lasan

build/cnitch: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) $(LIBS) -o $@

.PHONY: clean
clean:
	rm -f build/cnitch
