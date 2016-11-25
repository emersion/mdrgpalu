// A buffer is a set of lines.
struct buffer {
	struct line* first; // The first line in the buffer
	struct line* last; // The last line in the buffer

	struct selection* sel; // The buffer's current selection
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

// buffer_new allocates a new empty buffer. The buffer initially contains an
// empty line and the selection is set to the first character.
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

// buffer_len returns the length of the buffer's text.
int buffer_len(struct buffer* b) {
	int len = 0;
	for (struct line* l = b->first; l != NULL; l = l->next) {
		len += l->len;
		if (l->next != NULL) {
			len += 1;
		}
	}
	return len;
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

// buffer_delete_line removes a line from the buffer.
void buffer_delete_line(struct buffer* b, struct line* l) {
	if (b->first == l && b->last == l) {
		return;
	}

	if (b->sel->line == l) {
		if (b->first == l) {
			b->sel->line = l->next;
		} else {
			b->sel->line = l->prev;
		}
	}
	if (b->first == l) {
		b->first = l->next;
	}
	if (b->last == l) {
		b->last = l->prev;
	}

	line_delete(l);
	line_free(l);
}

// buffer_delete_char removes a character at the current cursor's position if
// dir == 0 or the previous if dir = -1. It returns the deleted char.
char32_t buffer_delete_char(struct buffer* b, int dir) {
	int at = b->sel->ch + dir;

	if (at >= 0 && at < b->sel->line->len) {
		// Delete a character in this line
		char32_t c = line_delete_char(b->sel->line, at);

		if (at > b->sel->line->len) {
			at = b->sel->line->len;
		}
		if (at < 0) {
			at = 0;
		}
		b->sel->ch = at;

		return c;
	}

	// Delete a line
	struct line* l = NULL;
	struct line* other = NULL;
	if (at < 0) {
		if (b->sel->line->prev == NULL) {
			return EOF;
		}

		l = b->sel->line->prev;
		other = b->sel->line;
	} else { // at >= b->sel->line->len
		if (b->sel->line->next == NULL) {
			return EOF;
		}

		l = b->sel->line;
		other = b->sel->line->next;
	}

	b->sel->line = l;
	b->sel->ch = l->len;

	line_join(l, other);
	buffer_delete_line(b, other);

	return '\n';
}

// buffer_delete_selection deletes the current selection.
void buffer_delete_selection(struct buffer* b) {
	struct line* l = b->sel->line;
	int from = b->sel->ch;
	int len = b->sel->len;
	while (len > 0 && l != NULL) {
		int n = len;
		int delete_line = 0;
		if (n > l->len - from) {
			n = l->len - from + 1;
			if (from == 0) {
				delete_line = 1;
			}
		}

		// Store next line here, because we'll maybe delete l and won't be able to
		// access l->next anymore
		struct line* next = l->next;

		if (delete_line) {
			buffer_delete_line(b, l);
		} else {
			line_delete_range(l, from, n);
			if (l != b->sel->line) {
				// Merge last line with first one if they're different
				line_join(b->sel->line, l);
				buffer_delete_line(b, l);
			}
		}

		len -= n;
		l = next;
		from = 0;
	}

	b->sel->len = 0;
}

// buffer_insert_char inserts a character at the current cursor's position.
void buffer_insert_char(struct buffer* b, char32_t c) {
	if (b->sel->len > 0) {
		buffer_delete_selection(b);
	}

	// Handle control chars
	switch (c) {
	case '\r':
		return;
	case '\n':
		buffer_insert_line(b);
		return;
	}

	line_insert_char(b->sel->line, b->sel->ch, c);

	b->sel->ch++;
	if (b->sel->ch > b->sel->line->len) {
		b->sel->ch = b->sel->line->len;
	}
}

// buffer_set_selection_line sets the current line index.
void buffer_set_selection_line(struct buffer* b, int i) {
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

// buffer_move_selection_line moves the current line index. Negative values move
// to previous lines, positive values move to next lines.
void buffer_move_selection_line(struct buffer* b, int i) {
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

// buffer_set_selection sets the current selection. i is the line number, j is
// the column number, and len is the selection's length. If i < 0, the current
// line number is not changed. Same for j and len.
void buffer_set_selection(struct buffer* b, int i, int j, int len) {
	if (i >= 0) {
		buffer_set_selection_line(b, i);
	}
	if (j >= 0) {
		selection_set_ch(b->sel, j);
	}
	if (len >= 0) {
		selection_set_len(b->sel, len);
	}
}

// buffer_set_position_string sets the selection by parsing a string in the
// "line:column" format.
void buffer_set_position_string(struct buffer* b, char* s) {
	char* end;
	int line = (int) strtoul(s, &end, 10) - 1;
	if (line < 0) {
		line = 0;
	}
	buffer_set_selection_line(b, line);

	int col = 0;
	if (end[0] != 0) {
		col = (int) strtoul(&end[1], &end, 10) - 1;
	}
	if (col > b->sel->line->len) {
		col = b->sel->line->len;
	} else if (col < 0) {
		col = 0;
	}
	b->sel->ch = col;
	b->sel->len = 0;
}

// buffer_move_selection moves the current selection. i is the line delta and j
// is the column delta.
void buffer_move_selection(struct buffer* b, int i, int j) {
	if (i != 0) {
		buffer_move_selection_line(b, i);
	}
	if (j != 0) {
		selection_move_ch(b->sel, j);
	}
}

// buffer_extend_selection extends the current selection of i lines and j
// columns.
void buffer_extend_selection(struct buffer* b, int i, int j) {
	if (b->sel->len == 0) {
		// Extending selection from a cursor, set dir
		if (i < 0 || (i == 0 && j < 0)) {
			b->sel->dir = SELECTION_DIR_LEFT;
		} else {
			b->sel->dir = SELECTION_DIR_RIGHT;
		}
	}

	if (i != 0) {
		// TODO
	}

	if (j != 0) {
		selection_extend_ch(b->sel, j);
	}
}

void buffer_shrink_selection(struct buffer* b, int dir) {
	selection_shrink(b->sel, dir);
}

void buffer_jump_selection(struct buffer* b, int dir) {
	selection_jump(b->sel, dir);
}

void buffer_extend_jump_selection(struct buffer* b, int dir) {
	selection_extend_jump(b->sel, dir);
}

void buffer_swap_lines(struct buffer* b, struct line* l1, struct line* l2) {
	if (l1 == l2) {
		return;
	}

	if (b->first == l1) {
		b->first = l2;
	} else if (b->first == l2) {
		b->first = l1;
	}
	if (b->last == l1) {
		b->last = l2;
	} else if (b->last == l2) {
		b->last = l1;
	}

	line_swap(l1, l2);
}

struct status {
	int curline;
	int curcol;
	int sellen;
};

void status_print(struct status* s);

void buffer_print(struct buffer* b, struct status* s);
