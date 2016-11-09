#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "line.c"
#include "selection.c"
#include "buffer.c"

#ifdef __unix__
	#include <sys/termios.h>
	#include "unix/term.c"
#elif _WIN32
	#include <windows.h>
	#include "windows/term.c"
#endif

#include "unix/format.c"
#include "unix/sequence.c"

#define ESC '\033'

void buffer_print(struct buffer* b) {
	printf("\n");
	print_escape(FORMAT_CLEAR); // clear

	int i = 0;
	int curline = 0, curchar = 0;
	int selch = -1, sellen = -1;
	for (struct line* l = b->first; l != NULL; l = l->next) {
		if (b->sel->line == l) {
			curline = i;
			selch = b->sel->ch;
			if (selch > l->len) {
				selch = l->len;
			}
			sellen = b->sel->len;
			if (sellen == 0) {
				sellen = 1;
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
			if (selch == -1 && sellen > 0) {
				sellen--;
				if (sellen <= 0) {
					print_escape(FORMAT_RESET);
				}
			}
		}

		printf("\n");
		i++;
	}

	char s[128];
	int n = snprintf((char*) &s, sizeof(s), "%d:%d", curline+1, curchar+1);
	if (b->sel->len > 0 && (uint) n < sizeof(s)) {
		n += snprintf((char*) &s[n], sizeof(s)-n, " (%d)", b->sel->len);
	}
	print_format(FORMAT_DIM, (char*) &s);
}

int main() {
	struct buffer* b = buffer_new();
	buffer_insert_char(b, 'c');
	buffer_insert_char(b, 'c');
	buffer_insert_line(b);
	buffer_insert_char(b, 's');
	buffer_insert_char(b, 'a');
	buffer_insert_char(b, 'v');
	buffer_insert_char(b, 'a');
	buffer_set_selection(b, 0, 0, 0);

	setup_term();
	buffer_print(b);

	int c;
	int prev = -1;
	while (1) {
		c = getchar();
		if (c < 0) {
			break;
		}

		if (prev == ESC && c == '[') {
			struct sequence* s = sequence_parse();
			if (s == NULL) {
				printf("Cannot parse escape sequence\n");
				continue;
			}

			switch (s->code) {
			case CODE_CUU:
			case CODE_CUD:
			case CODE_CUF:
			case CODE_CUB:;
				int delta = s->params[0];
				if (delta == 0) {
					delta = 1;
				}
				int modifiers = s->params[1];

				int i = 0, j = 0;
				switch (s->code) {
				case CODE_CUU:
					i = -delta;
					break;
				case CODE_CUD:
					i = delta;
					break;
				case CODE_CUF:
					j = delta;
					break;
				case CODE_CUB:
					j = -delta;
					break;
				}
				if (modifiers & MODIFIER_SHIFT) {
					buffer_extend_selection(b, i, j);
				} else {
					buffer_move_selection(b, i, j);
				}
				break;
			}

			sequence_free(s);
		} else if (c == ESC) {
			prev = c;
			continue;
		} else {
			buffer_insert_char(b, (char) c);
		}

		buffer_print(b);

		prev = c;
	}
}
