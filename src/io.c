int buffer_read_stream(struct buffer* b, FILE* f) {
	buffer_reset(b);

	char buf[1024];
	int n;
	int err;
	while (!feof(f)) {
		n = fread(buf, 1, sizeof(buf), f);
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
