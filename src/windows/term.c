void setup_term() {
	HANDLE stdinh = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(stdinh, ENABLE_PROCESSED_INPUT);
}
