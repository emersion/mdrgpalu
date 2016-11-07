#include <stdio.h>

#ifdef __linux__
	#include <termios.h>
#endif

#include "src/editor.c"

void editor_print(struct editor* e) {
	printf("\n\e[2J"); // clear

	int i = 0;
	int linenum = 0;
	for (struct line* l = e->first; l != NULL; l = l->next) {
		int needsCursor = (e->curline == l);
		if (needsCursor) {
			linenum = i;
		}

		for (int i = 0; i < l->len; i++) {
			char c = l->chars[i];
			if (needsCursor && e->curchar == i) {
				printf("\e[7m%c\e[0m", c); // highlight cursor pos
				needsCursor = 0;
			} else {
				printf("%c", c);
			}
		}

		if (needsCursor) {
			printf("\e[7m \e[0m");
		}
		printf("\n");

		i++;
	}

	printf("\e[2m%d:%d\e[0m", linenum+1, e->curchar+1);
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
				editor_move_line(e, -1);
				break;
			case 66: // down
				editor_move_line(e, 1);
				break;
			case 67: // right
				editor_move_index(e, 1);
				break;
			case 68: // left
				editor_move_index(e, -1);
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
