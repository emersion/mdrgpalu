#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
	#include <sys/termios.h>
#endif
#ifdef _WIN32
	#include <windows.h>
#endif

#include "line.c"
#include "selection.c"
#include "buffer.c"

#define ESC '\033'

#define FORMAT_CLEAR   "2J"
#define FORMAT_RESET   "0m"
#define FORMAT_DIM     "2m"
#define FORMAT_REVERSE "7m"

void print_escape(char* seq) {
	printf("\033[%s", seq);
}

void print_format(char* seq, char* text) {
	print_escape(seq);
	printf(text);
	print_escape(FORMAT_RESET);
}

void buffer_print(struct buffer* e) {
	printf("\n");
	print_escape(FORMAT_CLEAR); // clear

	int i = 0;
	int curline = 0, curchar = 0;
	int selch = -1, sellen = -1;
	for (struct line* l = e->first; l != NULL; l = l->next) {
		if (e->sel->line == l) {
			curline = i;
			selch = e->sel->ch;
			sellen = e->sel->len;
			if (selch > l->len) {
				selch = l->len;
			}
		}

		char c;
		for (int j = 0; j <= l->len; j++) {
			if (j == l->len) {
				c = ' ';
			} else {
				c = l->chars[j];
			}
			if (sellen >= 0 && j == selch) {
				print_escape(FORMAT_REVERSE);
				curchar = j;
				selch = -1;
			} else if (j == l->len) {
				continue;
			}
			printf("%c", c);
			if (selch == -1 && sellen >= 0) {
				sellen--;
				if (sellen < 0) {
					print_escape(FORMAT_RESET);
				}
			}
		}

		printf("\n");

		i++;
	}

	char s[128];
	snprintf((char*) &s, sizeof(s), "%d:%d", curline+1, curchar+1);
	print_format(FORMAT_DIM, (char*) &s);
}

int main() {
	struct buffer* e = buffer_new();
	buffer_insert_char(e, 'c');
	buffer_insert_char(e, 'c');
	buffer_insert_line(e);
	buffer_insert_char(e, 's');
	buffer_insert_char(e, 'a');
	buffer_insert_char(e, 'v');
	buffer_insert_char(e, 'a');
	buffer_set_selection_line(e, 0);
	buffer_set_selection_char(e, 0);

	#ifdef __unix__
		struct termios t;
		tcgetattr(0, &t);
		t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
		tcsetattr(0, TCSANOW, &t);
	#endif
	#ifdef _WIN32
		HANDLE stdinh = GetStdHandle(STD_INPUT_HANDLE);
		SetConsoleMode(stdinh, ENABLE_PROCESSED_INPUT);
	#endif

	buffer_print(e);

	int c;
	int prev = -1;
	int csi = 0;
	while (1) {
		c = getchar();
		if (c < 0) {
			break;
		}

		if (csi) {
			csi = 0;
			int handled = 1;
			switch (c) {
			case 'A': // up
				buffer_move_selection_line(e, -1);
				break;
			case 'B': // down
				buffer_move_selection_line(e, 1);
				break;
			case 'C': // forward
				buffer_move_selection_char(e, 1);
				break;
			case 'D': // back
				buffer_move_selection_char(e, -1);
				break;
			case 'E': // next line
				break;
			case 'F': // previous line
				break;
			case 'G': // column horizontal absolute
				break;
			case 'H': // cursor position
				break;
			default:
				printf("Unhandled escape sequence: %d\n", c);
				handled = 0;
			}
			if (!handled) {
				continue;
			}
		} else if (prev == ESC && c == '[') {
			csi = 1; // Entering escape sequence
			continue;
		} else if (c == ESC) {
			prev = c;
			continue;
		} else {
			buffer_insert_char(e, (char) c);
		}

		buffer_print(e);
		printf(" %d", c);

		prev = c;
	}
}
