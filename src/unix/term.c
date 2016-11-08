void setup_term() {
	struct termios t;
	tcgetattr(0, &t);
	t.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
	tcsetattr(0, TCSANOW, &t);
}
