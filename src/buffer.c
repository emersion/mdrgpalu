// A buffer is a set of lines.
struct buffer {
	struct line* first;
	struct line* last;

	struct selection* sel;
};

// buffer_reset discards the buffer data and fills it with a new empty line.
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

// buffer_new allocates and initializes a new buffer.
struct buffer* buffer_new() {
	struct buffer* b = malloc(sizeof(struct buffer));
	buffer_reset(b);
	return b;
}

// buffer_free frees a buffer.
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

// buffer_insert_line inserts a new line at the cursor's position.
struct line* buffer_insert_line(struct buffer* b) {
	struct line* l = line_split(b->sel->line, b->sel->ch);

	if (b->last == b->sel->line) {
		b->last = l;
	}
	b->sel->line = l;
	b->sel->ch = 0;

	return l;
}

// buffer_remove_line removes a line at the current cursor's position.
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

// buffer_remove_char removes a character at the current cursor's position.
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

// buffer_insert_char inserts a character at the current cursor's position.
void buffer_insert_char(struct buffer* b, char c) {
	// Handle control chars
	switch (c) {
	case '\r':
		return;
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

// buffer_set_selection sets the current selection. i is the line number, j is
// the column number, and len is the selection's length.
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

// buffer_move_selection moves the current selection. i is the line delta and j
// is the column delta.
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
				at += l->prev->len + 1;
				l = l->prev;
			}
			if (at < 0) {
				at = 0;
			}
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

// buffer_extend_selection extends the current selection of i lines and j
// columns.
void buffer_extend_selection(struct buffer* b, int i, int j) {
	if (i != 0) {
		// TODO: backward
		// TODO
	}

	if (j != 0) {
		int len = b->sel->len;
		if (len == 0) {
			// Extending selection from a cursor, set dir
			if (i < 0 || (i == 0 && j < 0)) {
				b->sel->dir = SELECTION_DIR_LEFT;
			} else {
				b->sel->dir = SELECTION_DIR_RIGHT;
			}
		}

		if (b->sel->dir == SELECTION_DIR_RIGHT) {
			len += j;
		} else {
			// Move the start of the selection
			int at = b->sel->ch + j;
			struct line* l = b->sel->line;
			while (at < 0 && l->prev != NULL) {
				at += l->len;
				l = l->prev;
			}
			if (at < 0) {
				at = 0;
			} else {
				len -= j;
			}
			b->sel->line = l;
			b->sel->ch = at;
		}

		// Check that selection is not beyond the end of the buffer
		int end = b->sel->ch + len;
		if (end > b->sel->line->len) {
			struct line* l = b->sel->line;
			while (l != NULL && end >= 0) {
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
