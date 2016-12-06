HANDLE term_stdin;
HANDLE term_stdout;
CONSOLE_SCREEN_BUFFER_INFO term_csbi;
DWORD term_cm_original;

static void term_load_csbi() {
	GetConsoleScreenBufferInfo(term_stdout, &term_csbi);
}

void term_setup() {
	term_stdin = GetStdHandle(STD_INPUT_HANDLE);
	term_stdout = GetStdHandle(STD_OUTPUT_HANDLE);

	term_load_csbi();
	// TODO: call term_load_csbi when terminal size changes

	GetConsoleMode(term_stdin, &term_cm_original);
	SetConsoleMode(term_stdin, ENABLE_PROCESSED_INPUT);
}

void term_close() {
	SetConsoleMode(term_stdin, term_cm_original);
}

void term_flush() {
	fflush(stdout);
}

/*void term_cursor_toggle(int show) {
	CONSOLE_CURSOR_INFO ci = {100, (BOOL) show};
	SetConsoleCursorInfo(term_stdout, &ci);
}

void term_cursor_move(int x, int y) {
	COORD pos = {x, y};
	SetConsoleCursorPosition(term_stdout, pos);
}*/

int term_width() {
	return term_csbi.dwSize.X;
}

int term_height() {
	return term_csbi.dwSize.Y;
}

/*void term_set_title(char* title) {
	SetConsoleTitle((LPCTSTR) title);
}*/
