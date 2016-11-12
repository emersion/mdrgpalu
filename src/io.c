// buffer_read_stream reads f until EOF and replaces data in the buffer.
int buffer_read_stream(struct buffer* b, FILE* f) {
	buffer_reset(b);

	char buf[1024];
	while (!feof(f)) {
		int n = fread(buf, sizeof(char), sizeof(buf)/sizeof(char), f);
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

// buffer_write_stream writes the buffer's data to f.
int buffer_write_stream(struct buffer* b, FILE* f) {
	char buf[1024];
	int n = 0; // Number of bytes written to buf
	for (struct line* l = b->first; l != NULL; l = l->next) {
		int len = l->len + 1; // Length of the line that will be written (line + \n)
		int i = 0; // Number of bytes written from l->chars
		while (i < len) {
			int ncpy = sizeof(buf) - n; // Number of bytes that will be written to buf
			if (len - i < ncpy) {
				ncpy = len - i;
			}
			if (i + ncpy == len) {
				// Last char is \n, not included in l->chars
				memcpy(&buf[n], &l->chars[i], ncpy-1);
				buf[n+ncpy-1] = '\n';
			} else {
				memcpy(&buf[n], &l->chars[i], ncpy);
			}
			n += ncpy;
			i += ncpy;

			if (n == sizeof(buf)) {
				// Buffer is full, flush to file
				fwrite(&buf, sizeof(char), n, f);
				int err = ferror(f);
				if (err) {
					return err;
				}
				n = 0;
			}
		}
	}

	// Is there something left that needs to be written in buf?
	if (n > 0) {
		fwrite(&buf, sizeof(char), n, f);
		return ferror(f);
	} else {
		return 0;
	}
}
