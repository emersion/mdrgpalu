// See http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// See https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_codes
#define CODE_CUU 'A' // Cursor Up
#define CODE_CUD 'B' // Cursor Down
#define CODE_CUF 'C' // Cursor Forward
#define CODE_CUB 'D' // Cursor Backward
#define CODE_CNL 'E' // Cursor Next Line
#define CODE_CPL 'F' // Cursor Preceding Line
#define CODE_CHA 'G' // Cursor Character Absolute
#define CODE_CUP 'H' // Cursor Position

#define CODE_CURSOR_HIDE "?25l"
#define CODE_CURSOR_SHOW "?25h"

#define MODIFIER_SHIFT      2
#define MODIFIER_ALT        3
#define MODIFIER_ALT_SHIFT  4
#define MODIFIER_CTRL       5
#define MODIFIER_CTRL_SHIFT 6

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
				s->params[paramslen] = atoi(n);
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
