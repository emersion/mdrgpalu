#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
	#define POSIX
	#include <sys/termios.h>
	#include <sys/ioctl.h>
#elif _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#include "line.c"
#include "selection.c"
#include "buffer.c"
#include "io.c"
#include "event.c"
#include "autocomplete.c"

// TODO: create windows counterparts for these
#include "unix/format.c"
#include "unix/sequence.c"
#include "unix/clipboard_internal.c"
#include "unix/clipboard_xclip.c"
#include "unix/clipboard.c"
#include "unix/buffer.c"
#include "unix/event.c"

#ifdef POSIX
	#include "unix/term.c"
#elif _WIN32
	#include "windows/term.c"
#endif

#include "editor.c"

int main(int argc, char** argv) {
	struct trie_node* root = NULL;
	root = trie_node_insert(root, "sava", 4);
	root = trie_node_insert(root, "sava", 4);
	root = trie_node_insert(root, "mdr", 3);
	root = trie_node_insert(root, "savamdr", 7);
	root = trie_node_insert(root, "mdsaas", 6);
	char** list = malloc(10 * sizeof(char*));
	int n = trie_node_list(root, list, 10);
	printf("%d\n", n);
	for (int i = 0; i < n; i++) {
		printf("%s\n", list[i]);
	}
	return 0;

	setvbuf(stdin, NULL, _IONBF, 0); // Turn off buffering for stdin
	term_setup();
	term_cursor_toggle(0);
	clipboard_init();
	int exitcode = editor_main(argc, argv);
	term_cursor_toggle(1);
	term_close();
	return exitcode;
}
