FILE* (*clipboard_open)(char* mode);
int (*clipboard_close)(FILE* s);

void clipboard_init() {
	char* display = getenv("DISPLAY");
	if (display != NULL) { // Use xclip
		clipboard_open = clipboard_xclip_open;
		clipboard_close = clipboard_xclip_close;
	} else {
		clipboard_open = clipboard_internal_open;
		clipboard_close = clipboard_internal_close;
	}
}
