void setup_term() {
	struct termios t;
	tcgetattr(fileno(stdin), &t);
	t.c_iflag |= IGNBRK;
	t.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);
	t.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	tcsetattr(fileno(stdin), TCSANOW, &t);
}
