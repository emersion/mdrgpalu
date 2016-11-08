#include <stdlib.h>
#include <string.h>

//#include "line.c"
#include "selection.c"

struct buffer {
	struct line* first;
	struct line* last;

	struct selection* sel;
};

struct buffer* buffer_new() {
	struct buffer* e = malloc(sizeof(struct buffer));
	struct line* l = line_new();
	e->first = l;
	e->last = l;
	e->sel = selection_new();
	e->sel->line = l;
	return e;
}

void buffer_free(struct buffer* e) {
	struct line* l = e->first;
	while (l != NULL) {
		struct line* next = l->next;
		line_free(l);
		l = next;
	}
	selection_free(e->sel);
	free(e);
}

void buffer_insert_line(struct buffer* e) {
	struct line* l = line_new();
	struct line* curline = e->sel->line;

	l->prev = curline;
	if (curline != NULL) {
		if (curline->next != NULL) {
			curline->next->prev = l;
		}
		l->next = curline->next;
		curline->next = l;

		// Split text between two lines
		if (e->sel->ch == 0) {
			l->len = curline->len;
			l->cap = curline->cap;
			l->chars = curline->chars;
			curline->len = 0;
			curline->cap = 0;
			curline->chars = NULL;
		} else {
			l->len = curline->len - e->sel->ch;
			l->chars = (char*) malloc(l->len);
			memcpy(l->chars, &curline->chars[e->sel->ch], l->len);
			curline->len = e->sel->ch;
		}
	}

	if (e->first == NULL) {
		e->first = l;
	}
	if (e->last == NULL || e->last == curline) {
		e->last = l;
	}
	e->sel->line = l;
	e->sel->ch = 0;
}

void buffer_remove_line(struct buffer* e) {
	if (e->sel->line == NULL || e->sel->line->prev == NULL) {
		return;
	}

	struct line* l = e->sel->line;

	// Copy the end of the current line to the end of the previous one
	int len = l->prev->len + l->len;
	if (l->len > 0) {
		if (l->prev->cap < len) {
			l->prev->cap = len;
			l->prev->chars = (char*) realloc(l->prev->chars, l->prev->cap);
		}
		memcpy(&l->prev->chars[l->prev->len], l->chars, l->len);
	}
	e->sel->ch = l->prev->len;
	l->prev->len = len;

	l->prev->next = l->next;
	if (l->next != NULL) {
		l->next->prev = l->prev;
	}
	if (e->first == l) {
		e->first = l->next;
	}
	if (e->last == l) {
		e->last = l->prev;
	}
	e->sel->line = l->prev;

	line_free(l);
}

int buffer_remove_char(struct buffer* e) {
	if (e->sel->ch == 0) {
		buffer_remove_line(e);
		return '\n';
	}

	int c = line_remove_at(e->sel->line, e->sel->ch-1);

	e->sel->ch--;
	if (e->sel->ch > e->sel->line->len) {
		e->sel->ch = e->sel->line->len;
	}

	return c;
}

void buffer_insert_char(struct buffer* e, char c) {
	// Handle control chars
	switch (c) {
	case '\n':
		buffer_insert_line(e);
		return;
	case 127: // backspace
		buffer_remove_char(e);
		return;
	}
	if (c < 32) {
		return;
	}

	line_insert_at(e->sel->line, e->sel->ch, c);

	e->sel->ch++;
	if (e->sel->ch > e->sel->line->len) {
		e->sel->ch = e->sel->line->len;
	}
}

void buffer_set_selection_line(struct buffer* e, int i) {
	if (i < 0) {
		return;
	}

	struct line* l = e->first;
	while (i > 0) {
		if (l->next == NULL) {
			break;
		}
		l = l->next;
		i--;
	}
	e->sel->line = l;
}

void buffer_move_selection_line(struct buffer* e, int delta) {
	if (delta == 0) {
		return;
	}

	struct line* l = e->sel->line;
	while (delta != 0) {
		if (delta > 0) {
			if (l->next == NULL) {
				break;
			}
			delta--;
			l = l->next;
		} else {
			if (l->prev == NULL) {
				break;
			}
			delta++;
			l = l->prev;
		}
	}
	e->sel->line = l;
}

void buffer_set_selection_char(struct buffer* e, int i) {
	if (i < 0) {
		i = 0;
	}
	if (i > e->sel->line->len) {
		i = e->sel->line->len;
	}
	e->sel->ch = i;
}

void buffer_move_selection_char(struct buffer* e, int delta) {
	buffer_set_selection_char(e, e->sel->ch + delta);
}
