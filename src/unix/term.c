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

	ansi_setup();
}

void term_close() {
	tcsetattr(fileno(stdin), TCSANOW, &term_original);

	ansi_close();
}

void term_flush() {
	fflush(stdout);
}

int term_width() {
	return (int) term_size.ws_col;
}

int term_height() {
	return (int) term_size.ws_row;
}
