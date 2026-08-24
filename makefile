CC = clang
CFLAGS = -Wall -Wextra -Wpedantic
NCURSES = $(shell pkg-config --cflags --libs ncursesw)

mandelbrot_set: mandelbrot_set.c
	$(CC) $(CFLAGS) mandelbrot_set.c -o mandelbrot_set $(NCURSES) -lm

run: mandelbrot_set
	./mandelbrot_set

clean:
	rm -f mandelbrot_set

.PHONY: run clean
