// Text formats
#define FORMAT_RESET 0
#define FORMAT_BOLD 1
#define FORMAT_DIM 2
#define FORMAT_ITALIC 3
#define FORMAT_UNDERLINE 4
#define FORMAT_REVERSE 7

// Colors
#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_MAGENTA 5
#define COLOR_CYAN 6
#define COLOR_WHITE 7

void ansi_setup();
void ansi_close();

void ansi_format(unsigned char format);
void ansi_foreground(unsigned char color);
void ansi_background(unsigned char color);
