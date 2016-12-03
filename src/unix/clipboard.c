FILE* (*_clipboard_open)(char* mode);
int (*_clipboard_close)(FILE* s);

FILE* clipboard_open(char* mode) {
	return _clipboard_open(mode);
}

int clipboard_close(FILE* s) {
	return _clipboard_close(s);
}

void clipboard_init() {
	char* display = getenv("DISPLAY");
	if (display != NULL) { // Use xclip
		_clipboard_open = clipboard_xclip_open;
		_clipboard_close = clipboard_xclip_close;
	} else {
		_clipboard_open = clipboard_internal_open;
		_clipboard_close = clipboard_internal_close;
	}
}
