#include <stdlib.h>

#include "line.c"

struct editor {
	struct line* first;
	struct line* last;

	struct line* curline;
	int curchar;
};

struct editor* editor_new() {
	struct editor* e = malloc(sizeof(struct editor));
	e->first = NULL;
	e->last = NULL;
	e->curline = NULL;
	e->curchar = 0;
	return e;
}

void editor_append_curline(struct editor* e) {
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

void editor_remove_curline(struct editor* e) {
	if (e->curline == NULL) {
		return;
	}

	struct line* l = e->curline;
	if (l->prev != NULL && l->next != NULL) {
		l->prev->next = l->next->prev;
	} else if (l->prev != NULL) {
		l->prev->next = NULL;
	} else if (l->next != NULL) {
		l->next->prev = NULL;
	}
	if (e->first == l) {
		e->first = l->next;
	}
	if (e->last == l) {
		e->last = l->prev;
	}
	e->curline = l->prev;

	free(l);
}

int editor_remove_curchar(struct editor* e) {
	if (e->curline == NULL) {
		return -1;
	}
	if (e->curline->len == 0 || e->curchar == 0) {
		editor_remove_curline(e);
		return '\n';
	}

	int c = line_remove_at(e->curline, e->curchar-1);

	e->curchar--;
	if (e->curchar > e->curline->len) {
		e->curchar = e->curline->len;
	}

	return c;
}

void editor_append_curchar(struct editor* e, char c) {
	// Handle control chars
	switch (c) {
	case '\n':
		editor_append_curline(e);
		return;
	case 127: // backspace
		editor_remove_curchar(e);
		return;
	}
	if (c < 32) {
		return;
	}

	if (e->curline == NULL) {
		editor_append_curline(e);
	}
	line_insert_at(e->curline, e->curchar, c);

	e->curchar++;
	if (e->curchar > e->curline->len) {
		e->curchar = e->curline->len;
	}
}

void editor_set_curline(struct editor* e, int i) {
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
	e->curline = l;
}

void editor_move_curline(struct editor* e, int delta) {
	if (delta == 0) {
		return;
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

void editor_set_curchar(struct editor* e, int i) {
	if (e->curline == NULL) {
		return;
	}

	if (i < 0) {
		i = 0;
	}
	if (i > e->curline->len) {
		i = e->curline->len;
	}
	e->curchar = i;
}

void editor_move_curchar(struct editor* e, int delta) {
	editor_set_curchar(e, e->curchar + delta);
}
