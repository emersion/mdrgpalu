// term_setup initializes the terminal.
void term_setup();

// term_close reconfigures the terminal as it was before term_setup.
void term_close();

// term_clear clears the terminal's screen.
void term_clear();

// term_cursor_toggle shows or hides the terminal's cursor.
void term_cursor_toggle(int show);

// term_width returns the terminal's width.
int term_width();

// term_height returns the terminal's height.
int term_height();
