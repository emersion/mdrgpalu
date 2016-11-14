FILE* clipboard_open(char* mode) {
	char* cmd;
	if (mode[0] == 'r') {
		cmd = "xclip -selection clipboard -o";
	} else {
		cmd = "xclip -selection clipboard -i";
	}

	return popen(cmd, mode);
}

int clipboard_close(FILE* p) {
	return pclose(p);
}

char* clipboard_get() {
	FILE* p = clipboard_open("r");
	if (p == NULL) {
		return NULL;
	}

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

	int err = clipboard_close(p);
	if (err) {
		free(s);
		return NULL;
	}
	return s;
}
