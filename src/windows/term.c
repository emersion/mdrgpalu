void term_setup() {
	HANDLE stdinh = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(stdinh, ENABLE_PROCESSED_INPUT);
}

void term_close() {
	// TODO
}
