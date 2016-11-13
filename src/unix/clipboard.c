char* clipboard_get() {
	FILE* p = popen("xclip -selection clipboard -o", "r");

	int cap = 32;
	int len = 0;
	char* s = (char*) malloc(cap);
	while (!feof(p)) {
		int n = fread(&s[len], sizeof(char), cap-len, p);
		int err = ferror(p);
		if (err) {
			free(s);
			return NULL;
		}
		len += n;

		if (len == cap) {
			if (cap > 1024) {
				cap += 1024;
			} else {
				cap *= 2;
			}
			s = (char*) realloc(s, cap);
		}
	}

	int err = pclose(p);
	if (err) {
		free(s);
		return NULL;
	}
	return s;
}

int clipboard_put(char* s) {
	FILE* p = popen("xclip -selection clipboard -i", "w");

	int n = fputs(s, p);
	if (n == EOF) {
		pclose(p);
		return 1;
	}

	return pclose(p);
}
