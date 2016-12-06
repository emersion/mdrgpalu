void term_clear_screen() {
	ansi_escape(CODE_ED);
}

void term_clear_line() {
	ansi_escape(CODE_EL);
}

void term_cursor_toggle(int show) {
	if (show) {
		ansi_escape(CODE_CURSOR_SHOW);
	} else {
		ansi_escape(CODE_CURSOR_HIDE);
	}
}

void term_cursor_move(int x, int y) {
	if (x < 0 || y < 0) {
		return;
	}
	printf("%s%d;%d%c", CSI, y+1, x+1, CODE_CUP);
}

void term_set_title(char* title) {
	printf("%s%d;%s%c", OSC, OSC_SET_TITLE, title, OSC_END);
}
