#include <stdlib.h>
#include <string.h>

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

void line_insert_at(struct line* l, int i, char c) {
	if (i > l->len) {
		i = l->len;
	}

	if (l->len + 1 > l->cap) {
		int more = 1;
		if (l->cap > 0) {
			more = l->cap;
		}
		if (l->cap > 1024) {
			more = 1024;
		}
		l->cap += more;

		l->chars = realloc(l->chars, l->cap);
	}

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
