// A line is a sequence of characters between two newlines.
struct line {
	struct line* prev; // The previous line
	struct line* next; // The next line

	char* chars; // An array of characters contained in this line, excluding the trailing \n
	int len; // The length of chars
	int cap; // The capacity of chars
};

// line_new allocates a new empty line and returns it.
struct line* line_new() {
	struct line* l = (struct line*) malloc(sizeof(struct line));
	l->prev = NULL;
	l->next = NULL;
	l->chars = NULL;
	l->len = 0;
	l->cap = 0;
	return l;
}

// line_free frees l. The line cannot be used anymore after.
void line_free(struct line* l) {
	free(l->chars);
	free(l);
}

// line_walk gets the i-th line after (or before) l. If no such line exists, it
// returns NULL.
struct line* line_walk(struct line* l, int i) {
	while (i != 0 && l != NULL) {
		if (i > 0) {
			i--;
			l = l->next;
		} else {
			i++;
			l = l->prev;
		}
	}
	return l;
}

// line_insert inserts l between prev and next. l must do not be inserted.
void line_insert(struct line* l, struct line* prev, struct line* next) {
	l->prev = prev;
	l->next = next;

	if (prev != NULL) {
		prev->next = l;
	}
	if (next != NULL) {
		next->prev = l;
	}
}

// line_delete deletes l without deallocating it. It's useful for instance when
// deleting a line from a buffer and inserting it somewhere else.
void line_delete(struct line* l) {
	if (l->prev != NULL) {
		l->prev->next = l->next;
	}
	if (l->next != NULL) {
		l->next->prev = l->prev;
	}

	l->prev = NULL;
	l->next = NULL;
}

// line_swap swaps l1 with l2.
void line_swap(struct line* l1, struct line* l2) {
	// We need a temporary variable as we'll need to swap next and prev
	struct line* tmp;

	// Swap next
	tmp = l1->next;
	l1->next = l2->next;
	l2->next = tmp;

	if (l1->next != NULL) {
		l1->next->prev = l1;
	}
	if (l2->next != NULL) {
		l2->next->prev = l2;
	}

	// Swap prev
	tmp = l1->prev;
	l1->prev = l2->prev;
	l2->prev = tmp;

	if (l1->prev != NULL) {
		l1->prev->next = l1;
	}
	if (l2->prev != NULL) {
		l2->prev->next = l2;
	}
}

// line_realloc is an internal function that grows l's capacity by at least one
// byte.
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

// line_read_from reads a single line from f and adds it to l, at the specified
// position. Returns the number of bytes appended to the line.
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

// line_write_range_to writes a range of characters from l to f. The range
// starts at index at and ends len bytes after. Returns a non-zero value on
// error.
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

	if (at + len > l->len) {
		fputc('\n', f);
		return ferror(f);
	}
	return 0;
}

// line_write_to writes l's content to f. Returns a non-zero value on error.
int line_write_to(struct line* l, FILE* f) {
	return line_write_range_to(l, 0, -1, f);
}

// line_insert_char inserts c at the position at. It moves characters after at
// if necessary.
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

// line_delete_range deletes a range of characters from l. The range starts at
// index at and ends len bytes after.
void line_delete_range(struct line* l, int at, int len) {
	memmove(&l->chars[at], &l->chars[at+len], l->len-at-len);
	l->len -= len;
	// TODO: realloc to free memory
}

// line_delete_char deletes a the character at index at from l and returns its
// value.
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

// line_jump jumps to a neighbor word. It jumps forward if dir > 0 and backward
// if dir < 0. It returns the new index.
int line_jump(struct line* l, int at, int dir) {
	if (dir == 0) {
		return at; // Nothing to do
	}
	if (dir > 0) {
		dir = 1;
	}
	if (dir < 0) {
		dir = -1;
	}

	int inword = 0;
	for (at += dir; at >= 0 && at <= l->len; at += dir) {
		char c = l->chars[at];
		if (c == ' ' || c == '\t') { // Whitespace char
			if (inword) {
				break;
			}
		} else {
			if (inword) {
				if (at == 0 || at == l->len) {
					break;
				}
			} else {
				inword = 1;
			}
		}
	}

	return at;
}
