// buffer_read_from reads f until EOF and appends data in the buffer.
// Returns EOF on error or the number of bytes appended to b on success.
int buffer_read_from(struct buffer* b, FILE* f) {
	if (b->sel->len > 0) {
		buffer_delete_selection(b);
	}

	struct line* l = b->sel->line;
	int at = b->sel->ch;
	int N = 0; // Number of bytes read from f
	while (1) {
		if (l == NULL) {
			// If the first line has be filled, create a new one
			l = buffer_insert_line(b);
		}

		// Read a single line
		int n = line_read_from(l, at, f);
		if (n == EOF) {
			return EOF;
		}

		b->sel->line = l;
		b->sel->ch = at + n;
		N += n + 1; // line + \n
		l = NULL;
		at = 0;
	}

	return N;
}

// buffer_write_to writes the buffer's data to f. It returns a non-zero value on
// error.
int buffer_write_to(struct buffer* b, FILE* f) {
	for (struct line* l = b->first; l != NULL; l = l->next) {
		int err = line_write_to(l, f);
		if (err) {
			return err;
		}
	}

	return 0;
}

// buffer_write_selection_to writes the buffer's selection to f. It returns a
// non-zero value on error.
int buffer_write_selection_to(struct buffer* b, FILE* f) {
	struct line* l = b->sel->line;
	int from = b->sel->ch;
	int len = b->sel->len;
	while (len > 0 && l != NULL) {
		int n = len;
		if (n > l->len - from) {
			n = l->len + 1 - from; // line + \n
		}

		int err = line_write_range_to(l, from, n, f);
		if (err) {
			return err;
		}

		len -= n;
		l = l->next;
		from = 0;
	}

	return 0;
}
