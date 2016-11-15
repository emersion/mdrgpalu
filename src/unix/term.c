struct termios term_original;

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
}

void term_close() {
	tcsetattr(fileno(stdin), TCSANOW, &term_original);
}
