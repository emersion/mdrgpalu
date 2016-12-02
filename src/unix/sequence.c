// See http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// See https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes

#define CSI "\033[" // Control Sequence Introducer

#define CODE_CUU 'A' // Cursor Up
#define CODE_CUD 'B' // Cursor Down
#define CODE_CUF 'C' // Cursor Forward
#define CODE_CUB 'D' // Cursor Backward
#define CODE_CNL 'E' // Cursor Next Line
#define CODE_CPL 'F' // Cursor Preceding Line
#define CODE_CHA 'G' // Cursor Character Absolute
#define CODE_CUP 'H' // Cursor Position
#define CODE_DECDC '~' // Delete Column

#define CODE_ED "2J" // Erase display
#define CODE_EL "2K" // Erase line

#define CODE_CURSOR_HIDE "?25l"
#define CODE_CURSOR_SHOW "?25h"

#define CODE_ALTSCREEN_ENABLE "?47h"
#define CODE_ALTSCREEN_DISABLE "?47l"

#define CODE_MODIFYOTHERKEYS_ENABLE ">4;2m"
#define CODE_MODIFYOTHERKEYS_DISABLE ">4;0m"

#define ANSI_MODIFIER_SHIFT      2
#define ANSI_MODIFIER_ALT        3
#define ANSI_MODIFIER_ALT_SHIFT  4
#define ANSI_MODIFIER_CTRL       5
#define ANSI_MODIFIER_CTRL_SHIFT 6

// Text formats
#define FORMAT_RESET 0
#define FORMAT_BOLD 1
#define FORMAT_DIM 2
#define FORMAT_ITALIC 3
#define FORMAT_UNDERLINE 4
#define FORMAT_REVERSE 7

#define FORMAT_FOREGROUND 30
#define FORMAT_BACKGROUND 40

// Colors
#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_MAGENTA 5
#define COLOR_CYAN 6
#define COLOR_WHITE 7

#define COLOR_EXTENDED 8

// Extended color modes
#define COLOR_EXTENDED_RGB 2
#define COLOR_EXTENDED_256 5

void print_escape(char* seq) {
	printf("%s%s", CSI, seq);
}

void print_format(unsigned char format) {
	printf("%s%dm", CSI, format);
}

void print_foreground(unsigned char color) {
	if (color >= COLOR_EXTENDED) {
		printf("%s%d;%d;%dm", CSI, FORMAT_FOREGROUND + COLOR_EXTENDED, COLOR_EXTENDED_256, color);
	} else {
		print_format(FORMAT_FOREGROUND + color);
	}
}

void print_background(unsigned char color) {
	if (color >= COLOR_EXTENDED) {
		printf("%s%d;%d;%dm", CSI, FORMAT_BACKGROUND + COLOR_EXTENDED, COLOR_EXTENDED_256, color);
	} else {
		print_format(FORMAT_BACKGROUND + color);
	}
}

int parse_modifiers(int input) {
	int output = 0;
	if (input == ANSI_MODIFIER_SHIFT || input == ANSI_MODIFIER_ALT_SHIFT || input == ANSI_MODIFIER_CTRL_SHIFT) {
		output |= MODIFIER_SHIFT;
	}
	if (input == ANSI_MODIFIER_ALT || input == ANSI_MODIFIER_ALT_SHIFT) {
		output |= MODIFIER_ALT;
	}
	if (input == ANSI_MODIFIER_CTRL || input == ANSI_MODIFIER_CTRL_SHIFT) {
		output |= MODIFIER_CTRL;
	}
	return output;
}

struct sequence {
	char code;
	int params[3];
};

struct sequence* sequence_parse() {
	struct sequence* s = (struct sequence*) malloc(sizeof(struct sequence));
	s->params[0] = 0;
	s->params[1] = 0;
	s->params[2] = 0;

	int c;
	int paramslen = 0;
	char n[8];
	int i = 0;
	while (1) {
		c = getchar();
		if (c < 0) {
			return NULL;
		}

		if (c >= '0' && c <= '9') {
			if ((uint) i > sizeof(n)-1) {
				return NULL;
			}
			n[i] = c;
			i++;
		} else {
			if (i > 0) {
				n[i] = '\0';

				if (paramslen * sizeof(int) > sizeof(s->params)) {
					return NULL;
				}
				s->params[paramslen] = (int) strtoul(n, NULL, 10);
				paramslen++;

				i = 0;
			}

			if (c != ';') {
				s->code = c;
				return s;
			}
		}
	}
}

void sequence_free(struct sequence* s) {
	free(s);
}
