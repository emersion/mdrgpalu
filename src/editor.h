// An editor.
struct editor {
	// The editor's buffer.
	struct buffer* buf;
	// The currently opened file, if any.
	char* filename;
	// True if the buffer is in sync with the file on disk.
	int saved;

	// The editor's status message.
	char* status;
	// If positive, the editor will exit with this code.
	int exitcode;
};

// editor_new allocates a new editor.
struct editor* editor_new();

// editor_free deallocates an editor.
void editor_free(struct editor* e);

// editor_open opens the file filename in the editor.
int editor_open(struct editor* e, char* filename);

// editor_save saves the editor's content.
int editor_save(struct editor* e);

// editor_prompt requests an input from the user. prompt is the message
// displayed, autocomplete is a function that takes a value entered by the user,
// puts into results a list of at most cap suggestions and returns the number
// of suggestions stored.
char* editor_prompt(struct editor* e, char* prompt, int (*autocomplete)(char* val, char** results, int cap));

// editor_prompt_filename requests a filename from the user.
char* editor_prompt_filename(struct editor* e, char* prompt);

// editor_prompt_bool requests a boolean from the user.
int editor_prompt_bool(struct editor* e, char* prompt);

// editor_set_status sets the editor's status text.
void editor_set_status(struct editor* e, char* status);

// editor_quit tells the editor to exit.
void editor_quit(struct editor* e);

// editor_print prints the editor's interface to the standard output.
void editor_print(struct editor* e);

// editor_main starts a new editor.
int editor_main(int argc, char** argv);
