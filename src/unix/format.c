#define FORMAT_CLEAR   "2J"
#define FORMAT_RESET   "0m"
#define FORMAT_DIM     "2m"
#define FORMAT_REVERSE "7m"

void print_escape(char* seq) {
	printf("\033[%s", seq);
}

void print_format(char* seq, char* text) {
	print_escape(seq);
	printf(text);
	print_escape(FORMAT_RESET);
}
