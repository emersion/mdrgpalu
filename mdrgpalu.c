#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

struct line {
	struct line* next;
	struct line* prev;

	char* chars;
	int len;
	int cap;
};

struct line* line_new() {
	struct line* l = (struct line*) malloc(sizeof(struct line));
	l->chars = NULL;
	l->len = 0;
	l->cap = 0;
	return l;
}

void line_grow(struct line* l, int more) {
	l->cap += more;
	l->chars = realloc(l->chars, l->cap);
}

void line_append(struct line* l, char c) {
	if (l->len + 1 > l->cap) {
		int more = l->cap;
		if (more == 0) {
			more = 1;
		}
		if (more > 1024) {
			more = 1024;
		}
		line_grow(l, more);
	}

	l->chars[l->len] = c;
	l->len++;
}

void line_insert_at(struct line* l, int i, char c) {
	if (i >= l->len) {
		return line_append(l, c);
	}

	if (l->len + 1 > l->cap) {
		line_grow(l, l->cap);
	}

	memmove(&l->chars[i+1], &l->chars[i], l->len - i);
	l->chars[i] = c;
	l->len++;
}

struct editor {
	struct line* first;
	struct line* last;

	struct line* curline;
	int curindex;
};

struct editor* editor_new() {
	struct editor* e = malloc(sizeof(struct editor));
	e->first = NULL;
	e->last = NULL;
	e->curline = NULL;
	e->curindex = 0;
	return e;
}

void editor_append_line(struct editor* e) {
	struct line* l = line_new();

	l->prev = e->curline;
	if (e->curline != NULL) {
		l->next = e->curline->next;
		e->curline->next = l;
	}

	if (e->first == NULL) {
		e->first = l;
	}
	if (e->last == NULL || e->last == e->curline) {
		e->last = l;
	}
	e->curline = l;
}

void editor_append_char(struct editor* e, char c) {
	line_insert_at(e->curline, e->curindex, c);
	e->curindex++;
}

void editor_move_line(struct editor* e, int delta) {
	if (delta == 0) {
		return;
	}

	int step = 1;
	if (delta < 0) {
		step = -1;
	}

	struct line* l = e->curline;
	while (delta != 0) {
		if (delta > 0) {
			if (l->next != NULL) {
				delta--;
				l = l->next;
			} else {
				break;
			}
		} else {
			if (l->prev != NULL) {
				delta++;
				l = l->prev;
			} else {
				break;
			}
		}
	}
	e->curline = l;
}

void editor_move_index(struct editor* e, int delta) {
	if (delta == 0 || e->curline == NULL) {
		return;
	}

	int target = e->curindex + delta;
	if (target < 0) {
		target = 0;
	}
	if (target >= e->curline->len) {
		target = e->curline->len - 1;
	}
	e->curindex = target;
}

void editor_print(struct editor* e) {
	printf("\e[2J"); // clear

	for (struct line* l = e->first; l != NULL; l = l->next) {
		for (int i = 0; i < l->len; i++) {
			char c = l->chars[i];
			if (e->curline == l && (e->curindex == i || (e->curindex >= l->len && i == l->len-1))) {
				printf("\e[7m%c\e[0m", c); // highlight cursor pos
			} else {
				printf("%c", c);
			}
		}
		printf("\n");
	}
}

int main() {
	struct editor* e = editor_new();
	editor_append_line(e);
	editor_append_char(e, 'c');
	editor_append_char(e, 'c');
	editor_append_line(e);
	editor_append_char(e, 's');
	editor_append_char(e, 'a');
	editor_append_char(e, 'v');
	editor_append_char(e, 'a');

	struct termios t;
	tcgetattr(0, &t);
	t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
	tcsetattr(0, TCSANOW, &t);

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
		} else if (c >= 32) { // printable char
			editor_append_char(e, (char) c);
		} else if (c == 10) { // line feed
			editor_append_line(e);
		}

		editor_print(e);
		//printf("%d", c);

		prev = c;
	}
}
