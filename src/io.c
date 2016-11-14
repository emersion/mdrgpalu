#define BUF_SIZE 1024

// buffer_read_from reads f until EOF and appends data in the buffer.
int buffer_read_from(struct buffer* b, FILE* f) {
	char buf[BUF_SIZE];
	while (!feof(f)) {
		int n = fread(&buf, sizeof(char), sizeof(buf)/sizeof(char), f);
		int err = ferror(f);
		if (err) {
			return err;
		}

		// TODO: optimize this
		for (int i = 0; i < n; i++) {
			buffer_insert_char(b, buf[i]);
		}
	}

	return 0;
}

// buffer_write_to writes the buffer's data to f.
int buffer_write_to(struct buffer* b, FILE* f) {
	for (struct line* l = b->first; l != NULL; l = l->next) {
		int err = line_write_to(l, f);
		if (err) {
			return err;
		}
	}

	return 0;
}

int buffer_write_selection_to(struct buffer* b, FILE* f) {
	struct line* l = b->sel->line;
	int from = b->sel->ch;
	int len = b->sel->len;
	while (len > 0 && l != NULL) {
		int n = len;
		if (n > l->len) {
			n = l->len+1; // line + \n
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
