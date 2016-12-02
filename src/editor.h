struct editor {
	struct buffer* buf;
	char* filename;
	int saved;
	char* status;
	int exitcode;
};

struct editor* editor_new();
void editor_free(struct editor* e);
int editor_open(struct editor* e, char* filename);
int editor_save(struct editor* e);
char* editor_prompt(struct editor* e, char* prompt, int (*autocomplete)(char* val, char** results, int cap));
char* editor_prompt_filename(struct editor* e, char* prompt);
int editor_prompt_bool(struct editor* e, char* prompt);
void editor_set_status(struct editor* e, char* status);
void editor_quit(struct editor* e);
void editor_print(struct editor* e);
int editor_main(int argc, char** argv);
