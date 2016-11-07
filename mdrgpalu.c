#include <stdio.h>

#ifdef __linux__
	#include <termios.h>
#endif

#include "src/editor.c"

void print_escape(char* seq) {
	printf("\033[%s", seq);
}

void print_format(char* seq, char* text) {
	print_escape(seq);
	printf(text);
	print_escape("0m");
}

void editor_print(struct editor* e) {
	printf("\n");
	print_escape("2J"); // clear

	int i = 0;
	int curline = 0, curchar = 0;
	for (struct line* l = e->first; l != NULL; l = l->next) {
		int needsCursor = (e->curline == l);
		if (needsCursor) {
			curline = i;
		}

		for (int i = 0; i < l->len; i++) {
			char c = l->chars[i];
			if (needsCursor && e->curchar == i) {
				char s[2] = {c, '\0'};
				print_format("7m", (char*) &s);
				needsCursor = 0;
				curchar = i;
			} else {
				printf("%c", c);
			}
		}

		if (needsCursor) {
			print_format("7m", " ");
			curchar = l->len;
		}
		printf("\n");

		i++;
	}

	char s[128];
	sprintf((char*) &s, "%d:%d", curline+1, curchar+1);
	print_format("2m", (char*) &s);
}

int main() {
	struct editor* e = editor_new();
	editor_append_curline(e);
	editor_append_curchar(e, 'c');
	editor_append_curchar(e, 'c');
	editor_append_curline(e);
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
