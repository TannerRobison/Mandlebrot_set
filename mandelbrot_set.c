#include <complex.h>
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define INITIAL_WIDTH 3.2L
#define INITIAL_ITERATIONS 100
#define MAX_ITERATIONS 100000
#define FRAME_DELAY 30000 // in microseconds
#define ZOOM_RATE 0.965L

static const char gradient[] = " .,:;irsXA253hMHGS#9B&@";

struct viewer {
	long double center_x;
	long double center_y;
	long double width;
	long double target_x;
	long double target_y;
	long double zoom_rate;
	size_t max_iterations;
	bool paused;
	bool show_status;
};

static int mandelbrot(long double cr, long double ci, size_t max_iterations);

static char iteration_character(size_t iteration, size_t max_iterations);

static void reset_viewer(struct viewer *viewer);

static void initialize_viewer(struct viewer *viewer);

static void update_zoom(struct viewer *viewer);

static void draw_status(const struct viewer *viewer, int rows, int columns);

static void draw_mandelbrot(const struct viewer *viewer);

static void move_view(struct viewer *viewer, long double dx, long double dy);

static bool handle_input(struct viewer *viewer);

int main(void)
{
	struct viewer viewer;
	bool running = true;

	initialize_viewer(&viewer);

	if (initscr() == NULL) {
		fprintf(stderr, "Failed to init ncurses");
		return -1;
	}

	if (has_colors()) {
		start_color();
		use_default_colors(); // keeps terminal background color

		// Galactic Void Theme (Pairs 14, 15, & 16)
		init_pair(1, COLOR_CYAN, COLOR_BLACK);
		init_pair(2, COLOR_BLUE, COLOR_BLACK);
		init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	}

	cbreak();
	noecho();
	keypad(stdscr, true);
	nodelay(stdscr, true);
	curs_set(0);

	while (running) {
		draw_mandelbrot(&viewer);
		running = handle_input(&viewer);
		update_zoom(&viewer);
		usleep(FRAME_DELAY);
	}

	endwin();
	return 0;
}

static int mandelbrot(long double cr, long double ci, size_t max_iterations)
{
	long double zr = 0.0L;
	long double zi = 0.0L;
	size_t iteration;

	for (iteration = 0; iteration < max_iterations; iteration++) {
		long double zr2 = zr * zr;
		long double zi2 = zi * zi;

		if (zr2 + zi2 > 4.0) {
			break;
		}

		zi = 2.0 * zr * zi + ci;
		zr = zr2 - zi2 + cr;
	}

	return iteration;
}

static char iteration_character(size_t iteration, size_t max_iterations)
{
	size_t gradient_length = sizeof(gradient) - 2;
	size_t index;

	if (iteration >= max_iterations) {
		return ' ';
	}

	// selecting the character
	index = iteration * gradient_length / max_iterations;

	if (index > gradient_length) {
		index = gradient_length;
	}

	return gradient[index];
}

static void reset_viewer(struct viewer *viewer)
{
	viewer->center_x = 0.0L;
	viewer->center_y = 0.0L;
	viewer->width = INITIAL_WIDTH;
	viewer->max_iterations = INITIAL_ITERATIONS;
}

static void initialize_viewer(struct viewer *viewer)
{
	reset_viewer(viewer);
	viewer->target_x = -0.743643887037151L;
	viewer->target_y = 0.131825904205330L;
	viewer->zoom_rate = ZOOM_RATE;
	viewer->paused = false;
	viewer->show_status = true;
}

static void update_zoom(struct viewer *viewer)
{
	if (viewer->paused) {
		return;
	}

	// scales movement magnitude based on zoom rate so camera is always in
	// real center,
	// i.e. the more zoomed in the smaller movements become
	long double movement = 1.0L - viewer->zoom_rate;

	viewer->center_x += (viewer->target_x - viewer->center_x) * movement;
	viewer->center_y += (viewer->target_y - viewer->center_y) * movement;
	viewer->width *= viewer->zoom_rate;

	// increases number of iterations as zoom increases to see finer detail
	viewer->max_iterations =
	    INITIAL_ITERATIONS +
	    (int)(40.0L * logl(INITIAL_WIDTH / viewer->width));

	if (viewer->max_iterations > MAX_ITERATIONS)
		viewer->max_iterations = MAX_ITERATIONS;

	if (viewer->width < 1.0e-16L)
		reset_viewer(viewer);
}

static void draw_status(const struct viewer *viewer, int rows, int columns)
{

	if (!viewer->show_status || rows < 2) {
		return;
	}

	char status[256];
	snprintf(status, sizeof(status),
		 "center %.18Lf %+.18Lfi  width %.3Le  iter %zu  %s",
		 viewer->center_x, viewer->center_y, viewer->width,
		 viewer->max_iterations,
		 viewer->paused ? "PAUSED" : "AUTO ZOOM");

	attron(A_REVERSE);
	mvaddnstr(0, 0, status, columns);

	// fill rest of row with spaces
	for (int column = (int)strlen(status); column < columns; column++) {
		mvaddch(0, column, ' ');
	}
	attroff(A_REVERSE);
}

static void draw_mandelbrot(const struct viewer *viewer)
{
	size_t rows;
	size_t columns;

	getmaxyx(stdscr, rows, columns);

	// if status bar is on -> start row is 1 not 0
	int start_row = viewer->show_status ? 1 : 0;

	if (rows - start_row <= 0 || columns <= 0) {
		return;
	}

	long double height = viewer->width * (long double)(rows - start_row) /
			     (long double)columns * 2.0L;

	erase();

	for (int screen_y = start_row; screen_y < rows; screen_y++) {
		long double normalized_y = (long double)(screen_y - start_row) /
					   (long double)(rows - start_row - 1);

		long double ci =
		    viewer->center_y + (normalized_y - 0.5L) * height;

		for (int screen_x = 0; screen_x < columns; screen_x++) {
			long double normalized_x =
			    (long double)screen_x / (long double)(columns - 1);

			long double cr = viewer->center_x +
					 (normalized_x - 0.5L) * viewer->width;

			int iteration =
			    mandelbrot(cr, ci, viewer->max_iterations);

			char pixel = iteration_character(
			    iteration, viewer->max_iterations);

			int color_pair = 0;
			bool is_bold = false;
			if (iteration < viewer->max_iterations) {
				// Group iterations into bands of 8 for a smooth
				// ripple effect
				int band = (iteration / 8) % 3;
				if (band == 0) {
					color_pair = 1; // Deep Blue
					is_bold = true;
				} else if (band == 1) {
					color_pair = 2; // Bright Cyan
				} else if (band == 2) {
					color_pair = 3; // White highlight
				}
			}

			// Apply color, draw pixel, remove color
			if (color_pair > 0) {
				attron(COLOR_PAIR(color_pair) | A_BOLD);
				if (is_bold) {
					attron(A_BOLD);
				}
			}

			mvaddch(screen_y, screen_x, pixel);

			if (color_pair > 0) {
				attroff(COLOR_PAIR(color_pair) | A_BOLD);
				if (is_bold) {
					attroff(A_BOLD);
				}
			}
		}
	}

	draw_status(viewer, rows, columns);
	refresh();
}

static void move_view(struct viewer *viewer, long double dx, long double dy)
{
	long double height = viewer->width * 0.5L;

	viewer->center_x += viewer->width * dx;
	viewer->center_y += height * dy;
	viewer->target_x += viewer->width * dx;
	viewer->target_y += height * dy;
}

static bool handle_input(struct viewer *viewer)
{
	int key = getch();

	switch (key) {
	case 'q':
	case 'Q':
		return false;

	case ' ':
	case 'p':
	case 'P':
		viewer->paused = !viewer->paused;
		break;

	case 'r':
	case 'R':
		reset_viewer(viewer);
		break;

	case 's':
	case 'S':
		viewer->show_status = !viewer->show_status;
		break;

	case '+':
	case '=':
		viewer->width *= 0.8L;
		break;

	case '-':
	case '_':
		viewer->width *= 1.25L;
		break;

	case KEY_LEFT:
	case 'h':
		move_view(viewer, -0.1L, 0.0L);
		break;

	case KEY_RIGHT:
	case 'l':
		move_view(viewer, 0.1L, 0.0L);
		break;

	case KEY_UP:
	case 'k':
		move_view(viewer, 0.0L, -0.1L);
		break;

	case KEY_DOWN:
	case 'j':
		move_view(viewer, 0.0L, 0.1L);
		break;

	case '[':
		viewer->zoom_rate -= 0.005L;
		if (viewer->zoom_rate < 0.8L)
			viewer->zoom_rate = 0.8L;
		break;

	case ']':
		viewer->zoom_rate += 0.005L;
		if (viewer->zoom_rate > 0.999L)
			viewer->zoom_rate = 0.999L;
		break;

	default:
		break;
	}

	return true;
}
