#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "line.c"
#include "selection.c"
#include "buffer.c"
#include "io.c"

#ifdef __unix__
	#include <sys/termios.h>
	#include "unix/term.c"
#elif _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include "windows/term.c"
#endif

// TODO: create windows counterparts for this
#include "unix/format.c"
#include "unix/sequence.c"
#include "unix/clipboard.c"

#define ESC '\033'

struct status {
	int curline;
	int curcol;
	int sellen;
};

void status_print(struct status* s) {
	char t[128];
	int n = snprintf((char*) &t, sizeof(t), "%d:%d", s->curline+1, s->curcol+1);
	if (s->sellen > 0 && (uint) n < sizeof(t)) {
		n += snprintf((char*) &t[n], sizeof(t)-n, " (%d)", s->sellen);
	}
	print_format(FORMAT_DIM, (char*) &t);
}

void buffer_print(struct buffer* b, struct status* s) {
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

	if (s != NULL) {
		s->curline = curline;
		s->curcol = curchar;
		s->sellen = b->sel->len;
	}
}

int main(int argc, char** argv) {
	struct buffer* b = buffer_new();

	char* filename = NULL;
	if (argc == 2) {
		filename = argv[1];
		FILE* f = fopen(filename, "r");
		if (f != NULL) {
			buffer_read_from(b, f);
			int err = ferror(f);
			fclose(f);
			if (err) {
				return err;
			}
			buffer_set_selection(b, 0, 0, 0);
		} else if (errno != ENOENT) {
			return 1;
		}
	}

	setvbuf(stdin, NULL, _IONBF, 0); // Turn off buffering
	setup_term();

	struct status* s = (struct status*) malloc(sizeof(struct status));
	buffer_print(b, s);
	status_print(s);

	int c;
	int prev = -1;
	char* statustext = NULL;
	while (1) {
		c = fgetc(stdin);
		if (feof(stdin)) {
			break;
		} else if (ferror(stdin)) {
			return 1;
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
		} else if (c == '\t' || c == '\n' || c >= ' ') {
			buffer_insert_char(b, (char) c);
		} else {
			switch (c) {
				case 3: { // ctrl+C
					FILE* f = clipboard_open("w");
					if (f == NULL) {
						return 1;
					}
					int err;
					if (b->sel->len == 0) {
						err = line_write_to(b->sel->line, f);
					} else {
						err = buffer_write_selection_to(b, f);
					}
					clipboard_close(f);
					if (err) {
						return err;
					}
					break;
				}
				case 17: // ctrl+Q
				case 23: // ctrl+W
					return 0;
				case 19: // ctrl+S
					if (filename != NULL) {
						FILE* f = fopen(filename, "w+");
						if (f == NULL) {
							return 1;
						}
						int err = buffer_write_to(b, f);
						fclose(f);
						if (err) {
							return err;
						}
						statustext = strdup("File saved.");
					}
					break;
				case 22: { // ctrl+V
					FILE* f = clipboard_open("r");
					if (f == NULL) {
						return 1;
					}
					buffer_read_from(b, f);
					int err = ferror(f);
					clipboard_close(f);
					if (err) {
						return err;
					}
					break;
				}
				case 24: { // ctrl+X
					// TODO
				}
			}
		}

		buffer_print(b, s);
		if (statustext != NULL) {
			print_format(FORMAT_DIM, statustext);
			free(statustext);
			statustext = NULL;
		} else {
			status_print(s);
		}

		prev = c;
	}

	return 0;
}
