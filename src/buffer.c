struct buffer {
	struct line* first;
	struct line* last;

	struct selection* sel;
};

void buffer_reset(struct buffer* b) {
	// Delete all lines
	struct line* l = b->first;
	while (l != NULL) {
		struct line* next = l->next;
		line_free(l);
		l = next;
	}
	selection_free(b->sel);

	// Create a new empty line
	l = line_new();
	b->first = l;
	b->last = l;
	b->sel = selection_new();
	b->sel->line = l;
}

struct buffer* buffer_new() {
	struct buffer* b = malloc(sizeof(struct buffer));
	buffer_reset(b);
	return b;
}

void buffer_free(struct buffer* b) {
	struct line* l = b->first;
	while (l != NULL) {
		struct line* next = l->next;
		line_free(l);
		l = next;
	}
	selection_free(b->sel);
	free(b);
}

void buffer_insert_line(struct buffer* b) {
	struct line* l = line_new();
	struct line* curline = b->sel->line;

	l->prev = curline;
	if (curline != NULL) {
		if (curline->next != NULL) {
			curline->next->prev = l;
		}
		l->next = curline->next;
		curline->next = l;

		// Split text between two lines
		if (b->sel->ch == 0) {
			l->len = curline->len;
			l->cap = curline->cap;
			l->chars = curline->chars;
			curline->len = 0;
			curline->cap = 0;
			curline->chars = NULL;
		} else {
			l->len = curline->len - b->sel->ch;
			l->chars = (char*) malloc(l->len);
			memcpy(l->chars, &curline->chars[b->sel->ch], l->len);
			curline->len = b->sel->ch;
		}
	}

	if (b->first == NULL) {
		b->first = l;
	}
	if (b->last == NULL || b->last == curline) {
		b->last = l;
	}
	b->sel->line = l;
	b->sel->ch = 0;
}

void buffer_remove_line(struct buffer* b) {
	if (b->sel->line == NULL || b->sel->line->prev == NULL) {
		return;
	}

	struct line* l = b->sel->line;

	// Copy the end of the current line to the end of the previous one
	int len = l->prev->len + l->len;
	if (l->len > 0) {
		if (l->prev->cap < len) {
			l->prev->cap = len;
			l->prev->chars = (char*) realloc(l->prev->chars, l->prev->cap);
		}
		memcpy(&l->prev->chars[l->prev->len], l->chars, l->len);
	}
	b->sel->ch = l->prev->len;
	l->prev->len = len;

	l->prev->next = l->next;
	if (l->next != NULL) {
		l->next->prev = l->prev;
	}
	if (b->first == l) {
		b->first = l->next;
	}
	if (b->last == l) {
		b->last = l->prev;
	}
	b->sel->line = l->prev;

	line_free(l);
}

int buffer_remove_char(struct buffer* b) {
	if (b->sel->ch == 0) {
		buffer_remove_line(b);
		return '\n';
	}

	int c = line_remove_at(b->sel->line, b->sel->ch-1);

	b->sel->ch--;
	if (b->sel->ch > b->sel->line->len) {
		b->sel->ch = b->sel->line->len;
	}

	return c;
}

void buffer_insert_char(struct buffer* b, char c) {
	// Handle control chars
	switch (c) {
	case '\n':
		buffer_insert_line(b);
		return;
	case 127: // backspace
		buffer_remove_char(b);
		return;
	}

	line_insert_at(b->sel->line, b->sel->ch, c);

	b->sel->ch++;
	if (b->sel->ch > b->sel->line->len) {
		b->sel->ch = b->sel->line->len;
	}
}

void buffer_set_selection(struct buffer* b, int i, int j, int len) {
	if (i >= 0) {
		struct line* l = b->first;
		while (i > 0) {
			if (l->next == NULL) {
				break;
			}
			l = l->next;
			i--;
		}
		b->sel->line = l;
	}

	if (j >= 0) {
		if (j > b->sel->line->len) {
			j = b->sel->line->len;
		}
		b->sel->ch = j;
	}

	if (len >= 0) {
		// TODO: bounds check
		b->sel->len = len;
	}
}

void buffer_move_selection(struct buffer* b, int i, int j) {
	if (i != 0) {
		struct line* l = b->sel->line;
		while (i != 0) {
			if (i > 0) {
				if (l->next == NULL) {
					break;
				}
				i--;
				l = l->next;
			} else {
				if (l->prev == NULL) {
					break;
				}
				i++;
				l = l->prev;
			}
		}
		b->sel->line = l;
	}

	if (j != 0) {
		int at = b->sel->ch;
		if (at > b->sel->line->len) {
			at = b->sel->line->len;
		}
		at += j;

		struct line* l = b->sel->line;
		if (at < 0) {
			// Want to move to a previous line
			while (l->prev != NULL && at < 0) {
				at += l->len - 1;
				l = l->prev;
			}
			if (at < 0) {
				at = 0;
			}
			printf("%d %d", at, l->len);
		} else if (at > b->sel->line->len) {
			// Want to move to a next line
			while (l->next != NULL && at > l->len) {
				at -= l->len + 1;
				l = l->next;
			}
			if (at > l->len) {
				at = l->len;
			}
		}
		b->sel->line = l;
		b->sel->ch = at;
	}
}

void buffer_extend_selection(struct buffer* b, int i, int j) {
	if (i != 0) {
		// TODO: backward
		// TODO
	}

	if (j != 0) {
		int len = b->sel->len;

		if (j < 0) {
			// Move the start of the selection
			len -= j;

			int at = b->sel->ch + j;
			struct line* l = b->sel->line;
			while (at < 0 && l != NULL) {
				at += l->len;
				l = l->prev;
			}
			if (at < 0) {
				at = 0;
			}
			b->sel->line = l;
			b->sel->ch = at;
		} else {
			len += j;
		}

		// Check that selection is not beyond the end of the buffer
		int end = b->sel->ch + len;
		if (end > b->sel->line->len) {
			struct line* l = b->sel->line;
			while (l != NULL && end - l->len >= 0) {
				end -= l->len;
				l = l->next;
			}
			if (end > 0) {
				len -= end;
			}
		}

		b->sel->len = len;
	}
}
