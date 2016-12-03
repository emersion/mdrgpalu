// clipboard_init initializes the clipboard. No clipboard_* function can be
// called before this one.
void clipboard_init();

// clipboard_open opens the clipboard for reading or writing.
FILE* clipboard_open(char* mode);

// clipboard_close closes a clipboard opened with clipboard_open.
int clipboard_close(FILE* s);
