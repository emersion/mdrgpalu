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

int line_read_range_from(struct line* l, int at, int len, FILE* f) {
	while (1) {
		int c = fgetc(f);
		if (c == EOF) {
			break;
		}
		int err = ferror(f);
		if (err) {
			return err;
		}

		if (c == '\r') {
			continue;
		}
		if (c == '\n') {
			return 0;
		}

		line_realloc(l);
		l->chars[l->len] = (char) c;
		l->len++;
	}

	return 0;
}

int line_read_from(struct line* l, FILE* f) {
	return line_read_range_from(l, 0, -1, f);
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

void line_insert_at(struct line* l, int i, char c) {
	if (i > l->len) {
		i = l->len;
	}

	line_realloc(l);

	if (i < l->len) {
		memmove(&l->chars[i+1], &l->chars[i], l->len-i);
	}

	l->chars[i] = c;
	l->len++;
}

int line_remove_at(struct line* l, int i) {
	if (l->len == 0) {
		return -1;
	}
	if (i < 0) {
		i = 0;
	}
	if (i >= l->len) {
		i = l->len-1;
	}

	char c = l->chars[i];
	memmove(&l->chars[i], &l->chars[i+1], l->len-i-1);
	l->len--;
	return c;
}
