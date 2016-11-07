#include <stdlib.h>
#include <string.h>

struct line {
	struct line* next;
	struct line* prev;

	char* chars;
	int len;
	int cap;
};

struct line* line_new() {
	struct line* l = (struct line*) malloc(sizeof(struct line));
	l->chars = NULL;
	l->len = 0;
	l->cap = 0;
	return l;
}

void line_grow(struct line* l, int more) {
	l->cap += more;
	l->chars = realloc(l->chars, l->cap);
}

void line_append(struct line* l, char c) {
	if (l->len + 1 > l->cap) {
		int more = l->cap;
		if (more == 0) {
			more = 1;
		}
		if (more > 1024) {
			more = 1024;
		}
		line_grow(l, more);
	}

	l->chars[l->len] = c;
	l->len++;
}

void line_insert_at(struct line* l, int i, char c) {
	if (i >= l->len) {
		return line_append(l, c);
	}

	if (l->len + 1 > l->cap) {
		line_grow(l, l->cap);
	}

	memmove(&l->chars[i+1], &l->chars[i], l->len-i);
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
