struct editor {
	struct buffer* buf;
	char* filename;
	char* status;
	int exitcode;
};

struct editor* editor_new();
void editor_free(struct editor* e);
int editor_open(struct editor* e, char* filename);
char* editor_prompt(struct editor* e, char* prompt);
char* editor_prompt_filename(struct editor* e, char* prompt);
void editor_set_status(struct editor* e, char* status);
void editor_quit(struct editor* e);
int editor_main(int argc, char** argv);
