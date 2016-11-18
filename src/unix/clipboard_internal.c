char* clipboard_buffer = NULL;
size_t clipboard_len = 0;

FILE* clipboard_internal_open(char* mode) {
	if (mode[0] == 'r') {
		return fmemopen(clipboard_buffer, clipboard_len, mode);
	} else {
		free(clipboard_buffer);
		clipboard_len = 0;
		return open_memstream(&clipboard_buffer, &clipboard_len);
	}
}

int clipboard_internal_close(FILE* s) {
	return fclose(s);
}
