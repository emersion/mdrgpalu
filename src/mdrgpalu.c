#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <ctype.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__APPLE__) && defined(__MACH__)
	#define OS_DARWIN
	#define OS_BSD
	#define OS_POSIX
#elif defined(_WIN32)
	#define OS_WINDOWS
#elif defined(__unix__)
	#define OS_UNIX
	#define OS_POSIX

	#include <sys/param.h>
	#if defined(BSD)
		#define OS_BSD
	#elif defined(__linux__)
		#define OS_LINUX
	#endif
#endif

#if defined(OS_POSIX)
	#include <sys/termios.h>
	#include <sys/ioctl.h>
#elif defined(OS_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#if defined(OS_WINDOWS)
	#define PATH_SEPARATOR '\\'
#else
	#define PATH_SEPARATOR '/'
#endif

#include "status.h"
#include "term.h"
#include "trie.h"
#include "clipboard.h"
#include "editor.h"
#include "command.h"
#include "ansi/ansi.h"

#include "utf8.c"
#include "line.c"
#include "selection.c"
#include "buffer.c"
#include "io.c"
#include "event.c"
#include "trie.c"

#include "ansi/ansi.c"
#include "ansi/event.c"
#include "ansi/term.c"

// TODO: create windows counterparts for these
#include "unix/status.c"
#include "unix/buffer.c"
#include "unix/editor.c"

#if defined(OS_POSIX)
	#include "unix/term.c"
	#if defined(OS_UNIX)
		#include "unix/clipboard_internal.c"
		#include "unix/clipboard_xclip.c"
		#include "unix/clipboard.c"
	#elif defined(OS_BSD)
		#include "bsd/clipboard.c"
	#endif
#elif defined(OS_WINDOWS)
	#include "windows/term.c"
	#include "windows/clipboard.c"
#endif

#include "command.c"
#include "editor.c"

int main(int argc, char** argv) {
	setvbuf(stdin, NULL, _IONBF, 0); // Turn off buffering for stdin
	term_setup();
	term_cursor_toggle(0);
	clipboard_init();
	int exitcode = editor_main(argc, argv);
	term_clear_screen();
	term_cursor_toggle(1);
	term_close();
	return exitcode;
}
