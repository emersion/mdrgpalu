// term_setup initializes the terminal.
void term_setup();

// term_close reconfigures the terminal as it was before term_setup.
void term_close();

// term_clear_screen clears the terminal's screen.
void term_clear_screen();

// term_clear_line clears the terminal's current line.
void term_clear_line();

// term_flush ensures that the terminal's screen is in sync.
void term_flush();

// term_cursor_toggle shows or hides the terminal's cursor.
void term_cursor_toggle(int show);

// term_cursor_move moves the terminal's cursor to x, y. Coordinates must be
// positive or null.
void term_cursor_move(int x, int y);

// term_width returns the terminal's width.
int term_width();

// term_height returns the terminal's height.
int term_height();
