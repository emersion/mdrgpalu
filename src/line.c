// A line is a sequence of characters between two newlines.
struct line {
	struct line* prev; // The previous line
	struct line* next; // The next line

	char32_t* chars; // An array of characters contained in this line, excluding the trailing \n
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

// line_walk_line gets the i-th line after (or before) l. If no such line
// exists, it returns the closest one.
struct line* line_walk_line(struct line* l, int i) {
	while (i != 0) {
		if (i > 0) {
			i--;
			if (l->next == NULL) {
				return l;
			}
			l = l->next;
		} else {
			i++;
			if (l->prev == NULL) {
				return l;
			}
			l = l->prev;
		}
	}
	return l;
}

// line_walk_char gets the position of the i-th character after (or before) the
// first character of l. If no such character exists, it selects the closest
// one.
void line_walk_char(struct line* l, int i, struct line** ol, int* och) {
	while (i != 0) {
		// We are at the first character of line l
		if (i > 0) { // Move forward
			if (i > l->len) { // Move to next line
				if (l->next != NULL) {
					i -= l->len+1;
					l = l->next;
				} else {
					*och = l->len;
					break;
				}
			} else {
				*och = i;
				break;
			}
		} else { // Move backward, to previous line
			if (l->prev != NULL) {
				i += l->prev->len+1;
				l = l->prev;
			} else {
				*och = 0;
				break;
			}
		}
	}

	*ol = l;
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

		l->chars = (char32_t*) realloc(l->chars, l->cap * sizeof(char32_t));
	}
}

// line_read_from reads a single line from s and adds it to l, at the specified
// position. Returns the number of bytes read.
int line_read_from(struct line* l, int at, FILE* s) {
	int N = 0;
	char32_t c;
	while (1) {
		int n = utf8_read_from(&c, s);
		if (n == EOF) {
			if (N == 0) {
				// No bytes read and we reach EOF, return EOF
				return EOF;
			}
			break;
		}

		N += n;
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
			memmove(&l->chars[at+1], &l->chars[at], (l->len-at)*sizeof(char32_t));
		}

		l->chars[at] = c;
		at++;
		l->len++;
	}

	return N;
}

// line_write_range_to writes a range of characters from l to s. The range
// starts at index at and ends len bytes after. Returns a non-zero value on
// error.
int line_write_range_to(struct line* l, int at, int len, FILE* s) {
	if (len < 0) {
		len = l->len+1;
	}

	if (at < l->len) {
		int n = len;
		if (n > l->len) {
			n = l->len;
		}
		for (int i = 0; i < n; i++) {
			int err = utf8_write_to(l->chars[at+i], s);
			if (err) {
				return err;
			}
		}
	}

	if (at + len > l->len && l->next != NULL) {
		// Write a trailing \n only if there's another line after
		int c = fputc('\n', s);
		if (c == EOF) {
			return 1;
		}
	}
	return 0;
}

// line_write_to writes l's content to s. Returns a non-zero value on error.
int line_write_to(struct line* l, FILE* s) {
	return line_write_range_to(l, 0, -1, s);
}

// line_insert_char inserts c at the position at. It moves characters after at
// if necessary.
void line_insert_char(struct line* l, int at, char32_t c) {
	if (at > l->len) {
		at = l->len;
	}

	line_realloc(l);

	if (at < l->len) {
		memmove(&l->chars[at+1], &l->chars[at], (l->len-at) * sizeof(char32_t));
	}

	l->chars[at] = c;
	l->len++;
}

// line_delete_range deletes a range of characters from l. The range starts at
// index at and ends len bytes after.
void line_delete_range(struct line* l, int at, int len) {
	if (at + len > l->len) {
		len = l->len - at;
	}

	memmove(&l->chars[at], &l->chars[at+len], (l->len-at-len)*sizeof(char32_t));
	l->len -= len;
	// TODO: realloc to free memory
}

// line_delete_char deletes a the character at index at from l and returns its
// value.
char32_t line_delete_char(struct line* l, int at) {
	if (l->len == 0) {
		return -1;
	}
	if (at < 0) {
		at = 0;
	}
	if (at >= l->len) {
		at = l->len-1;
	}

	char32_t c = l->chars[at];
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
			next->chars = (char32_t*) malloc(next->len * sizeof(char32_t));
			memcpy(next->chars, &l->chars[at], next->len * sizeof(char32_t));
			l->len = at;
		}
	}

	return next;
}

// line_join joins l and other to l.
void line_join(struct line* l, struct line* other) {
	// Copy the other line to the end of this one
	int len = l->len + other->len;
	if (other->len > 0) {
		if (l->cap < len) {
			l->cap = len;
			l->chars = (char32_t*) realloc(l->chars, l->cap * sizeof(char32_t));
		}
		memcpy(&l->chars[l->len], other->chars, other->len * sizeof(char32_t));
	}
	l->len = len;
}

// line_jump jumps to a neighbor word. It jumps forward if dir > 0 and backward
// if dir < 0. It returns the difference between at and the new index.
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
	int d = 0;
	for (d += dir; at + d >= 0 && at + d <= l->len; d += dir) {
		char32_t c = l->chars[at + d];
		if (c == ' ' || c == '\t') { // Whitespace char
			if (inword) {
				break;
			}
		} else {
			if (inword) {
				if (at + d == 0 || at + d == l->len) {
					break;
				}
			} else {
				inword = 1;
			}
		}
	}

	return d;
}
