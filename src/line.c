struct line {
	struct line* prev;
	struct line* next;

	char* chars;
	int len;
	int cap;
};

struct line* line_new() {
	struct line* l = (struct line*) malloc(sizeof(struct line));
	l->prev = NULL;
	l->next = NULL;
	l->chars = NULL;
	l->len = 0;
	l->cap = 0;
	return l;
}

void line_free(struct line* l) {
	free(l->chars);
	free(l);
}

void line_realloc(struct line* l) {
	if (l->len + 1 > l->cap) {
		int more = 1;
		if (l->cap > 0) {
			more = l->cap;
		}
		if (l->cap > 1024) {
			more = 1024;
		}
		l->cap += more;

		l->chars = (char*) realloc(l->chars, l->cap);
	}
}

// line_read_from reads a single line from f and adds it to the line, at the
// specified position. Returns the number of bytes appended to the line.
int line_read_from(struct line* l, int at, FILE* f) {
	int n = 0;
	while (1) {
		int c = fgetc(f);
		if (c == EOF) {
			break;
		}
		if (c == '\r') {
			continue;
		}
		if (c == '\n') {
			break;
		}

		if (l->len + 1 >= l->cap) {
			line_realloc(l);
		}
		if (at < l->len) {
			// TODO: optimize this
			memmove(&l->chars[at+1], &l->chars[at], l->len-at);
		}

		l->chars[at] = (char) c;
		at++;
		n++;
		l->len++;
	}

	return n;
}

int line_write_range_to(struct line* l, int at, int len, FILE* f) {
	if (len < 0) {
		len = l->len+1;
	}

	if (at < l->len) {
		int n = len;
		if (n > l->len) {
			n = l->len;
		}
		fwrite(&l->chars[at], sizeof(char), n, f);
		int err = ferror(f);
		if (err) {
			return err;
		}
	}

	if (len > l->len) {
		fputc('\n', f);
		return ferror(f);
	}
	return 0;
}

int line_write_to(struct line* l, FILE* f) {
	return line_write_range_to(l, 0, -1, f);
}

void line_insert_char(struct line* l, int at, char c) {
	if (at > l->len) {
		at = l->len;
	}

	line_realloc(l);

	if (at < l->len) {
		memmove(&l->chars[at+1], &l->chars[at], l->len-at);
	}

	l->chars[at] = c;
	l->len++;
}

void line_delete_range(struct line* l, int at, int len) {
	memmove(&l->chars[at], &l->chars[at+len], l->len-at-len);
	l->len -= len;
	// TODO: realloc
}

int line_delete_char(struct line* l, int at) {
	if (l->len == 0) {
		return -1;
	}
	if (at < 0) {
		at = 0;
	}
	if (at >= l->len) {
		at = l->len-1;
	}

	char c = l->chars[at];
	line_delete_range(l, at, 1);
	return c;
}

// line_delete deletes l without deallocating it. It's useful for instance when
// deleting a line from a buffer and inserting it somewhere else.
void line_delete(struct line* l) {
	l->prev->next = l->next;
	if (l->next != NULL) {
		l->next->prev = l->prev;
	}
}

// line_split splits l in two lines, l will contain the part before at and the
// returned line will contain the remainder.
struct line* line_split(struct line* l, int at) {
	struct line* next = line_new();
	next->prev = l;
	next->next = l->next;
	if (l->next != NULL) {
		l->next->prev = next;
	}
	l->next = next;

	if (at < l->len) {
		// Split chars between two lines if necessary
		if (at == 0) {
			// next contains all chars, l is empty
			next->len = l->len;
			next->cap = l->cap;
			next->chars = l->chars;
			l->len = 0;
			l->cap = 0;
			l->chars = NULL;
		} else {
			next->len = l->len - at;
			next->chars = (char*) malloc(next->len);
			memcpy(next->chars, &l->chars[at], next->len);
			l->len = at;
		}
	}

	return next;
}
