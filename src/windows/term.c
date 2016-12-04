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

	GetConsoleMode(term_stdin, &term_cm_original);
	SetConsoleMode(term_stdin, ENABLE_PROCESSED_INPUT);
}

void term_close() {
	SetConsoleMode(term_stdin, term_cm_original);
}

void term_clear_screen() {
	COORD origin = {term_csbi.srWindow.Left, term_csbi.srWindow.Top};
	DWORD n = term_width() * term_height();
	DWORD written;
	FillConsoleOutputCharacter(term_stdout, (TCHAR) ' ', n, origin, &written);
}

void term_clear_line() {
	// TODO
}

void term_flush() {
	fflush(stdout);
}

void term_cursor_toggle(int show) {
	CONSOLE_CURSOR_INFO ci = {100, (BOOL) show};
	SetConsoleCursorInfo(term_stdout, &ci);
}

void term_cursor_move(int x, int y) {
	COORD pos = {term_csbi.srWindow.Left + x, term_csbi.srWindow.Top + y};
	SetConsoleCursorPosition(term_stdout, pos);
}

int term_width() {
	return term_csbi.srWindow.Right - term_csbi.srWindow.Left + 1;
}

int term_height() {
	return term_csbi.srWindow.Bottom - term_csbi.srWindow.Top + 1;
}

void term_set_title(char* title) {
	SetConsoleTitle((LPCTSTR) title);
}
