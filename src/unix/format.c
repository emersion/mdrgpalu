#define CODE_CLEAR "2J"

#define FORMAT_RESET 0
#define FORMAT_BOLD 1
#define FORMAT_DIM 2
#define FORMAT_ITALIC 3
#define FORMAT_UNDERLINE 4
#define FORMAT_REVERSE 7

void print_escape(char* seq) {
	printf("\033[%s", seq);
}

void print_format(char format) {
	printf("\033[%dm", format);
}
