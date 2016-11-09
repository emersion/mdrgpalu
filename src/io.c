int buffer_read_stream(struct buffer* b, FILE* f) {
	buffer_reset(b);

	char buf[1024];
	int n;
	int err;
	while (!feof(f)) {
		n = fread(buf, sizeof(char), sizeof(buf)/sizeof(char), f);
		err = ferror(f);
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

int buffer_write_stream(struct buffer* b, FILE* f) {
	int err;
	for (struct line* l = b->first; l != NULL; l = l->next) {
		fwrite(l->chars, sizeof(char), l->len, f);
		err = ferror(f);
		if (err) {
			return err;
		}
	}

	return 0;
}
