struct termios term_original;
struct winsize term_size;

static void term_load_size() {
	ioctl(fileno(stdout), TIOCGWINSZ, &term_size);
}

void term_setup() {
	struct termios t;
	tcgetattr(fileno(stdin), &t);
	term_original = t;
	t.c_iflag |= IGNBRK;
	t.c_iflag &= ~(INLCR | IXON | IXOFF);
	t.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	tcsetattr(fileno(stdin), TCSANOW, &t);

	term_load_size();
	signal(SIGWINCH, term_load_size);

	//print_escape(CODE_ALTSCREEN_ENABLE);
	print_escape(CODE_MODIFYOTHERKEYS_ENABLE);
}

void term_close() {
	tcsetattr(fileno(stdin), TCSANOW, &term_original);

	print_escape(CODE_MODIFYOTHERKEYS_DISABLE);
	print_escape(CODE_ALTSCREEN_DISABLE);
}

void term_clear_screen() {
	print_escape(CODE_ED);
}

void term_clear_line() {
	print_escape(CODE_EL);
}

void term_flush() {
	fflush(stdout);
}

void term_cursor_toggle(int show) {
	if (show) {
		print_escape(CODE_CURSOR_SHOW);
	} else {
		print_escape(CODE_CURSOR_HIDE);
	}
}

void term_cursor_move(int x, int y) {
	if (x < 0 || y < 0) {
		return;
	}
	printf("%s%d;%d%c", CSI, y+1, x+1, CODE_CUP);
}

int term_width() {
	return (int) term_size.ws_col;
}

int term_height() {
	return (int) term_size.ws_row;
}
