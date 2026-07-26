CC = clang
CFLAGS = -Wall -Wextra -Wpedantic
NCURSES = $(shell pkg-config --cflags --libs ncursesw)

mandlebrot_set: mandlebrot_set.c
	$(CC) $(CFLAGS) mandlebrot_set.c -o mandlebrot_set $(NCURSES)

run: mandlebrot_set
	./mandlebrot_set

clean:
	rm -f mandlebrot_set

.PHONY: run clean
