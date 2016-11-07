#include <stdio.h>

#ifdef __linux__
	#include <termios.h>
#endif

#include "src/editor.c"

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

void editor_print(struct editor* e) {
	printf("\n");
	print_escape(FORMAT_CLEAR); // clear

	int i = 0;
	int curline = 0, curchar = 0;
	for (struct line* l = e->first; l != NULL; l = l->next) {
		int needsCursor = (e->curline == l);
		if (needsCursor) {
			curline = i;
		}

		for (int j = 0; j < l->len; j++) {
			char c = l->chars[j];
			if (needsCursor && e->curchar == j) {
				char s[2] = {c, '\0'};
				print_format(FORMAT_REVERSE, (char*) &s);
				needsCursor = 0;
				curchar = j;
			} else {
				printf("%c", c);
			}
		}

		if (needsCursor) {
			print_format(FORMAT_REVERSE, " ");
			curchar = l->len;
		}
		printf("\n");

		i++;
	}

	char s[128];
	snprintf((char*) &s, sizeof(s), "%d:%d", curline+1, curchar+1);
	print_format(FORMAT_DIM, (char*) &s);
}

int main() {
	struct editor* e = editor_new();
	editor_insert_line(e);
	editor_append_curchar(e, 'c');
	editor_append_curchar(e, 'c');
	editor_insert_line(e);
	editor_append_curchar(e, 's');
	editor_append_curchar(e, 'a');
	editor_append_curchar(e, 'v');
	editor_append_curchar(e, 'a');
	editor_set_curline(e, 0);
	editor_set_curchar(e, 0);

	#ifdef __linux__
		struct termios t;
		tcgetattr(0, &t);
		t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
		tcsetattr(0, TCSANOW, &t);
	#endif

	editor_print(e);

	int c;
	int prev = -1;
	int escseq = 0;
	while (1) {
		c = getchar();
		if (c < 0) {
			break;
		}

		if (escseq) {
			escseq = 0;

			switch (c) {
			case 65: // up
				editor_move_curline(e, -1);
				break;
			case 66: // down
				editor_move_curline(e, 1);
				break;
			case 67: // right
				editor_move_curchar(e, 1);
				break;
			case 68: // left
				editor_move_curchar(e, -1);
				break;
			}
		} else if (prev == 27 && c == 91) {
			escseq = 1; // Entering escape sequence
		} else {
			editor_append_curchar(e, (char) c);
		}

		editor_print(e);

		prev = c;
	}
}
