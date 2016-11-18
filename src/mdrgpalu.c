#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

#include "line.c"
#include "selection.c"
#include "buffer.c"
#include "io.c"

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
	#include <sys/termios.h>
	#include "unix/term.c"
#elif _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include "windows/term.c"
#endif

// TODO: create windows counterparts for these
#include "unix/format.c"
#include "unix/sequence.c"
#include "unix/clipboard_internal.c"
#include "unix/clipboard_xclip.c"
#include "unix/clipboard.c"
#include "unix/buffer.c"

#include "editor.c"

int main(int argc, char** argv) {
	setvbuf(stdin, NULL, _IONBF, 0); // Turn off buffering
	term_setup();
	print_escape(CODE_ALTSCREEN_ENABLE);
	print_escape(CODE_CURSOR_HIDE);
	clipboard_init();
	int exitcode = editor_main(argc, argv);
	term_close();
	print_escape(CODE_CURSOR_SHOW);
	print_escape(CODE_ALTSCREEN_DISABLE);
	return exitcode;
}
